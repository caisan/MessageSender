#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <limits.h>
#include <map>
#include <list>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <boost/intrusive_ptr.hpp>
#include "Thread.h"

using namespace std;

class Messenger;
class Pipe;
struct Connection
  {
      Messenger *msgr;
   public:
      Pipe *pipe;
  
   public:
      Connection(Messenger *m)
          : msgr(m),
            pipe(NULL) {}
  
      ~Connection() {
          if (pipe)
            pipe->put();
      }
};

typedef boost::intrusive_ptr<Connection> ConnectionRef;

class Message {
protected:
      char header[1024];
      char footer[1024];
  
  public:
      Message(){
          memset(&header, 0, sizeof(header));
          memset(&footer, 0, sizeof(footer));
      }
  
    unsigned get_priority() const { return header.priority;  }

};

struct entity_addr_t {
      uint32_t type;
      uint32_t nonce;
      union {
          sockaddr_storage addr;
          /*Only support IPv4 currently*/
          sockaddr_in addr4;
      };
      unsigned int addr_size() const {
          switch (addr.ss_family) {
              case AF_INET:
                  return sizeof(addr4);
                  break;
          }
          return sizeof(addr);
      }
      entity_addr_t() : type(0), nonce(0) {
          memset(&addr, 0, sizeof(addr));
      }
  };

struct entity_inst_t {
   entity_addr_t addr;
  
   entity_inst_t(const entity_addr_t & a) : addr(a) {}
   entity_inst_t() {};
  };

class Messenger{
  private:
  list<Dispatcher*> dispatchers;
  
  protected:
      entity_inst_t my_inst;
      bool started;
  
  public:
      Messenger(entity_addr_t a)
      : my_inst()
      {
          my_inst.addr = a;
      }
      virtual ~Messenger() {}
  
      /*Create new Messenger*/
      static Messenger *create();
  
  protected:
      /*set messenger address*/
      void set_addr(const entity_addr_t& a) { my_inst.addr = a; }
  
  public:
      virtual int bind(const entity_addr_t& bind_addr) = 0;
    virtual int rebind(const set<int>& null_ports) {return -EOPNOTSUPP;}
  
      virtual int start() { started = true; return 0; }
  
      virtual int send_message(Message* m, const entity_inst_t& dest) = 0;
  
    virtual Pipe* get_connection(const entity_inst_t& dest) = 0;
  
  };

class TinyMsg : public Messenger {
  public:
  
      TinyMsg();
      virtual ~TinyMsg();
  
      int bind(const entity_addr_t& bind_addr);
      int rebind(const set<int>& null_port);
  private:
      set<Pipe*> pipes;
  
  protected:
      virtual void ready();
  
  private:
      class ReciveThread : public Thread {
          TinyMsg *msgr;
      public:
          ReciveThread(TinyMsg *m) : msgr(m) {}
          void* entry() {
            msgr->recive_entry();
              return 0;
          }
      }revice_thread;
  
      Pipe *connect_entity(const entity_addr_t& addr);
  
      int _send_message(Message *m, Connection *con);
  
      void submit_message(Message *m, Connection *con,
                          const entity_addr_t& addr);
  
      virtual Pipe* get_connection(const entity_inst_t& dest);
      Pipe *_connect(const entity_addr_t& addr, int type, Connection *con, Message *first);
  
  };

class Pipe {
    /**
     * The Reader thread handles all read off the socket.
     * Message, and also acks, other protocol
     * */
    class Reader : public Thread {
        Pipe *pipe;
    public:
        Reader(Pipe *p) : pipe(p) {}
        void *entry() { pipe->reader(); return 0; }
    } reader_thread;
    friend class Reader;

    class Writer : public Thread {
        Pipe *pipe;
    public:
        Writer(Pipe *p) : pipe(p) {}
        void *entry() {pipe->writer(); return 0;}
    } writer_thread;
    friend class Writer;

    
    public:
        Pipe(TinyMsg *r, int st, Connection *con);
        ~Pipe();

        TinyMsg *msgr;
        uint64_t cond_id;

        enum {
            STATE_ACCEPTING,
            STATE_CONNECTING,
            STATE_OPEN,
            STATE_CLOSED,
            STATE_CLOSING,
            STATE_WAIT
        };
        static const char *get_state_name(int s) {
            switch (s) {
                case STATE_ACCEPTING: return "accepting";
                case STATE_CONNECTING: return "connection";
                case STATE_OPEN: return "open";
                case STATE_CLOSED: return "closed";
                case STATE_CLOSING: return "closing";
                case STATE_WAIT: return "wait";
                default: return "UNKNOWN";
            }
        }
        int state;
        const char *get_state_name() {
            return get_state_name(state);
        }
        entity_addr_t peer_addr;

    protected:
        friend class TinyMsg;
        Connection connection_state;

        bool reader_running;
        bool writer_running;
        map< int, std::list<Message*> > out_q;
        std::list<Message*> sent;

        void set_sock_opts();
        int accept();
        int connect();
        void reader();
        void writer();

        int read_message();
        int write_message();

        int do_sendmsg();
    public:
        Pipe(const Pipe& other);
        
        void start_reader();
        void start_writer();
        void join_reader();
        
        entity_addr_t& get_peer_addr() {return peer_addr;}

        void set_peer_addr(const entity_addr_t& addr) {
            //if (&peer_addr != &addr)
                peer_addr = addr;
        }

        void register_pipe();
        void unregister_pipe();

        void stop();

        void _send(Message *m) {
            out_q[m->get_priority()].push_back(m);
        }
};





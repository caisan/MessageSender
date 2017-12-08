#ifndef MSG_PIPE_H
#define MSG_PIPE_H

#include "Messenger.h"
#include "Thread.h"
#include <list>
using namespace std;
class TinyMsg;


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

        void register_pipe();
        void unregister_pipe();

        void stop();

        void _send(Message *m) {
            out_q[m->get_priority()].push_back(m);
        }





};




#endif

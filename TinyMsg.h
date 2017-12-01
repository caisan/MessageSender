#include "Messenger.h"

class TinyMsg : public Messenger {
public:

    TinyMsg();
    virtual ~TinyMsg();

    int bind(const entity_addr_t& bind_addr);
    int rebind(const set<int>& null_port);




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



};

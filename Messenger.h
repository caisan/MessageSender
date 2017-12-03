#ifndef MESSENGER_H
#define MESSENGER_H

#include <iostream>
#include <map>
#include <list>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Message.h"
using namespace std;


/*Linux socket*/
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

    virtual get_connection(const entity_inst_t& dest) = 0;

};






#endif

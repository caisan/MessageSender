#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <string.h>
#include <boost/intrusive_ptr.hpp>
#include "Pipe.h"

//abstract Connection

class Messenger;

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


//Abstract Message Class
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




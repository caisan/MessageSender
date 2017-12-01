#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <string.h>
#include <boost/intrusive_ptr.hpp>


//abstract Connection

class Messenger;

struct Connection
{
    Messenger *msgr;

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

};


#include "TinyMsg.h"
#include "Pipe.h"

Pipe* TinyMsg::Messenger::get_connection(const entity_inst_t& dest)
{
    while (true) {
        //TODO: Check wheather already exists pipe.
        /*Createing new pipe*/
        Pipe *pipe = new Pipe();

    }
}

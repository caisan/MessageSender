#include "libmessage.h"

Pipe* TinyMsg::_connect(const entity_addr_t& addr, int type,
                        Connection *con, Message *first)
{
    std::cout<<"Tiny message: Creating Pipe and registering"<<std::endl;
    Pipe *pipe = new Pipe(this, Pipe::STATE_CONNECTING, con);
    pipe->set_peer_addr(addr);
    pipe->start_writer();
    if (first)
        pipe->_send(first);
    pipe->register_pipe();
    pipes.insert(pipe);
    return pipe;
}



Pipe* TinyMsg::Messenger::get_connection(const entity_inst_t& dest)
{
    Pipe *pipe = NULL;
    pipe = _connect(dest.addr, dest.name.type(), NULL, NULL);
    if (pipe->connection_state)
        return pipe->connection_state;
}

/********************************************
 * Pipe start reader and writer
 * */
void Pipe::start_reader()
{
    assert(!read_running);
    reader_running = true;
    reader_thread.create();
}

void Pipe::start_writer()
{
    assert(!writer_running);
    writer_thread.create();
}

void Pipe::reader()
{

}
void Pipe::writer()
{
    while (state != STATE_CLOSED) {

        //if connect ?
        if (state == STATE_CONNECTING) {
            connect();
            continue;
        }

        if (state == STATE_CLOSING) {
            //TODO: write tag if closing
            continue;
        }


    }
}

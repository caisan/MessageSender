#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <limits.h>
#include "Pipe.h"
#include "Message.h"
#include "TinyMsg.h"


/****
 * Pipe
 *****/
Pipe::Pipe(TinyMsg *r, int st, Connection *con)
    : reader_thread(this), writer_thread(this),
    msgr(r),
    reader_running(false),
    writer_running(false)
{
    if (con) {
        connection_state = con;
        connection_state->reset_pipe(this);
    } else {
        connection_state = new Connection(msgr);
        connection_state->pipe = get();
    }
}

Pipe::~Pipe()
{
    assert(out_q.empty());
}


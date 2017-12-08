#include "Thread.h"
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>


pid_t _get_id(void)
{
#ifdef __linux
    return syscall(SYS_gettid);
#else
    return -ENOSYS;
#endif
}

Thread::Thread()
    : thread_id(0),
      pid(0)
{
}

Thread::~Thread()
{
}

void *Thread::_entry_func(void *arg) {
    void *r = ((Thread*)arg)->entry_wrapper();
    return r;
}
void *Thread::entry_wrapper()
{
    int p = _get_id();
    if (p > 0)
        pid = p;
    return entry();
}
const pthread_t &Thread::get_thread_id()
{
    return thread_id;
}
bool Thread::is_started()
{
    return thread_id != 0;
}
bool Thread::am_i()
{
    return (pthread_self() == thread_id);
}
int Thread::kill(int signal)
{
    if (thread_id)
        return pthread_kill(thread_id, signal);
    else
        return -EINVAL;
}

unsigned long _page_mask = ~(unsigned long)(sysconf(_SC_PAGESIZE) -1);
#define PAGE_MASK _page_mask

int Thread::try_create(size_t stacksize)
{
    pthread_attr_t *thread_attr = NULL;
    stacksize &= PAGE_MASK;
    if (stacksize) {
        thread_attr = (pthread_attr_t*)malloc(sizeof(pthread_attr_t));
        if (!thread_attr)
            return -ENOMEM;
        pthread_attr_init(thread_attr);
        pthread_attr_setstacksize(thread_attr, stacksize);
    }

    int r;
    //TODO: Set the signal mask we want to block
    r = pthread_create(&thread_id, thread_attr, _entry_func, (void*)this);

    if (thread_attr)
        free(thread_attr);
    return r;
}

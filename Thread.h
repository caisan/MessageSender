#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread {

    private:
        pthread_t thread_id;
        pid_t pid;

        void *entry_wrapper();

    public:
        Thread(const Thread& t);
        Thread();
        virtual ~Thread();

    protected:
        virtual void *entry() = 0;
    private:
        static void *_entry_func(void *arg);

    public:
        const pthread_t &get_thread_id();
        bool is_started();
        bool am_i();
        int kill(int signal);
        int try_create(size_t stacksize);
        int create(size_t stacksize = 0);
        int join();
        int detach();
};
#endif

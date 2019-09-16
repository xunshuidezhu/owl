#include "thread.h"
#include "current_thread.h"
#include "error_handler.h"
#include <sys/prctl.h>
#include <sys/types.h>

using namespace owl::base;

namespace CurrentThread {
__thread pthread_t cached_tid = 0;

__thread int t_tidStringLength = 6;
}

void CurrentThread::get_cached_tid()
{
    if (cached_tid == 0) {
        cached_tid = pthread_self();
    }
}

int CurrentThread::tid()
{
    get_cached_tid();
    return cached_tid;
}

Thread::Thread(callback _cb, string default_name = "default_thread")
    : thread_id(0)
    , started(false)
    , joined(false)
    , thread_name(default_name)
    , callback_func(_cb)
    , c_d_latch(5)

{
    set_thread_name(default_name);
    set_callback(_cb);
}

Thread::~Thread()
{
    if (!started) {
        error_handler("thread isn`t started");
    } else if (!joined) {
        join();
    } else {
        detach();
    }
}
void Thread::start()
{
    thread_arg_data* data = new thread_arg_data(callback_func, thread_name, process_id, c_d_latch);
    if (!started) {
        pthread_create(&thread_id, NULL, &start_routine, data);
    } else {
        error_handler("thread has started!");
        return;
    }
    started = true;
}

void Thread::set_callback(callback cb)
{
    callback_func = cb;
}

pthread_t Thread::get_threadid()
{
    return thread_id;
}

void Thread::detach()
{
    pthread_detach(thread_id);
}
void Thread::join()
{
    if (started) {
        error_handler("thread isn`t started");
        return;
    }
    if (!joined) {
        error_handler("thread has joined");
        return;
    }
    pthread_join(thread_id, NULL);
}

pid_t Thread::get_process_id()
{
    return getpid();
}

Thread::callback Thread::get_callback()
{
    return callback_func;
}
struct thread_arg_data {
    Thread::callback thread_cb;
    string thread_name;
    pid_t thread_process_id;
    CountDownLatch& count_down_latch;
    thread_arg_data(Thread::callback cb, string name, pid_t pid, CountDownLatch& latch)
        : thread_cb(cb)
        , thread_name(name)
        , thread_process_id(pid)
        , count_down_latch(latch)
    {
    }
    void callback_run()
    {
        count_down_latch.count_down();
        CurrentThread::cached_thread_name = thread_name;
        prctl(PR_SET_NAME, CurrentThread::cached_thread_name);
        thread_cb();
    }
};
void* Thread::start_routine(void* arg)
{
    thread_arg_data* data = (thread_arg_data*)arg;
    data->callback_run();
    return NULL;
}

void Thread::set_thread_name(string name)
{
    thread_name.clear();
    thread_name = name;
}

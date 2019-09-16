/*
 * @brief base thread class 
 * 
 * */
#ifndef OWL_THREAD_H
#define OWL_THREAD_H
#include "count_down_latch.h"
#include "error_handler.h"
#include "noncopyable.h"
#include <functional>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace owl {
namespace base {
    //struct thread_arg_data;
    class Thread : noncopyable {
    public:
        typedef std::function<void()> callback;
        Thread(callback _cb, string default_name = "default_thread");
        ~Thread();
        static void* start_routine(void* arg);
        void start();
        void join();
        void detach();
        void set_callback(callback func);
        callback get_callback();
        pid_t get_process_id();
        CountDownLatch c_d_latch;
        void set_thread_name(string name);
        pthread_t get_threadid();

    private:
        pthread_t thread_id;
        pid_t process_id;
        string thread_name;
        bool started;
        bool joined;
        callback callback_func;
    };
    class ThreadData {
    public:
        Thread thread;
    };
} //namespace base
} //namespace owl

#endif
/**
 * @breif 条件变量类
 * @author OWL
 * */
#ifndef OWL_BASE_THREAD_CONDITION
#define OWL_BASE_THREAD_CONDITION
#include "noncopyable.hpp"
#include "thread_mutex.h"
#include <pthread.h>

namespace owl {
namespace base {
    class Condition : public noncopyable {
    public:
        explicit Condition(Mutex& _mutex);
        ~Condition();
        void wait();
        void notify();
        void notify_broadcast();
        void init();
        void set_mutex(Mutex _mutex);

    private:
        Mutex& mutex;
        pthread_cond_t cond;
    };
} //namespace base
} //namespace owl

#endif // OWL_BASE_THRAD_CONDITION
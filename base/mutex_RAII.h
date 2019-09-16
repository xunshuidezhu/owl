#ifndef OWL_BASE_MUTEX_RAII_H
#define OWL_BASE_MUTEX_RAII_H
#include "noncopyable.h"
#include "thread_mutex.h"

namespace owl {
namespace base {
    /**
     * 通过RAII控制mutex生存
     * 
     * **/
    class Mutex_Guard : noncopyable {
    public:
        explicit Mutex_Guard(Mutex& _mutex)
            : mutex(_mutex)
        {
            mutex.lock();
        }
        ~Mutex_Guard()
        {
            mutex.unlock();
        }

    private:
        Mutex& mutex;
    };
} //namespace base
} // namespace owl
#endif // OWL_BASE_MUTEX_RAII_H
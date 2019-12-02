/***************************************************************************
 * @brief Mutex Class
 * @author OWL
 * 
 * *************************************************************************/
#ifndef OWL_BASE_THREAD_MUTEX
#define OWL_BASE_THREAD_MUTEX
#include "noncopyable.hpp"
#include <pthread.h>
#include <string>

using namespace std;
namespace net {
static int current_thread[10];
}
namespace owl {
namespace base {
    class Mutex : noncopyable {
    public:
        Mutex(string name = "default");
        ~Mutex();
        void init();
        void destroy();
        void lock();
        void unlock();
        void set_mutex_name(string name);
        string get_mutex_name();
        pthread_mutex_t get_mutex();

    private:
        pthread_mutex_t mutex;
        string mutex_name;
    };
} // namespace base
} // namespace owl

#endif
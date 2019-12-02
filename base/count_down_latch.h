#ifndef OWL_BASE_COUNT_DOWN_LATCH
#define OWL_BASE_COUNT_DOWN_LATCH
#include "noncopyable.hpp"
#include "thread_condition.h"
#include "thread_mutex.h"

using namespace owl::base;

class CountDownLatch : public noncopyable {
public:
    explicit CountDownLatch(int _count);
    void wait();
    void count_down();

private:
    int count;
    Mutex mutex;
    Condition cond;
};
#endif // OWL_BASE_COUNT_DOWN_LATCH


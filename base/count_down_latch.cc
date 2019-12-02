/**
 * @brief count down latch
 * @author owl
 * */
#include "count_down_latch.h"

CountDownLatch::CountDownLatch(int _count)
    : count(_count)
    , cond(mutex)
{
}

void CountDownLatch::wait()
{
    mutex.lock();
    while (count > 0) {
        cond.wait();
    }
}

void CountDownLatch::count_down()
{
    mutex.lock();
    count--;
    if (count == 0) {
        cond.notify_broadcast();
    }
}

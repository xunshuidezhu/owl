/**
 * @breif 条件变量类
 * @author OWL
 * */
#include "thread_condition.h"

using namespace owl::base;

Condition::Condition(Mutex& _mutex)
    : mutex(_mutex)
{
    init();
}
void Condition::set_mutex(Mutex _mutex)
{
    //FIXME noncopyable
}
void Condition::init()
{
    pthread_cond_init(&cond, NULL);
}

void Condition::wait()
{
    pthread_cond_wait(&cond, &mutex.get_mutex());
}

void Condition::notify()
{
    pthread_cond_signal(&cond);
}

void Condition::notify_broadcast()
{
    pthread_cond_broadcast(&cond);
}
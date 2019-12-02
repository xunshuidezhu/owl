/***************************************************************************
 * @brief Mutex Class
 * @author OWL
 * *************************************************************************/
#include "thread_mutex.h"

using namespace owl::base;

void Mutex::init()
{
    pthread_mutex_init(&mutex, NULL);
}

void Mutex::destroy()
{
    pthread_mutex_destroy(&mutex);
}

Mutex::Mutex(string name)
{
    init();
    set_mutex_name(name);
}

Mutex::~Mutex()
{
    destroy();
}

void Mutex::lock()
{
    pthread_mutex_lock(&mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&mutex);
}

void Mutex::set_mutex_name(string name)
{
    mutex_name = name;
}

string Mutex::get_mutex_name()
{
    return mutex_name;
}

pthread_mutex_t Mutex::get_mutex()
{
    return mutex;
}
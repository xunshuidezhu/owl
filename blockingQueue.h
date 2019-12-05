#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include "demultiplexer.h"

using namespace std;

struct Data {
    handle_t handle;
};

class Mutex {
public:
    Mutex()
    {
        pthread_mutex_init(&_mutex, 0);
        pthread_cond_init(&_read, 0);
        pthread_cond_init(&_write, 0);
    }
    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }
    void wait_read()
    {
        pthread_cond_wait(&_read, &_mutex);
    }
    void wait_write()
    {
        pthread_cond_wait(&_write, &_mutex);
    }
    void notify_read()
    {
        pthread_cond_signal(&_read);
    }
    void notify_write()
    {
        pthread_cond_signal(&_write);
    }
    void notify_all()
    {
        pthread_cond_broadcast(&_read);
        pthread_cond_broadcast(&_write);
    }
    void destroy()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_read);
        pthread_cond_destroy(&_write);
    }
    pthread_mutex_t _mutex;
    pthread_cond_t _read;
    pthread_cond_t _write;
};

class MutexGuard {
public:
    MutexGuard()
    {
        _mutex = new Mutex;
    }
    ~MutexGuard()
    {
        delete _mutex;
    }
    Mutex* _mutex;
};
class Queue {
public:
    Queue(int capacity)
        : _capacity(capacity)
        , _len(0)
    {
    }
    bool is_full();
    bool is_empty();
    void enQueue(const Data& data);
    Data deQueue();
    int _len;
    int _capacity;
    deque<Data> queue;
};

bool Queue::is_empty()
{
    if (_len == 0) {
        return true;
    } else {
        return false;
    }
}

bool Queue::is_full()
{
    if (_len >= _capacity) {
        return true;
    } else {
        return false;
    }
}

void Queue::enQueue(const Data& data)
{
    if (_len >= _capacity) {
        cout << "queue full" << endl;
        return;
    }
    cout << "produce data: " << data.handle << endl;
    queue.push_back(data);
    _len++;
}

Data Queue::deQueue()
{
    if (_len == 0) {
        cout << "queue empty" << endl;
    }
    Data res = queue.front();
    cout << "consume data: " << res.handle << endl;
    queue.pop_front();
    _len--;
    return res;
}
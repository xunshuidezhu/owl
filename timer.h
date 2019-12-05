
/************************************************************
 *  head only 小根堆计时器
 * **********************************************************/
#ifndef TIMER_H
#define TIMER_H

#include <arpa/inet.h>
#include <functional>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>

class HeapTimer;
//客端数据
class ClientData {
public:
    ClientData(char* address)
        : target_count(0)
    {
        strcpy(ip_address, address);
    }

private:
    char ip_address[32];

    int target_count;
};
typedef std::function<void(ClientData*)> callback;

//计时器
class Timer {
public:
    Timer() {}

    virtual ~Timer() {}
};

//堆计时器
class HeapTimer : public Timer {
public:
    HeapTimer(int ms_delay)
    {
        gettimeofday(&_expire, NULL);
        _expire.tv_usec += ms_delay * 1000;
        if (_expire.tv_usec > 1000000) {
            _expire.tv_sec += _expire.tv_usec / 1000000;
            _expire.tv_usec %= 1000000;
        }
    }

    void setCallback(callback cb)
    {
        _cb = cb;
    }

public:
    struct timeval _expire;
    callback _cb;
    ClientData* _userData;
    ~HeapTimer()
    {
    }
};
//计时器容器
class TimerContainer {
public:
    virtual ~TimerContainer() {}
};
//小根堆计时器容器

class HeapTimerContainer : public TimerContainer {
public:
    HeapTimerContainer(int cap = 1)
        : capacity(cap)
        , cur_size(0)
    {
        array = new HeapTimer*[capacity];
        if (!array) {
            throw std::exception();
        }
        for (int i = 0; i < capacity; ++i) {
            array[i] = NULL;
        }
    }

    ~HeapTimerContainer()
    {
        for (int i = 0; i < cur_size; ++i) {
            delete array[i];
        }
        delete[] array;
    }

public:
    int get_cursize()
    {
        return cur_size;
    }

    void add_timer(HeapTimer* timer)
    {
        if (!timer) {
            return;
        }
        if (cur_size >= capacity) {
            resize();
        }
        int hole = cur_size++;
        int parent = 0;
        for (; hole > 0; hole = parent) {
            parent = (hole - 1) / 2;
            if (timercmp(&(array[parent]->_expire), &(timer->_expire), <)) {
                break;
            }
            array[hole] = array[parent];
        }
        array[hole] = timer;
    }
    void del_timer(HeapTimer* timer)
    {
        if (!timer) {
            return;
        }
        // lazy delelte
        timer->_cb = NULL;
    }
    int top(struct timeval& time_top) const
    {
        if (empty()) {
            return 0;
        }
        time_top = array[0]->_expire;
        return 1;
    }
    void pop_timer()
    {
        if (empty()) {
            return;
        }
        if (array[0]) {
            delete array[0];
            array[0] = array[--cur_size];
            percolate_down(0);
        }
    }
    void tick()
    {
        HeapTimer* tmp = array[0];
        struct timeval cur;
        gettimeofday(&cur, NULL);
        while (!empty()) {
            if (!tmp) {
                break;
            }
            if (timercmp(&cur, &(tmp->_expire), <)) {
                break;
            }
            if (array[0]->_cb) {
                array[0]->_cb(array[0]->_userData);
            }
            pop_timer();
            tmp = array[0];
        }
    }
    bool empty() const
    {
        return cur_size == 0;
    }
    HeapTimer** get_heap_array()
    {
        return array;
    }

private:
    void percolate_down(int hole)
    {
        HeapTimer* temp = array[hole];
        int child = 0;
        for (; ((hole * 2 + 1) <= (cur_size - 1)); hole = child) {
            child = hole * 2 + 1;
            if ((child < (cur_size - 1)) && timercmp(&(array[child + 1]->_expire), &(array[child]->_expire), <)) {
                ++child;
            }
            if (timercmp(&(array[child]->_expire), &(temp->_expire), <)) {
                array[hole] = array[child];
            } else {
                break;
            }
        }
        array[hole] = temp;
    }
    void resize()
    {
        HeapTimer** temp = new HeapTimer*[2 * capacity];
        for (int i = 0; i < 2 * capacity; ++i) {
            temp[i] = NULL;
        }
        if (!temp) {
            throw std::exception();
        }
        capacity = 2 * capacity;
        for (int i = 0; i < cur_size; ++i) {
            temp[i] = array[i];
        }
        delete[] array;
        array = temp;
    }

private:
    HeapTimer** array;
    int capacity;
    int cur_size;
};

#endif
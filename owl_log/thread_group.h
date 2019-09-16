#ifndef __THREAD_GROUP_H
#define __THREAD_GROUP_H

#include <pthread.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <ctime>
#include <functional>
#include <iterator>
#include <vector>
//#include "mynoncopyable.h"

#ifndef func_type
#define func_type void*
#endif

class ThreadMutex {
private: // noncopyable
    ThreadMutex(const ThreadMutex&);
    ThreadMutex& operator=(const ThreadMutex&);

public:
    ThreadMutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    ~ThreadMutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    void enterCriticalSection()
    {

        lock();
    }
    void leaveCriticalSection()
    {

        unlock();
    }

private:
    pthread_mutex_t m_mutex;
};

template <typename MutexType>
class ScopedLock {
private: // noncopyable
    ScopedLock(const ScopedLock&);
    ScopedLock& operator=(const ScopedLock&);

public:
    ScopedLock(MutexType& threadlock)
        : m_mutex(&threadlock)
        , isLocked(false)
    {
        lock();
    }

    ~ScopedLock()
    {
        if (ownsLock()) {
            m_mutex->unlock();
        }
    }

    void lock()
    {
        if (m_mutex == 0) {
            return;
        }
        if (ownsLock()) {
            return;
        }
        m_mutex->lock();
        isLocked = true;
    }

    void unlock()
    {
        if (m_mutex == 0) {
            return;
        }
        if (!ownsLock()) {
            return;
        }
        m_mutex->unlock();
        isLocked = false;
    }

protected:
    bool ownsLock() const
    {
        return isLocked;
    }

private:
    MutexType* m_mutex;
    bool isLocked;
};

inline void SleepSeconds(unsigned int counts)
{
    sleep(counts);
}

inline void SleepMicroseconds(time_t counts)
{

    usleep(counts);
}

class ThreadSleep {
public:
    static void seconds(unsigned int counts)
    {
        SleepSeconds(counts);
    }
    static void microseconds(time_t counts)
    {
        SleepMicroseconds(counts);
    }

private:
    ThreadSleep(const ThreadSleep&);
    ThreadSleep& operator=(const ThreadSleep&);
};

class ThreadGroup {
private: // noncopyable
    ThreadGroup(const ThreadGroup&);
    ThreadGroup& operator=(const ThreadGroup&);

public:
    typedef pthread_t thread_type;

    typedef size_t size_type;

    ThreadGroup()
        : joinedSize(0)
    {
    }

    template <typename F, typename A>
    bool createThread(F threadfunc, const A& arg)
    //bool createThread(func_type threadfunc(void *), void * arg)
    {
        ScopedLock<ThreadMutex> lock(m_mutex); // auto lock and unlock
        thread_type handle;

        if (pthread_create(&handle, NULL, threadfunc, (void*)&arg) != 0) {
            return false;
        }

        m_threadHandle.push_back(handle);

        return true;
    }

    void join()
    {
        ScopedLock<ThreadMutex> lock(m_mutex);
        if (joinedSize == m_threadHandle.size())
            return;
        container_type::iterator start = m_threadHandle.begin();
        std::advance(start, joinedSize);
        std::for_each(start, m_threadHandle.end(),
            std::bind2nd(std::ptr_fun(pthread_join), (void*)0));
        joinedSize = m_threadHandle.size();
    }

    size_type size() const
    {
        ScopedLock<ThreadMutex> lock(m_mutex);
        return m_threadHandle.size();
    }

    ~ThreadGroup()
    {
    }

private:
    typedef std::vector<thread_type> container_type;
    container_type m_threadHandle;

    mutable ThreadMutex m_mutex;
    std::vector<thread_type>::size_type joinedSize;
};

#endif //__THREAD_GROUP_H
#ifndef REACTOR_H
#define REACTOR_H

#include <sys/epoll.h>

#include "timer.h"
#include "threadpool.h"


typedef int handle_t;

namespace reactor {
enum event_t {
    readEvent = 0x01,
    writeEvent = 0x02,
    errorEvent = 0x04,
    eventMask = 0xff
};
}
using namespace reactor;
class EventHandler {
public:
    virtual handle_t getHandle() const = 0;

    virtual void handleRead() = 0;

    virtual void handleWrite() = 0;

    virtual void handleError() = 0;

public:
    EventHandler(){};

    virtual ~EventHandler(){};
};

class Reactor {
public:
    class ReactorImpl;
    //CThreadPool threadpool;
private:
    ReactorImpl* _reactorImpl;

public:
    explicit Reactor();

    ~Reactor();

    int registerHandler(EventHandler* handler, event_t event);

    int removeHandler(EventHandler* handler);

    void handlerEvent();

    int registerTimerTask(HeapTimer* timer);

    void startThreadpool();

private:
    Reactor(const Reactor&) {} // 禁止拷贝

    Reactor& operator=(const Reactor&) {} // 禁止赋值
};
#endif
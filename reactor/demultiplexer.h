#ifndef DEMULTIPLEXER_H
#define DEMULTIPLEXER_H

#include "reactor.h"
#include "timer.h"

#include <map>

namespace reactor {
typedef enum Exit {
    success = 1,
    fail = 0
} exit_t;
}

using namespace reactor;

class EventDemultiplexer {
public:
    virtual ~EventDemultiplexer() {}

    virtual int waitEvent(std::map<handle_t, EventHandler*>*, int timeout, TimerContainer*, callback& c) = 0;

    virtual exit_t addEvent(handle_t handle, event_t event) = 0;

    virtual exit_t delEvent(handle_t) = 0;

    EventDemultiplexer(){};
};

class EpollEventDemultiplexer : public EventDemultiplexer {
public:
    EpollEventDemultiplexer();

    virtual int waitEvent(std::map<handle_t, EventHandler*>*, int timeout, TimerContainer*, callback& c) override;

    virtual exit_t addEvent(handle_t handle, event_t event_t) override;

    virtual exit_t delEvent(handle_t) override;

    virtual ~EpollEventDemultiplexer() override;

private:
    int _epoll_fd;

    int _fd_num;

    static int _epoll_size;
};

#endif

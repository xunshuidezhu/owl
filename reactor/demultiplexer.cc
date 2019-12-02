#include "demultiplexer.h"
#include "reactor.h"
#include "timer.h"

#include <errno.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <unistd.h>

using namespace reactor;

int EpollEventDemultiplexer::_epoll_size = 10;

EpollEventDemultiplexer::EpollEventDemultiplexer()
    : _fd_num(0)
{
    _epoll_fd = epoll_create(_epoll_size);
}

EpollEventDemultiplexer::~EpollEventDemultiplexer()
{
    close(_epoll_fd);
}

exit_t EpollEventDemultiplexer::addEvent(handle_t handle, event_t event)
{
    epoll_event epollEvent;
    epollEvent.data.fd = handle;
    epollEvent.events = 0;

    if (event & readEvent) {
        epollEvent.events |= EPOLLIN;
    }

    if (event & writeEvent) {
        epollEvent.events |= EPOLLOUT;
    }

    if (event & errorEvent) {
        epollEvent.events |= EPOLLERR;
    }

    epollEvent.events |= EPOLLONESHOT;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, handle, &epollEvent) != 0) {
        if (errno == ENOENT) {
            if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handle, &epollEvent) != 0) {
                return fail;
            }
            _fd_num++;
        }
    }
    return success;
}

exit_t EpollEventDemultiplexer::delEvent(handle_t handle)
{
    epoll_event epollEvent;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, handle, &epollEvent) != 0) {
        return fail;
    }

    _fd_num--;
    return success;
}

int EpollEventDemultiplexer::waitEvent(std::map<handle_t, EventHandler*>* handlers, int timeout, TimerContainer* timer, callback& c)
{
    epoll_event epollEvents[_fd_num];

    int num = epoll_wait(_epoll_fd, epollEvents, _fd_num, timeout);
    if (num > 0) {
        for (int i = 0; i < _fd_num; i++) {
            handle_t handle = epollEvents[i].data.fd;
            if (handlers->find(handle) != handlers->end()) {

            } else {
                return fail;
            }
            if (epollEvents[i].events & EPOLLERR || epollEvents[i].events & EPOLLHUP) {
                (*handlers)[handle]->handleError();
            } else {
                if (epollEvents[i].events & EPOLLIN) {
                    (*handlers)[handle]->handleRead();
                }
                if (epollEvents[i].events & EPOLLOUT) {
                    (*handlers)[handle]->handleWrite();
                }
            }
        }
    }
    if (timer->haveTimer()) {
        timer->doCallback(c);
    }
    return success;
}
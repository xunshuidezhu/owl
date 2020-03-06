#include "common.h"
#include "global.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

using namespace reactor;
Reactor g_reactor;
//#define g_reactor (*(sGlobal->g_reactor_ptr))

char g_buffer[1000];

class Request : public EventHandler {
private:
    handle_t _handle;

public:
    Request(handle_t handle)
    {
        printf("server start");
        _handle = handle;
        fcntl(_handle, F_SETFL, O_NONBLOCK);
    }
    //无健壮性处理

    virtual handle_t getHandle() const
    {
        return _handle;
    }

    virtual void handleError()
    {
    }

    virtual void handleWrite()
    {
    }

    virtual void handleRead() override
    {
        recv(_handle, g_buffer, 100, 0);

        //g_reactor.registerHandler(this, readEvent);
    }
};

class Server : public EventHandler {
public:
    handle_t _handle;
    const char* _ip;
    short _port;

public:
    Server(const char* ip = "127.0.0.1", short port = 8080)
        : _ip(ip)
        , _port(port)
    {
    }
    bool start()
    {
        _handle = socket(AF_INET, SOCK_STREAM, 0);
        fcntl(_handle, F_SETFL, O_NONBLOCK);
        int val = 1;
        socklen_t valLen = sizeof(val);
        setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, &val, valLen);
        struct sockaddr_in localAddr;
        struct sockaddr_in remoteAddr;

        memset(&localAddr, 0, sizeof(localAddr));
        localAddr.sin_family = AF_INET;
        localAddr.sin_port = htons(_port);
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(_handle, (sockaddr*)&localAddr, sizeof(localAddr));

        listen(_handle, 10);
        return 1;
    }

    virtual handle_t getHandle() const
    {
        return _handle;
    }

    virtual void handleError()
    {
    }

    virtual void handleWrite()
    {
    }

    virtual void handleRead() override
    {
        struct sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        int sock = accept(_handle, (sockaddr*)&addr, &addrlen);
        Request* request = new Request(sock);
        g_reactor.registerHandler(request, readEvent);
        //g_reactor.registerHandler(this, readEvent);
    }
};

int main()
{
    Server server;
    server.start();
    // printf("3");
    g_reactor.registerHandler(&server, readEvent);
    // printf("2")
    g_reactor.handlerEvent();
    // while (1) {
    //     g_reactor.handlerEvent();
    //     printf("1");
    //     //g_reactor.startThreadpool();
    //     sleep(1);
    // }
    g_reactor.removeHandler(&server);
    return 0;
}
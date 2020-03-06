#include "common.h"
#include "global.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
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
//reactor::Reactor g_reactor;
#define g_reactor (*(sGlobal->g_reactor_ptr))

char g_buffer[1000];

class Client : public EventHandler {
private:
    const char* _ip;
    int _port;
    handle_t _handle;

public:
    Client(const char* ip = "127.0.0.1", int port = 8080)
        : _ip(ip)
        , _port(port)
    {
    }
    //无健壮性处理
    bool start()
    {
        _handle = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in remoteAddr;
        memset(&remoteAddr, 0 , sizeof(remoteAddr));
        remoteAddr.sin_family = AF_INET;
        inet_aton("127.0.0.1", &remoteAddr.sin_addr);
        remoteAddr.sin_port = htons(8888);

        connect(_handle, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
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
        sprintf(g_buffer, "owl client send");
        while (1) {
            send(_handle, g_buffer, 1000, 0);  
            sleep(1);
        } 
        g_reactor.registerHandler(this, readEvent);
    }


    virtual void handleRead() override
    {
    }
};

int main()
{
    Client client;
    client.start();
    g_reactor.registerHandler(&client, writeEvent);

    while (1) {
        g_reactor.handlerEvent();
        //g_reactor.startThreadpool();
        sleep(1);
    }
    g_reactor.removeHandler(&client);
    return 0;
}
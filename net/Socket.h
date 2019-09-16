#ifndef OWL_NET_SOCKET_H
#define OWL_NET_SOCKET_H
#include <fcntl.h>
#include <sys/socket.h>

namespace owl {
namespace net {
    class Socket {
    public:
        explicit Socket(int _fd);
        int get_fd();
        void set_nonblock();

    private:
        int fd;
        bool stream;
    };
}
}

#endif
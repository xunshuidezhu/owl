#include <Socket.h>
using namespace owl::net;
Socket::Socket(int _fd)
    : fd(_fd)
    , stream(true)
{
    socket(fd, SOCK_STREAM, 0);
}

void Socket::set_nonblock()
{
    fcntl(F_GETFL);
}

int Socket::get_fd()
{
    return fd;
}

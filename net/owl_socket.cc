#include "owl_socket.h"
#include "owl_host_net.h"
#include <sys/socket.h>
#define NON_BLOCK 1
#define LOG_THREAD 1
#define CLOSE_EXEC 1
#define DEBUG 1
#define IPV4 1
using namespace owl;
net::Socket::Socket(int _fd)
    : fd(_fd)
    , stream(true)
    , listen_num(5)
{
    socket(fd, SOCK_STREAM, 0);
#if LOG_THREAD
    start_log();
#endif
}

int net::Socket::set_nonblock(int fd)
{
    int old_flags = fcntl(fd, F_GETFL);
    int new_flags = old_flags | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flags);
    return old_flags;
}

int net::Socket::set_close_on_exec(int fd)
{
    int old_flags = fcntl(fd, F_GETFL);
    int new_flags = old_flags | FD_CLOEXEC;
    fcntl(fd, F_SETFL, new_flags);
    return old_flags;
}

int net::Socket::get_fd() const
{
#ifdef DEBUG
    debug_log("<fd> : <%d>", fd);
#endif
    return fd;
}

int net::Socket::connect(int fd, struct sockaddr_in& addr)
{
    int ret = ::connect(fd, (sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
        err_log("<error> connect error");
    return ret;
}

void net::Socket::bind_addr(int fd, const struct sockaddr_in& addr)
{
    int ret = ::bind(fd, (sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
        err_log("<error> bind error");
}

int net::Socket::set_listen_num(int num = 5)
{
    listen_num = num;
}

void net::Socket::sock_listen(int fd)
{
    int ret = ::listen(fd, listen_num);
}

int net::Socket::accept(int sockfd, struct sockaddr_in* addr)
{
    socklen_t addr_len = sizeof(*addr);
    int connfd = ::accept(sockfd, (sockaddr*)addr, &addr_len);
#if NON_BLOCK
    set_nonblock(sockfd);
#endif
#if CLOSE_EXEC
    set_close_on_exec(sockfd);
#endif
    //TODO:errno
    if (connfd < 0) {
        err_log("<error> connfd error");
    }
}

void net::Socket::close(int sockfd)
{
    int ret = ::close(sockfd);
    if (ret == -1) {
        err_log("close sockfd error");
    }
}

void net::Socket::start_log()
{
    log.start();
}

void net::Socket::shutdown_write(int sockfd)
{
    int ret = ::shutdown(sockfd, SHUT_WR);
    if (ret == -1) {
        err_log("shutdown write error");
    }
}
//TODO:
void net::Socket::host_net_port(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
#if IPV4
    addr->sin_family = AF_INET;
#else
    addr->sin_family = AF_INET6;
#endif
    addr->sin_port = host_net::host_net_16(port);
    int ret = ::inet_aton(ip, &addr->sin_addr);
    if (ret < 0) {
        err_log("host_net_port");
    }
}
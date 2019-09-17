#ifndef OWL_NET_SOCKET_H
#define OWL_NET_SOCKET_H
#include "../owl_log/log.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

owllog::Log log("net.log");
owllog::Log* OWL = &log;

namespace owl {
namespace net {
    class Socket {
    public:
        explicit Socket(int _fd);
        ~Socket();
        int get_fd() const;
        int set_nonblock(int fd);
        int set_close_on_exec(int fd);
        int connect(int fd, sockaddr_in& addr);
        void bind_addr(int fd, const struct sockaddr_in& addr);
        void sock_listen(int fd);
        int set_listen_num(int num);
        int accept(int sockfd, struct sockaddr_in* addr);
        void close(int sockfd);
        void start_log();
        void shutdown_write(int sockfd);
        void net_host_port();
        void host_net_port(const char* ip, uint16_t port, struct sockaddr_in* addr);

    private:
        int fd;
        int listen_num;
        bool stream;
    };
}
}

#endif
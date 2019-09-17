#ifndef OWL_NET_ADDRESS
#define OWL_NET_ADDRESS
#include <netinet/in.h>
#include <string>

namespace owl {
namespace net {
    
    class Address {
    public:
        explicit Address(uint16_t port);
        Address(const std::string& ip, uint16_t port);
        Address(const struct sockaddr_in& _addr)
            : addr(_addr)
        {
        }
        std::string host_port();
        const struct sockaddr_in& get_addr();
    private:
        struct sockaddr_in addr;
    };

} // namespace net
} // namespace owl

#endif
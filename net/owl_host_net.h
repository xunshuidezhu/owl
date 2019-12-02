#ifndef OWL_HOST_NET_H
#define OWL_HSOT_NET_H
#include <netinet/in.h>
#include <stdint.h>
namespace owl {
typedef uint32_t net_addr32_t;
typedef uint16_t net_addr16_t;
typedef uint64_t net_addr64_t;
namespace host_net {

    /***
 * host to net
 * **/
    net_addr64_t host_net_64(net_addr64_t h_64) { return htobe64(h_64); }

    net_addr32_t host_net_32(net_addr32_t h_32) { return htonl(h_32); }

    net_addr16_t host_net_16(net_addr16_t h_16) { return htons(h_16); }
} // namespace host_net
/**
 * net to host
 * */
namespace net_host {
    net_addr64_t net_host_64(net_addr64_t n_64) { return be64toh(n_64); }

    net_addr32_t net_host_32(net_addr32_t n_32) { return ntohl(n_32); }

    net_addr16_t net_host16(net_addr16_t n_16) { return ntohs(n_16); }
} // namespace net_host
} // namespace owl
#endif
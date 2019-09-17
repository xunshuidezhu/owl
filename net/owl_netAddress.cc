#include "owl_netAddress.h"
#include "owl_host_net.h"
#include <string.h>
#define OWL_DEFAULT_IP INADDR_ANY
using namespace owl;

net::Address::Address(uint16_t port)
{
    memset(&addr, 0, sizeof(addr));

    addr.sin_addr.s_addr = host_net::host_net_32(OWL_DEFAULT_IP);
    addr.sin_family = AF_INET;
    addr.sin_port = host_net::host_net_16();
}

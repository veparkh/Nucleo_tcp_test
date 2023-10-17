#ifndef TCP_INCLUDE_SERVER_TCPSERVEROPTS_H_
#define TCP_INCLUDE_SERVER_TCPSERVEROPTS_H_

#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

    lwipthread_opts_t opts_s;
    struct ip4_addr ip_s, gateway_s, netmask_s;
    IP4_ADDR(&ip_s, 192, 168, 1, 123);
    IP4_ADDR(&gateway_s, 192, 168, 1, 1);
    IP4_ADDR(&netmask_s, 255, 255, 255, 0);
    uint8_t mac_address_s[6] = {0xC2, 0xAF, 0x51, 0x33, 0xCC, 0x02};

#endif /* TCP_INCLUDE_SERVER_TCPSERVEROPTS_H_ */

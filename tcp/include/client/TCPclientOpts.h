#ifndef TCP_INCLUDE_CLIENT_TCPCLIENTOPTS_H_
#define TCP_INCLUDE_CLIENT_TCPCLIENTOPTS_H_

#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

lwipthread_opts_t opts_c;
struct ip4_addr ip_c, gateway_c, netmask_ñ;

IP4_ADDR(&ip_c, 192, 168, 1, 124);
IP4_ADDR(&gateway_c, 192, 168, 1, 1);
IP4_ADDR(&netmask_ñ, 255, 255, 255, 0);
uint8_t mac_address_ñ[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};

#endif /* TCP_INCLUDE_CLIENT_TCPCLIENTOPTS_H_ */

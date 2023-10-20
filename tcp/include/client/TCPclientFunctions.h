#ifndef TCP_INCLUDE_CLIENT_TCPCLIENTFUNCTIONS_H_
#define TCP_INCLUDE_CLIENT_TCPCLIENTFUNCTIONS_H_


#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>





void up_callback_ñ(void *p);

void down_callback_ñ(void *p);

void modb_message(void);

void tcp_init_client();

#endif /* TCP_INCLUDE_CLIENT_TCPCLIENTFUNCTIONS_H_ */

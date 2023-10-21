#ifndef TCP_INCLUDE_CLIENT_TCPCLIENTFUNCTIONS_H_
#define TCP_INCLUDE_CLIENT_TCPCLIENTFUNCTIONS_H_


#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

#include "Requests.h"




void up_callback_c(void *p);

void down_callback_c(void *p);

void modb_message(void);

void tcp_init_client(void);

#endif /* TCP_INCLUDE_CLIENT_TCPCLIENTFUNCTIONS_H_ */

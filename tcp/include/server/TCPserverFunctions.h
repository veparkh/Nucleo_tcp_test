#ifndef TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_
#define TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_

#include "serial.h"
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "modbusFunc.h"



int write_log(struct netconn *conn);

int tcp_query_handler(char *query, struct netconn *conn);

void up_callback_s(void *p);
void down_callback_s(void *p);

void tcp_init_server(void);
#endif /* TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_ */

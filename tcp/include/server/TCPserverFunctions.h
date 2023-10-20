#ifndef TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_
#define TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_


#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

int write_log(struct netconn *conn, modbus_package *query);

int tcp_query_handler(modbus_package *query, struct netconn *conn);

void up_callback_s(void *p);
void down_callback_s(void *p);

void tcp_init_server();
#endif /* TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_ */

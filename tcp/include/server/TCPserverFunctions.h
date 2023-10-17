/*
 * serverFunctions.h
 *
 *  Created on: 7 окт. 2023 г.
 *      Author: vparx
 */

#ifndef TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_
#define TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_


int write_log(struct netconn *conn, modbus_package *query);

int tcp_query_handler(modbus_package *query, struct netconn *conn);

void up_callback_s(void *p);
void down_callback_s(void *p);


#endif /* TCP_INCLUDE_SERVER_TCPSERVERFUNCTIONS_H_ */

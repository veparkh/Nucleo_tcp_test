/*
 * TCPserverThreads.h
 *
 *  Created on: 7 окт. 2023 г.
 *      Author: vparx
 */

#ifndef TCP_INCLUDE_SERVER_TCPSERVERTHREADS_H_
#define TCP_INCLUDE_SERVER_TCPSERVERTHREADS_H_

void conn_handler(void *p);

void tcp_server(void *ip);

#endif /* TCP_INCLUDE_SERVER_TCPSERVERTHREADS_H_ */

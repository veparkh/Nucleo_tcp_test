/*
 * threads.h
 *
 *  Created on: 31 рту. 2023 у.
 *      Author: vparx
 */

#ifndef INCLUDE_THREADS_H_
#define INCLUDE_THREADS_H_

void conn_handler(void *p);

void tcp_server(void *ip);

void up_callback_s(void *p);
void down_callback_s(void *p);

#endif /* INCLUDE_THREADS_H_ */

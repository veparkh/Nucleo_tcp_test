#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>

#include "common.h"
#include "requestHandler.h"
#include "modbusFunc.h"

THD_WORKING_AREA(tcp_conn_handler1, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
THD_WORKING_AREA(tcp_conn_handler2, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
THD_WORKING_AREA(tcp_conn_handler3, 1024);
THD_WORKING_AREA(tcp_conn_handler4, 1024);
THD_WORKING_AREA(wa_tcp_server, 1024);




THD_FUNCTION(conn_handler, p){
	(void) p;
	chRegSetThreadName("conn_handler");
	msg_t cast_letter;
	modbus_package modbus_answer;
	modbus_package *query = NULL;
	struct netconn *conn;
	int recv_err_or_buflen;
	while(true)
	{
		chMBFetchTimeout (&mb_conn, &cast_letter, TIME_INFINITE);
		conn = (struct netconn*) cast_letter;
		while(true)
		{
			for(int i =0 ;i<300;i++){
				recv_err_or_buflen = read_data(conn, 1000, &query);
				if((!isConnection)||(recv_err_or_buflen>0)||(recv_err_or_buflen!=ERR_TIMEOUT))
					break;
			}
			dbgprintf("recv_err : %d\r\n",recv_err_or_buflen);
			uint16_t a = 0xA003;
			dbgprintf("%d\t%d\t%d\t%d\t%d\t%d\t\r\n",((uint8_t*)query)[0], ((uint8_t*)query)[1], ((uint16_t*)query)[0], *((uint8_t *)&a), *(((uint8_t *)&a) + 1), 5);
			if (recv_err_or_buflen > 0 && isConnection){

				change_endian(query);
				dbgprintf("%d %d %d %d",query->tid,query->pid,query->length,query->uid);
				if(is_modbus_package(query,recv_err_or_buflen)){
					dbgprintf("its modbus\r\n");
					modbus_query_handler(query, &modbus_answer);
					uint16_t length = (modbus_answer.length<<8|modbus_answer.length>>8)+6;
					dbgprintf("modbus length: %d\r\n",length);
					netconn_write(conn, &modbus_answer, length, NETCONN_NOCOPY);
				}
				else{
					change_endian(query);
					int tcp_error = tcp_query_handler(query, conn);
					dbgprintf("tcp_err : %d\r\n",tcp_error);
					if(tcp_error!=ERR_OK)
						break;
					}
			}
			else{
				break;
			}
		}
		netconn_close(conn);
		netconn_delete(conn);
		dbgprintf("Clear memory");
	}
}

THD_FUNCTION(tcp_server, ip) {

  palToggleLine(LINE_LED1);
  chRegSetThreadName("server");
  struct netconn *conn, *newconn;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, (struct ip4_addr *) ip, 80);
  netconn_listen(conn);
  while (true) {
    err_t accept_err = netconn_accept(conn, &newconn);
    dbgprintf("accept_error = %d\n\r",accept_err);
    if (accept_err != ERR_OK)
    	continue;
    chMBPostTimeout (&mb_conn, (msg_t)newconn, TIME_IMMEDIATE);
  }
}


#include <stdbool.h>
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <threadsTCP.h>
#include "modbusGet.h"
#include "modbusFunc.h"
#include "funcTCP.h"
#include <serial.h>
extern bool isConnection;

extern mailbox_t mb_conn;


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
			if (recv_err_or_buflen > 0 && isConnection){
				change_endian(query);
				if(is_modbus_query(query,recv_err_or_buflen)){
					modbus_query_handler(query, &modbus_answer);
					netconn_write(conn, &modbus_answer, modbus_answer.length+6, NETCONN_NOCOPY);
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

void up_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable in\r\n");
	isConnection = 1;

}
void down_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
	isConnection = 0;
}

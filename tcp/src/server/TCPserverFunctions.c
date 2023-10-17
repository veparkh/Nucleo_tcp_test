#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>
#include <stdio.h>
#include <serial.h>
#include "modbusFunc.h"
#include "funcTCP.h"
#include <stdbool.h>
#include <serial.h>

extern  bool isConnection;

extern mailbox_t mb_conn;

int write_log(struct netconn *conn, modbus_package *query){
	int recv_err_or_length;
	int tcp_code;
	while(isConnection){
		recv_err_or_length = read_data(conn, 1000, &query);
		if(recv_err_or_length==ERR_TIMEOUT){
			netconn_write(conn, "hi,i am log. I can't do something workful now, i just wanna say u got this\r\n",76, NETCONN_NOCOPY);
		}
		else if(recv_err_or_length > ERR_OK){
			int scanf= sscanf((char*)query,"%d",&tcp_code);
			dbgprintf("sscanf:%d\r\n", scanf);
				if(scanf==1){
					dbgprintf("tcp_code log:%d", tcp_code);
					if(tcp_code==100){
						return ERR_OK;
					}
				}
		}
		else{
			return recv_err_or_length;
		}
	}
	return ERR_RTE;
}

int tcp_query_handler(modbus_package *query, struct netconn *conn){
	int tcp_code;
	if(sscanf((char*)query,"%d",&tcp_code)==1){
		switch(tcp_code){
			case 100:{
				return write_log(conn,query);
				break;
			}
		}
	}
	return ERR_VAL;
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

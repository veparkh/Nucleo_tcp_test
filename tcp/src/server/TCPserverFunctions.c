#include "serial.h"
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>
#include <stdio.h>
#include "modbusFunc.h"
#include "funcTCP.h"
#include "TCPserverFunctions.h"
#include "TCPserverThreads.h"

extern  bool isConnectionEnabled;

int write_log(struct netconn *conn, modbus_package *query){
	int recv_err_or_length;
	int tcp_code;
	while(isConnectionEnabled){
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
	isConnectionEnabled = 1;

}
void down_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
	isConnectionEnabled = 0;
}

void tcp_init_server(void){

	THD_WORKING_AREA(tcp_conn_handler1, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
	THD_WORKING_AREA(tcp_conn_handler2, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
	THD_WORKING_AREA(wa_tcp_server, 1024); 	   // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")

	lwipthread_opts_t opts_s;
	struct ip4_addr ip_s, gateway_s, netmask_s;
	uint8_t mac_address_s[6] = {0xC2, 0xAF, 0x51, 0x33, 0xCC, 0x02};

	IP4_ADDR(&ip_s, 192, 168, 1, 123);
	IP4_ADDR(&gateway_s, 192, 168, 1, 1);
	IP4_ADDR(&netmask_s, 255, 255, 255, 0);
	tcpInit(&opts_s,ip_s, gateway_s,netmask_s,mac_address_s, down_callback_s,up_callback_s);
	chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, &ip_s);
	chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler,NULL);
	chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler,NULL);
}

#include "serial.h"
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "funcTCP.h"
#include "TCPserverFunctions.h"
#include "TCPServerThreads.h"

extern  bool isConnectionEnabled;



int write_log(struct netconn *conn){
	char query[600];
	int recv_err_or_length;
	int tcp_code;
	while(isConnectionEnabled){
		recv_err_or_length = read_data(conn, 1000, (char*)&query);
		if(recv_err_or_length==ERR_TIMEOUT){
			netconn_write(conn, "hi,i am log. I can't do something workful now, i just wanna say u got this\r\n",76, NETCONN_NOCOPY);
		}
		else if(recv_err_or_length > ERR_OK){
			int scanf= sscanf(query,"%d",&tcp_code);
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

int tcp_query_handler(char *query, struct netconn *conn){
	int tcp_code;
	if(sscanf(query,"%d",&tcp_code)==1){
		switch(tcp_code){
			case 100:{
				return write_log(conn);
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

THD_WORKING_AREA(tcp_conn_handler1, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
THD_WORKING_AREA(tcp_conn_handler2, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
THD_WORKING_AREA(wa_tcp_server, 2048); 	   // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")

void tcp_init_server(void){

	lwipthread_opts_t opts_s;
	struct ip4_addr ip_s, gateway_s, netmask_s;
	uint8_t mac_address_s[6] = {0xC2, 0xAF, 0x51, 0x33, 0xCC, 0x02};
	IP4_ADDR(&ip_s, 192, 168, 1, 123);
	IP4_ADDR(&gateway_s, 192, 168, 1, 1);
	IP4_ADDR(&netmask_s, 255, 255, 255, 0);

	opts_s.address = ip_s.addr;
	opts_s.gateway = gateway_s.addr;
	opts_s.netmask = netmask_s.addr;
	opts_s.macaddress = mac_address_s;
	opts_s.link_up_cb = up_callback_s;
	opts_s.link_down_cb = down_callback_s;
	lwipInit(&opts_s);
	chThdSleepSeconds(5);
	chThdCreateStatic(wa_tcp_server, 2048, NORMALPRIO, tcp_server, (void*)&ip_s);
	chThdCreateStatic(tcp_conn_handler1, 2048, NORMALPRIO, conn_handler, NULL);
	chThdCreateStatic(tcp_conn_handler2, 2048, NORMALPRIO, conn_handler, NULL);
}

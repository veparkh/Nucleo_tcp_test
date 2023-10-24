#include "serial.h"
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"
#include "stdint.h"
#include "modbusFunc.h"
#include "TCPclientFunctions.h"
#include "funcTCP.h"
extern  bool isConnectionEnabled;

void up_callback_c(void *p)
{
	(void)p;
	dbgprintf("cable in\r\n");
	isConnectionEnabled = true;
}
void down_callback_c(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
	isConnectionEnabled = false;
}

void modb_message(void){
	err_t err_connect;
		struct ip4_addr server_ip;
		IP4_ADDR(&server_ip, 192, 168, 1, 123);
		struct netconn *conn = netconn_new(NETCONN_TCP);
		if(conn==NULL)
		{
			chThdExit(ERR_MEM);
		}
		err_connect = netconn_connect(conn,&server_ip,80);
		dbgprintf("connect:%d\r\n",err_connect);
		if (err_connect != ERR_OK)
		{
		  netconn_close(conn);
		  netconn_delete(conn);
		  chThdExit(err_connect);
		}
		modbus_package query;
		form_MBAP(0, 0, 1, 0x02,&query);
		dbgprintf("%d %d",query.uid,query.func);
		resp_0x01_0x02 *resp = request_0x01_0x02(&query, 3, 5, conn);
		chThdSleepMilliseconds(100);
	    netconn_close(conn);
	    netconn_delete(conn);
	    chThdExit(ERR_OK);
}


void tcp_init_client(void){
	lwipthread_opts_t opts_c;
	struct ip4_addr ip_c, gateway_c, netmask_c;
	uint8_t mac_address_c[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};
	IP4_ADDR(&ip_c, 192, 168, 1, 124);
	IP4_ADDR (&gateway_c, 192, 168, 1, 1);
	IP4_ADDR (&netmask_c, 255, 255, 255, 0);
	tcpInit(&opts_c,ip_c, gateway_c,netmask_c,mac_address_c, down_callback_c,up_callback_c);
}

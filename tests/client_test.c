
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"
#include "stdint.h"
#include "modbusFunc.h"
#include "FuncTCP.h"
#include "TCPclientFunctions.h"
extern bool isConnectionEnabled;


void client_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    tcp_init_client();
    err_t err_connect;
    struct ip4_addr server_ip;
    IP4_ADDR(&server_ip, 192, 168, 1, 123);
    while (true) {
    	if (isConnectionEnabled){
			struct netconn *conn = netconn_new(NETCONN_TCP);
			if(conn==NULL)
				continue;
			err_connect = netconn_connect(conn,&server_ip,80);
			dbgprintf("connect:%d\r\n",err_connect);
			if (err_connect != ERR_OK)
			{
			  netconn_close(conn);
			  netconn_delete(conn);
			  continue;
			}
			modbus_package query;
			form_MBAP(0, 0, 1, 0x03,&query);
			dbgprintf("%d %d",query.uid,query.func);
			resp_0x03_0x04 *resp = request_0x03_0x04(&query, 0, 1, conn);
			dbgprintf("count:%d, data:%d",resp->count,resp->bytes[0]);
			chThdSleepMilliseconds(100);
			netconn_close(conn);
			netconn_delete(conn);
    	}
    	else{
    		dbgprintf("connectionIsNotEnabled:\r\n");
    	}
        chThdSleepMilliseconds(4000);
    }
}

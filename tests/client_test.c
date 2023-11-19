
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
	modbus_package query={0};
	modbus_package answer={0};
    struct ip4_addr server_ip;
    IP4_ADDR(&server_ip, 192, 168, 1, 2);
    while (true) {
        chThdSleepSeconds(4);
    	if (isConnectionEnabled){
			struct netconn *conn = netconn_new(NETCONN_TCP);
			if(conn==NULL)
				continue;
			err_connect = netconn_connect(conn,&server_ip,80);
			dbgprintf("connect_err:%d\r\n",err_connect);
			if (err_connect != ERR_OK)
			{
			  netconn_close(conn);
			  netconn_delete(conn);
			  continue;
			}
			bool is_succeded = request_0x01_0x02(&query, 0, 2, conn, &answer, 0x01);
			if(!is_succeded){
				mb_error err =  getError();
				dbgprintf("is_read_err: %d is_write_err:%d is_mb_exception %d err_code: %d\r\n", err.is_read_err, err.is_write_err,err.is_mb_exception,err.err_code);
			}
			netconn_close(conn);
			netconn_delete(conn);
    	}
    	else{
    		dbgprintf("connectionIsNotEnabled:\r\n");
    	}
    }
}

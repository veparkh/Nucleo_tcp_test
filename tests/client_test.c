
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

    while (true) {
    	if (isConnectionEnabled){

    	}
    	else{
    		dbgprintf("connectionIsNotEnabled:\r\n");
    	}
        chThdSleepMilliseconds(4000);
    }
}

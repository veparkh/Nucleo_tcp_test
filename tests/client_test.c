
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"
#include "stdint.h"
#include "modbusFunc.h"
#include "TCPclientOpts.h"
#include "FuncTCP.h"
#include "TCPclientFunctions.h"
extern bool isConnectionEnabled = false;


void client_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    tcpInit(&opts_c, ip_c, gateway_c, netmask_�, mac_address_�, down_callback_�, up_callback_�);

    while (true) {
    	if (isConnectionEnabled){

    	}
    	else{
    		dbgprintf("connectionIsNotEnabled:\r\n");
    	}
        chThdSleepMilliseconds(4000);
    }
}

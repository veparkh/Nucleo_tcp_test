
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
    tcpInit(&opts_c, ip_c, gateway_c, netmask_ñ, mac_address_ñ, down_callback_ñ, up_callback_ñ);

    while (true) {
    	if (isConnectionEnabled){

    	}
    	else{
    		dbgprintf("connectionIsNotEnabled:\r\n");
    	}
        chThdSleepMilliseconds(4000);
    }
}

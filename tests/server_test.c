#include "common.h"
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdio.h>
#include "stdint.h"
#include "TCPserverFunctions.h"
#include "modbusFunc.h"


void server_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    fill_tables();
	tcp_init_server();
    while (true) {
        chThdSleepMilliseconds(1000);
        //dbgprintf("Serv\r\n");
        palToggleLine(LINE_LED2);
    }
}



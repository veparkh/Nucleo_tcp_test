
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdio.h>
#include "modbusFunc.h"
#include "funcTCP.h"
#include "common.h"
#include "TCPserverFunctions.h"
#include "TCPserverThreads.h"

void server_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    tcp_init_server();
    fill_tables();
    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}



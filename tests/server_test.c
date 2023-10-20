
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdio.h>
#include "modbusFunc.h"
#include "funcTCP.h"
#include "common.h"
#include "TCPserverFunctions.h"

extern bool isConnection = false;
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);


void server_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    tcpInit(&opts_s, ip_c, gateway_s, netmask_s, mac_address_s, down_callback_s, up_callback_s);

    for(int i =0;i<REGISTER_LEN;i++){
    	Analog_Output_Register[i]=i;
    	Discrete_Input_Register[i] = i % 2;
    	Analog_Input_Register[i]=i;
    	Discrete_Output_Register[i] = i % 2;
    }

    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, &ip_c);
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler,NULL);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler,NULL);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}



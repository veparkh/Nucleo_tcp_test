
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "modbusFunc.h"
#include "modbusGet.h"
#include "modbusRegister.h"
#include "funcTCP.h"
#include "common.h"
#include "threadsTCP.h"
#include <stdio.h>


bool isConnection = false;
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);

THD_WORKING_AREA(tcp_conn_handler1, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
THD_WORKING_AREA(tcp_conn_handler2, 1024); // @suppress("Symbol is not resolved") // @suppress("Type cannot be resolved")
THD_WORKING_AREA(tcp_conn_handler3, 1024);
THD_WORKING_AREA(tcp_conn_handler4, 1024);
THD_WORKING_AREA(wa_tcp_server, 1024);
void server_test(void){

    halInit();
    chSysInit();
    debug_stream_init();

    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 192, 168, 1, 123);
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x33, 0xCC, 0x02};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;
    opts.link_up_cb = up_callback_s;
    opts.link_down_cb =  down_callback_s;

    lwipInit(&opts);
    chThdSleepSeconds(3);

    for(int i =0;i<REGISTER_LEN;i++){
    	Analog_Output_Register[i]=i;
    	Discrete_Input_Register[i] = i % 2;
    	Analog_Input_Register[i]=i;
    	Discrete_Output_Register[i] = i % 2;
    }

    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, &ip);
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler,NULL);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler,NULL);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}




#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <serial.h>
#include "modbusFunc.h"
#include "funcTCP.h"

bool isConnectionEnabled = false;
int32_t read_data(struct netconn *conn,int timeout,char *data){
	char *buf;
    struct netbuf *inbuf = NULL;
    uint16_t buflen;
    err_t recv_error;
    netconn_set_recvtimeout(conn, timeout);
    recv_error = netconn_recv(conn, &inbuf);
    int32_t recv_error_or_buflen;
    dbgprintf("recv_err :%d\r\n", recv_error);
    if (recv_error==0){
    	err_t data_err = netbuf_data(inbuf, (void **)&buf, &buflen);
        dbgprintf("data_err :%d\r\n", data_err);
    	netbuf_copy(inbuf, data, buflen);
    	recv_error_or_buflen = buflen;
    	for(uint8_t i=0; i < 28; i++) {
    		dbgprintf("%x\t", *(((uint8_t*)buf)+i));
    	}
    	dbgprintf("\r\n");
    }
    else{
    	recv_error_or_buflen = recv_error;
    }
    netbuf_delete(inbuf);
    return recv_error_or_buflen;
}

err_t write_data(struct netconn *conn,char *data){
		err_t err = netconn_write(conn, data, strlen(data), NETCONN_NOCOPY);
		return err;
}

void tcpInit(lwipthread_opts_t *opts,struct ip4_addr ip, struct ip4_addr gateway,struct ip4_addr netmask,uint8_t *mac_address,void (*down_callback) (void*),void (*up_callback) (void*)){

    opts->address = ip.addr;
    opts->gateway = gateway.addr;
    opts->netmask = netmask.addr;
    opts->macaddress = mac_address;
    opts->link_up_cb = up_callback;
    opts->link_down_cb = down_callback;

    lwipInit(opts);
	palToggleLine(LINE_LED2);
    chThdSleepSeconds(5);
}

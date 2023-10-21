
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>
#include <stdio.h>
#include <serial.h>
#include "modbusFunc.h"
#include "funcTCP.h"

bool isConnectionEnabled = false;
int32_t read_data(struct netconn *conn,int timeout,modbus_package **query){

    struct netbuf *inbuf = NULL;
    uint16_t buflen;
    err_t recv_error;
    netconn_set_recvtimeout(conn,timeout);
    int32_t recv_error_or_buflen;
    recv_error = netconn_recv(conn, &inbuf);
    if (recv_error==0){
    	netbuf_data(inbuf, (void **)query, &buflen);
    	recv_error_or_buflen = buflen;
    }
    else{
    	recv_error_or_buflen = recv_error;
    }
    netbuf_delete(inbuf);
    return recv_error_or_buflen;
}

err_t write_data(struct netconn *conn,char *data){
    // При записи просто отправляем указатель на буфер и количество байт.
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
    chThdSleepSeconds(5);
}

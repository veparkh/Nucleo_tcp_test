
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"

char msg1[200];
char msg2[200];
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);

err_t write_data(struct netconn *conn){
    return netconn_write(conn, "hi client!", strlen("hi client!"), NETCONN_NOCOPY);
}

void read_data(struct netconn *conn,char *arr){

    struct netbuf *inbuf;
    uint8_t *buf;
    uint16_t buflen;

    netconn_set_recvtimeout(conn,4000);
    err_t recv_error = netconn_recv(conn, &inbuf);
    dbgprintf("recv_error = %d\n\r",recv_error);
    if (recv_error==0){
    	netbuf_data(inbuf, (void **)&buf, &buflen);
    	strncpy(arr, inbuf->p->payload ,buflen);
    	dbgprintf(arr);
    }
    netbuf_delete(inbuf);
}
THD_WORKING_AREA(tcp_conn_handler1, 1024);
THD_WORKING_AREA(tcp_conn_handler2, 1024);
THD_WORKING_AREA(tcp_conn_handler3, 1024);
THD_WORKING_AREA(tcp_conn_handler4, 1024);
THD_FUNCTION(conn_handler, arr) {
		msg_t cast_per;
		while(true)
		{
			chMBFetchTimeout (&mb_conn, &cast_per, TIME_INFINITE);
			struct netconn *conn = (struct netconn*) cast_per;
			palToggleLine(LINE_LED1);
			read_data(conn,arr);
			write_data(conn);
			netconn_close(conn);
			netconn_delete(conn);
		}
}

THD_WORKING_AREA(wa_tcp_server, 1024);
THD_FUNCTION(tcp_server, ip) {

  chRegSetThreadName("server");
  struct netconn *conn, *newconn;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, (struct ip4_addr *) ip, 80);
  netconn_listen(conn);
  while (true) {
    palToggleLine(LINE_LED3);
    err_t accept_err = netconn_accept(conn, &newconn);
    dbgprintf("accept_error = %d\n\r",accept_err);
    if (accept_err != ERR_OK)
    	continue;

    chMBPostTimeout (&mb_conn, (msg_t)newconn, TIME_INFINITE);
  }

}

void up_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable in\r\n");

}
void down_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
}

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
    opts.link_up_cb = NULL;
    opts.link_down_cb = NULL;

    lwipInit(&opts);
    chThdSleepSeconds(2);

    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, &ip);
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler, msg1);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler, msg2);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}




#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"

char msg[200];
thread_t tp1;
thread_t tp2;
thread_t tp3;
thread_t tp4;

mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);
err_t write_data(struct netconn *conn){
    // ��� ������ ������ ���������� ��������� �� ����� � ���������� ����. ��������� �������� - ���� ��� ������. �� � ���� ��� ��� ������ �� �������
    return netconn_write(conn, "hi there!", strlen("hi there!"), NETCONN_NOCOPY);
}

void read_data(struct netconn *conn,char *arr){
    // ��� ������ ������� �������
    struct netbuf *inbuf;
    uint8_t *buf;
    uint16_t buflen;

    // ���� ��� ��� �������. ������� �����������. � inbuf ����� ���������� ��� � ��� �������
    netconn_set_recvtimeout(conn,4000);
    err_t recv_error = netconn_recv(conn, &inbuf);
    dbgprintf("recv_error = %d\n\r",recv_error);
    if (recv_error==0){
    // ������ �� ���� �� ��������, ��� ������ lwip. ������ ����������� ��������� ���, ����� ��� ������ ��������� �� ������ ������. ������ ������ ���������� ����
    netbuf_data(inbuf, (void **)&buf, &buflen);
    // ������� �� ����� ���������� ������. � ����������� tcp ���������� � ��������� �����, ��� ��� �������� ������������ �� ���� ����
    //sdWrite(&SD3, buf, buflen);
    strncpy(arr, inbuf->p->payload ,buflen);
    dbgprintf(arr);
    }
    // ������� ������. ���� ����� �� ������ ��� ����� ������ ����������
    netbuf_delete(inbuf);
}
THD_WORKING_AREA(tcp_conn_handler1, 1024);
THD_WORKING_AREA(tcp_conn_handler2, 1024);
THD_WORKING_AREA(tcp_conn_handler3, 1024);
THD_WORKING_AREA(tcp_conn_handler4, 1024);
THD_FUNCTION(conn_handler, p) {
		(void)p;
		while(true){
			msg_t cast_per;

			chMBFetchTimeout (&mb_conn, &cast_per, TIME_INFINITE);
			struct netconn *conn = (struct netconn*) cast_per;
		   // ��������� ��� ���-�� �����������
			palToggleLine(LINE_LED1);
			// ��������� ���-������
			read_data(conn,msg);
			// ������� ���-������
			err_t write_err = write_data(conn);
			dbgprintf("write_error = %d\n\r",write_err);

			// �� ��������� ������ ��������� ���������� � ������� �����������
//				netconn_close(conn);
//				netconn_delete(conn);
			// ����� ����� ������ ����� �����������
		}
}

THD_WORKING_AREA(wa_tcp_server, 1024);
THD_FUNCTION(tcp_server, p) {
chRegSetThreadName("server");
// ����� ������ ��������
  (void)p;

// ��������� ����������. conn - ���������� ����������, ����� ������ ������� ����
// newconn - ������� ���������� � ��� ��� ����������
  struct netconn *conn, *newconn;

// ��������� ���������� � ������ TCP
  conn = netconn_new(NETCONN_TCP);

// ��������� ��������� ��������� ���� �����, ������� ��������� ��� ������. �� ������ ���� ���� �� ��� � � �����
  struct ip4_addr ip;
  IP4_ADDR(&ip, 192, 168, 1, 123);
// ������������ � 80 �����
  netconn_bind(conn, &ip, 80);

// � �������� ��� �������
  netconn_listen(conn);

  while (true) {
    palToggleLine(LINE_LED3);
    // ���� ��� ���-�� �����������. ������� �����������
    err_t accept_err = netconn_accept(conn, &newconn);
    dbgprintf("accept_error = %d\n\r",accept_err);
    // ���� ���-�� �� ��� - ������ �� ������
    if (accept_err != ERR_OK)
    {
      continue;
    }
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
	void chMBReset (mailbox_t *mbp_conn);

}


void server_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    dbgprintf("start\r\n");
// ������ ����� �����
    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 192, 168, 1, 123);
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
// ��������� ����� ����� ���� ��������� �����. ������� ����� � ���� �� ���� ����������
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;
    opts.link_up_cb = NULL;
    opts.link_down_cb = NULL;


// ��������� ������� �������. � ����� ������� �� ������� �������� ��������� � ��� ����� �����������, ���� ������ ����� ����� � ��� �� ����
// ����� �� ���������� ���������, ����������� NULL � �������� ���������. ����� ����� ������������ ��������� �� ��������� �� ����� lwipthread.h
    lwipInit(&opts);
    chThdSleepSeconds(2);

// �������� ����� �������
    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, NULL);
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler, NULL);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler, NULL);
   /* chThdCreateStatic(tcp_conn_handler3, 1024, NORMALPRIO, conn_handler, NULL);
    chThdCreateStatic(tcp_conn_handler4, 1024, NORMALPRIO, conn_handler, NULL);*/
// �������� ���������, ����� �������� ��� �� �������
    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}



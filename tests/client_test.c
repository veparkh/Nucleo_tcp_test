
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"

char msg_c[200];

thread_t *tp = NULL;
bool isConnectionEnabled = false;
err_t write_data_c(struct netconn *conn,char *data){
    // При записи просто отправляем указатель на буфер и количество байт. Последний аргумент - флаг для записи. Но я пока про них ничего не понимаю
	if(strlen(data)!=0)
	{
		err_t err = netconn_write(conn, data, strlen(data), NETCONN_NOCOPY);
		strcpy(msg_c, "");
		return err;
	}
	return netconn_write(conn, "hi server!", strlen("hi server!"), NETCONN_NOCOPY);
}
enum REQUEST_TYPE {
	READ,
	WRITE,
	WRITE_READ,
};

typedef struct  {
  int TYPE;
  char myLetter[30];
} Request;
err_t read_data_c(struct netconn *conn){

    struct netbuf *inbuf;
    uint8_t *buf;
    uint16_t buflen;

	netconn_set_recvtimeout(conn,6000);
    err_t err_recv = netconn_recv(conn, &inbuf);
   if (err_recv ==0)
   {
	   netbuf_data(inbuf, (void **)&buf, &buflen);
	   strncpy(msg_c, inbuf->p->payload ,buflen);
   }
    netbuf_delete(inbuf);
    return err_recv;
}



static THD_FUNCTION(tcp_client, req) {
  Request *request = req;
  chRegSetThreadName("client_thread");
  err_t err_connect;
  struct ip4_addr server_ip;
  IP4_ADDR(&server_ip, 192, 168, 1, 123);
    struct netconn *conn = netconn_new(NETCONN_TCP);
    if(conn==NULL)
    {
    	chThdExit(ERR_MEM);
    }
    err_connect = netconn_connect(conn,&server_ip,80);
    if (err_connect != ERR_OK)
    {
      netconn_close(conn);
      netconn_delete(conn);
      chThdExit(err_connect);
    }
    err_t error;
    switch(request->TYPE){
    case	READ:{
    	error = read_data_c(conn);
    	break;
    	}
    case WRITE:{
    	error = write_data_c(conn,request->myLetter);
    	break;
    	}
    case WRITE_READ:{
    	write_data_c(conn,request->myLetter);
    	error = read_data_c(conn);
    	break;
    	}

    default:{
    	error = -100;
    	}
    }
    netconn_close(conn);
    netconn_delete(conn);
    chThdExit(error);
}

msg_t getData(Request request){
	tp = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(1024),"tcp", NORMALPRIO, tcp_client, &request);
	return chThdWait(tp);
}

void up_callback(void *p)
{
	(void)p;
	dbgprintf("cable in\r\n");
	isConnectionEnabled = true;
}
void down_callback(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
	isConnectionEnabled=false;

}
void client_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 192, 168, 1, 124);
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;
    opts.link_up_cb = up_callback;
    opts.link_down_cb = down_callback;

    lwipInit(&opts);
    chThdSleepSeconds(5);

    while (true) {
    	if (isConnectionEnabled){
    		Request request = {WRITE_READ,"Hi from client\r\n"};
        	msg_t err = getData(request);
        	if(err==ERR_OK){
        		dbgprintf(msg_c);
        	}else{
        	dbgprintf("err:%d\r\n",err);
        	}
    	}
    	else{
    		dbgprintf("connectionIsNotEnabled:\r\n");
    	}
        chThdSleepMilliseconds(4000);
    }
}

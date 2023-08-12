
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"


#include "modbusGet.h"
#include "modbusRegister.h"
#include "modbusFunc.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
char msg1[200];
char msg2[200];
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);

uint16_t answer_len;
mailbox_t main_mb;
msg_t main_mb_buffer[100];
uint8_t modbus_out_buf[100];

void write_register( uint16_t address, int16_t value){
	 if(address<100)
	  {
		 MB_WRITE_DISCRET_REG(address,value);
	  }
	 else if(address>99){
		 MB_WRITE_REG_INT16(address-100,value);
	 }
}
int16_t modbustcp_go(uint8_t* data)

{
  int16_t tid, pid, address, count,len=0;
  uint8_t func, uid;
  tid=modbustcp_get_tid(data);
  pid=modbustcp_get_pid(data);
  uid=modbustcp_get_uid(data);
  func=modbustcp_get_func(data);
  address=modbustcp_get_address(data);
  count=modbustcp_get_count(data);
  dbgprintf("tid: %d"" pid:%d address:%d func:%d uid:%d count:%d \n\r",tid, pid, address,func,uid,count);

  if(uid==MY_ID)
  {
    switch(func)
    {
    case MB_FUN_READ_ANALOG_INPUT_REGISTER:
    {
      len=modbusTCP_Read_Analog_Input_Register(tid,pid,uid,func,count,address);
      return len;
    }
    break;
    case MB_FUN_READ_ANALOG_OUTPUT_REGISTER:
    {
      len=modbusTCP_Read_Analog_Output_Register(tid,pid,uid,func,count,address);
      return len;
    }
    break;
    case MB_FUN_WRITE_DISCRETE_REGISTER:
    {
      write_register(address, modbustcp_get_boll_value(data));
      len=modbusTCP_Write_Discrete_Register(tid,pid,uid,func,address,Discrete_Register[address]);
      return len;
    }
    break;
    case MB_FUN_READ_DISCRETE_OUTPUT_REGISTER:
    {
      len=modbusTCP_Read_Discrete_Output_Register(tid,pid,uid,func,count,address);
      return len;
    }
    break;
    case MB_FUN_READ_DISCRETE_INPUT_REGISTER:
    {
      len=modbusTCP_Read_Discrete_Input_Register(tid,pid,uid,func,count,address);
      return len;
    }
    break;
    case MB_FUN_WRITE_ANALOG_REGISTER:
    {
      write_register(address+100, modbustcp_get_value(data));
      len=modbusTCP_Write_Analog_Register(tid,pid,uid,func, address,Analog_Register[address]);
      return len;
    }
    break;
    case MB_FUN_WRITE_MULTIPLE_ANALOG_REGISTER:
       {
         for(uint8_t perem=0;perem<count;perem++)
         {
           write_register(address+100+perem, modbustcp_get_multiple_register(data,perem));
         }
         len=modbusTCP_Write_Multiple_Analog_Register(tid,pid,uid,func, address,count);
         return len;
       }
       break;
       }
    }
  return len;
}



err_t write_data(struct netconn *conn){
    return netconn_write(conn, modbus_out_buf, answer_len, NETCONN_NOCOPY);
}

uint8_t* read_data(struct netconn *conn,char *arr){

    struct netbuf *inbuf = NULL;
    uint8_t *buf;
    uint16_t buflen;

    netconn_set_recvtimeout(conn,4000);
    err_t recv_error = netconn_recv(conn, &inbuf);
    dbgprintf("recv_error = %d\n\r",recv_error);
    if (recv_error==0){
    	netbuf_data(inbuf, (void **)&buf, &buflen);
    	strncpy(arr, inbuf->p->payload ,buflen);
    	//dbgprintf(arr);
    }
    netbuf_delete(inbuf);
    return buf;
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
			uint8_t *data = read_data(conn,arr);
			answer_len = modbustcp_go(data);
			write_data(conn);
			netconn_close(conn);
			netconn_delete(conn);
		}
}

THD_WORKING_AREA(wa_tcp_server, 1024);
THD_FUNCTION(tcp_server, ip) {

  palToggleLine(LINE_LED1);
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
    chThdSleepSeconds(3);

    for(int i =0;i<REGISTER_LEN;i++){
    	Analog_Register[i]=i;
    	Discrete_Register[i]=REGISTER_LEN-i;
    }

    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, &ip);
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler, msg1);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler, msg2);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}



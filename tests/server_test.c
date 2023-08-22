
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

#include "common.h"

#include "modbusGet.h"
#include "modbusRegister.h"
#include "modbusFunc.h"

char msg1[200];
char msg2[200];
bool connection = false;
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
modbus_package *query = NULL;
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);

modbus_package modbus_answer;
void write_register( uint16_t address, int16_t value){
	 if(address<100)
	  {
		 MB_WRITE_DISCRET_REG(address,value);
	  }
	 else if(address>99){
		 MB_WRITE_REG_INT16(address-100,value);
	 }
}

int16_t modbus_query_handler(modbus_package* query)
{
  int16_t address, count,len=0;
  query->tid=query->tid>>8|query->tid<<8;
  address=modbustcp_get_address(query);
  count=modbustcp_get_count(query);
  dbgprintf("data->tid: %u"" pid:%d address:%d func:%d uid:%d count:%d \n\r",query->tid, query->pid, address,query->func,query->uid,count);
  if(query->uid==MY_ID)
  {
    switch(query->func)
    {
    case MB_FUN_READ_ANALOG_INPUT_REGISTER:
    {
      len=modbusTCP_Read_Analog_Input_Register(query,count,address);
      return len;
    }
    case MB_FUN_READ_ANALOG_OUTPUT_REGISTER:
    {
      len=modbusTCP_Read_Analog_Output_Register(query,count,address);
      return len;
    }
    case MB_FUN_WRITE_DISCRETE_REGISTER:
    {
      write_register(address, modbustcp_get_boll_value(query));
      len=modbusTCP_Write_Discrete_Register(query,address,Discrete_Register[address]);
      return len;
    }
    case MB_FUN_READ_DISCRETE_OUTPUT_REGISTER:
    {
      len=modbusTCP_Read_Discrete_Output_Register(query,count,address);
      return len;
    }
    case MB_FUN_READ_DISCRETE_INPUT_REGISTER:
    {
      len=modbusTCP_Read_Discrete_Input_Register(query,count,address);
      return len;
    }
    case MB_FUN_WRITE_ANALOG_REGISTER:
    {
      write_register(address+100, modbustcp_get_value(query));
      len=modbusTCP_Write_Analog_Register(query, address,Analog_Register[address]);
      return len;
    }
    case MB_FUN_WRITE_MULTIPLE_ANALOG_REGISTER:
       {
         for(uint8_t perem=0;perem<count;perem++)
         {
           write_register(address+100+perem, modbustcp_get_multiple_register(query,perem));
         }
         len=modbusTCP_Write_Multiple_Analog_Register(query, address,count);
         return len;
       }
    }
  }
  return -10;
}

err_t write_data(struct netconn *conn, int16_t answer_len){
	if (answer_len==-10)
		return netconn_write(conn, "hi,i am log. I can't do something workful now, i just wanna say u got this\r\n",strlen("hi,i am log. I can't do something workful now, i just wanna say u got this\r\n") , NETCONN_NOCOPY);
    return netconn_write(conn, &modbus_answer, answer_len, NETCONN_NOCOPY);
}

err_t read_data(struct netconn *conn,char *arr,int timeout){

    struct netbuf *inbuf = NULL;
    uint16_t buflen;
    netconn_set_recvtimeout(conn,timeout);
    err_t recv_error = netconn_recv(conn, &inbuf);
    dbgprintf("recv_error = %d\n\r",recv_error);
    if (recv_error==0){
    	netbuf_data(inbuf, (void **)&query, &buflen);
    	strncpy(arr, inbuf->p->payload ,buflen);
    }
    netbuf_delete(inbuf);
    return recv_error;
}

THD_WORKING_AREA(tcp_conn_handler1, 1024);
THD_WORKING_AREA(tcp_conn_handler2, 1024);
THD_WORKING_AREA(tcp_conn_handler3, 1024);
THD_WORKING_AREA(tcp_conn_handler4, 1024);
THD_FUNCTION(conn_handler, arr) {
	chRegSetThreadName("conn_handler");
	msg_t cast_letter;
	int16_t answer_len;
	struct netconn *conn;
	err_t recv_err;
	while(true)
	{
		chMBFetchTimeout (&mb_conn, &cast_letter, TIME_INFINITE);

		conn = (struct netconn*) cast_letter;
		int timeout = 300000;
		palToggleLine(LINE_LED1);
		while(true)
		{
			recv_err = read_data(conn,arr,timeout);
			/*if(!connection)
				break;*/
			if (recv_err == 0)
			{
				answer_len = modbus_query_handler(query);
				dbgprintf("answer-len = %d", answer_len);
				dbgprintf("tiimeout = %d", timeout);
				if (answer_len == -10 && timeout == 1000)
				{

					timeout = 300000;
					continue;
				}
				if (answer_len == -10 && timeout == 300000)
				{
					timeout = 1000;
					continue;
				}
				write_data(conn, answer_len);
			}
			else if(recv_err == -3 && timeout == 1000){
				write_data(conn, -10);
			}
			else
			{
				break;
			}

		}
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
    msg_t mb_status = chMBPostTimeout (&mb_conn, (msg_t)newconn, TIME_IMMEDIATE);

    dbgprintf("MB: %d\n\r", mb_status);
  }

}

void up_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable in\r\n");
	connection = true;

}
void down_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
	connection = false;
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
    opts.link_up_cb = NULL;//up_callback_s;
    opts.link_down_cb = NULL;//down_callback_s;

    lwipInit(&opts);
    chThdSleepSeconds(3);

    for(int i =0;i<REGISTER_LEN;i++){
    	Analog_Register[i]=i;
    	Discrete_Register[i] = i % 2;
    }

    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, &ip);
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler, msg1);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler, msg2);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}




#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

#include "common.h"

#include "modbusGet.h"
#include "modbusRegister.h"
#include "modbusFunc.h"
#include <stdio.h>
char msg1[200];
char msg2[200];
bool isConnection = false;
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
modbus_package *query = NULL;
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);
modbus_package modbus_answer;



int16_t modbus_query_handler(modbus_package* query)
{
  int16_t address,len = 0;
  uint16_t count;
  query->tid = query->tid>>8 | query->tid<<8;
  query->length = query->length>>8 | query->length<<8;
  address = modbustcp_get_address(query);
  count = modbustcp_get_count(query);
  dbgprintf("data->tid: %d"" pid:%d address:%d func:%d uid:%d count:%d length:%d\n\r",query->tid, query->pid, address,query->func,query->uid,count,query->length);
  if(query->uid == MY_ID)
  {
    switch(query->func)
    {
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
    case MB_FUN_READ_ANALOG_OUTPUT_REGISTER:
    {
      len=modbusTCP_Read_Analog_Output_Register(query,count,address);
      return len;
    }
    case MB_FUN_READ_ANALOG_INPUT_REGISTER:
    {
      len=modbusTCP_Read_Analog_Input_Register(query,count,address);
      return len;
    }

    case MB_FUN_WRITE_DISCRETE_OUTPUT_REGISTER://count в данном случае значение
    {
    	if(count){
    		MB_WRITE_DISCRETE_OUTPUT_REGISTER(address,1);
    	}
    	 else{
    		 MB_WRITE_DISCRETE_OUTPUT_REGISTER(address,0);
    	 }
      len=modbusTCP_Write_Discrete_Register(query,address,Discrete_Output_Register[address]);
      return len;
    }
    case MB_FUN_WRITE_ANALOG_OUTPUT_REGISTER:
    {
      MB_WRITE_ANALOG_OUTPUT_REGISTER(address,modbustcp_get_value(query));
      len=modbusTCP_Write_Analog_Register(query, address,Analog_Output_Register[address]);
      return len;
    }
    case MB_FUN_WRITE_MULTIPLE_DIGITAL_OUTPUT_REGISTER:{
    	 for(uint8_t i=0;i<count;i++)
    	 {
    		 uint8_t multiple = MB_TCP_MULTIPLE_REGISTER+i*2;
    		   uint8_t value = query->data[multiple];
    		   MB_WRITE_DISCRETE_OUTPUT_REGISTER(address+i,value);
    	 }
    	 len = modbusTCP_Write_Multiple_Discrete_Register(query, address,count);
    	 return len;
    }
    case MB_FUN_WRITE_MULTIPLE_ANALOG_OUTPUT_REGISTER:{
             for(uint8_t i=0;i<count;i++)
             {
               MB_WRITE_ANALOG_OUTPUT_REGISTER(address+i,modbustcp_get_multiple_analog_register(query,i));
             }
             len=modbusTCP_Write_Multiple_Analog_Register(query, address,count);
             return len;
    	}
    case MB_FUN_WRITE_READ_MULTIPLE_ANALOG_OUTPUT_REGISTER:
    	{
    		int16_t write_start_address = query->data[4]<<8|query->data[6];
    		int8_t write_count = query->data[6]<<8|query->data[7];
			dbgprintf("write_count:%d",write_count);
    		int16_t write_value;
    		for(int i = 0;i<write_count;i++){
    			write_value = query->data[9+2*i]<<8 | query->data[9+2*i+1];
    			MB_WRITE_ANALOG_OUTPUT_REGISTER(write_start_address+i,write_value);
    		}
    		int16_t read_start_address = query->data[0]<<8|query->data[1]; // @suppress("Type cannot be resolved")
    		int16_t read_count = query->data[2]<<8|query->data[3]; // @suppress("Type cannot be resolved")
    		len = modbusTCP_Read_Write_Multiple_Analog_Output_Register(query, read_count,read_start_address);
    		return len;
    	}
    }
  }
  query->tid = query->tid>>8 | query->tid<<8;
  query->length = query->length>>8 | query->length<<8;
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

int write_log(struct netconn *conn,char *arr){
	err_t recv_err;
	int tcp_code;
	while(isConnection){
		recv_err= read_data(conn, arr, 1000);
		if(recv_err==ERR_TIMEOUT){
			write_data(conn, -10);
		}else if(recv_err == ERR_OK){
			if(sscanf((char*)query,"%d",&tcp_code)==1){
				dbgprintf("tcp_code log:%d", tcp_code);
				if(tcp_code==100){
				return ERR_OK;
				}
			}
		}else
		{
			return recv_err;
		}
	}
	return ERR_CLSD;
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
		palToggleLine(LINE_LED1);
		while(true)
		{
			for(int i =0 ;i<300;i++){
				recv_err = read_data(conn,arr,1000);
				dbgprintf("connection = %d", isConnection);
				if((!isConnection)||(recv_err==0)||(recv_err!=ERR_TIMEOUT))
					break;

			}
			if (recv_err == 0 && isConnection){
				answer_len = modbus_query_handler(query);
				dbgprintf("answer-len = %d", answer_len);
				if (answer_len == -10){
					int tcp_code;
					if(sscanf((char*)query,"%d",&tcp_code)==1){
						if(tcp_code==100){
							if(write_log(conn, arr)!=ERR_OK)
								break;

						}
					}
					dbgprintf("tcp_code:%d",tcp_code);
				}
				else{
					write_data(conn, answer_len);
				}
			}
			else{
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
	isConnection = 1;

}
void down_callback_s(void *p)
{
	(void)p;
	dbgprintf("cable out\r\n");
	isConnection = 0;
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
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler, msg1);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler, msg2);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}



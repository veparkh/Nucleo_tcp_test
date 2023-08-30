
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

#include "common.h"

#include "modbusGet.h"
#include "modbusRegister.h"
#include "modbusFunc.h"
#include <stdio.h>

bool isConnection = false;
mailbox_t mb_conn;
msg_t mb_conn_buffer[5];
MAILBOX_DECL(mb_conn, mb_conn_buffer, 20);

int tcp_query_handler(modbus_package *query, struct netconn *conn){
	int tcp_code;
	if(sscanf((char*)query,"%d",&tcp_code)==1){
		switch(tcp_code){
			case 100:{
				return write_log(conn);
				break;
			}
		}
	}
	return ERR_VAL;
}

int16_t modbus_query_handler(modbus_package* query,modbus_package *modbus_answer)
{
	int16_t address,len = -10;
	uint16_t count;
	modbus_answer->tid = query->tid;
	modbus_answer->pid = query->pid;
	modbus_answer->uid = query->uid;
	modbus_answer->func = query->func;
	address = modbustcp_get_address(query);
	count = modbustcp_get_count(query);
	dbgprintf("data->tid: %d"" pid:%d address:%d func:%d uid:%d count:%d length:%d\n\r",query->tid, query->pid, address,query->func,query->uid,count,query->length);
	switch(query->func)
	{
		case MB_FUN_READ_DISCRETE_OUTPUT_REGISTER:
		{
			len = modbustcp_send_answer_fun_0x01or0x02(query,count, address, modbus_answer);
			break;
		}
		case MB_FUN_READ_DISCRETE_INPUT_REGISTER:
		 {
			 len = modbustcp_send_answer_fun_0x01or0x02(query,count, address,modbus_answer);
			 break;
		}
		case MB_FUN_READ_ANALOG_OUTPUT_REGISTER:
		{
			 len = modbustcp_send_answer_fun_0x03or0x04(query,count, address, modbus_answer);
			 break;
		}
		case MB_FUN_READ_ANALOG_INPUT_REGISTER:
		{
			len = modbustcp_send_answer_fun_0x03or0x04(query, count, address,modbus_answer);
			break;
		}

		case MB_FUN_WRITE_DISCRETE_OUTPUT_REGISTER://count в данном случае значение
		{
			if(count)
			{
				MB_WRITE_DISCRETE_OUTPUT_REGISTER(address,1);
			}
			 else
			 {
				 MB_WRITE_DISCRETE_OUTPUT_REGISTER(address,0);
			 }
			len = modbustcp_send_answer_fun_0x05(query, address, Discrete_Output_Register[address],modbus_answer);
			break;
		}
		case MB_FUN_WRITE_ANALOG_OUTPUT_REGISTER:
		{
			MB_WRITE_ANALOG_OUTPUT_REGISTER(address,modbustcp_get_value(query));
			len = modbustcp_send_answer_fun_0x06( query, address, Analog_Output_Register[address],modbus_answer);
			break;
		}
		case MB_FUN_WRITE_MULTIPLE_DIGITAL_OUTPUT_REGISTER:
		{
			for(uint8_t i=0;i<count;i++)
			{
				uint8_t multiple = MB_TCP_MULTIPLE_REGISTER+i*2;
				uint8_t value = query->data[multiple];
				MB_WRITE_DISCRETE_OUTPUT_REGISTER(address+i,value);
			}
			len = modbustcp_send_answer_fun_0x10or0x0F(query,  address, count,modbus_answer);
			break;
		}
		case MB_FUN_WRITE_MULTIPLE_ANALOG_OUTPUT_REGISTER:
		{

			for(uint8_t i=0;i<count;i++)
			{
				MB_WRITE_ANALOG_OUTPUT_REGISTER(address+i,modbustcp_get_multiple_analog_register(query,i));
			}
			palToggleLine(LINE_LED1);
			len = modbustcp_send_answer_fun_0x10or0x0F(query,  address, count,modbus_answer);
			break;

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
			int16_t read_start_address = query->data[0]<<8|query->data[1];
			int16_t read_count = query->data[2]<<8|query->data[3];
			len = modbustcp_send_answer_fun_0x17(query, read_count,read_start_address,modbus_answer);
			break;
		}
		default:
		{
			modbus_answer->func = 128+query->func;
			modbus_answer->data[0] = 0x01;
			modbus_answer->length=3;
			len = 7+2;
		}
	}
	query = NULL;
	change_endian(modbus_answer);
	return len;
}

int32_t read_data(struct netconn *conn,int timeout,modbus_package **query){

    struct netbuf *inbuf = NULL;
    netconn_set_recvtimeout(conn,timeout);
    int32_t recv_error_or_buflen = netconn_recv(conn, &inbuf);
    if (recv_error_or_buflen==0){
    	netbuf_data(inbuf, (void **)query, &recv_error_or_buflen);
    }
    netbuf_delete(inbuf);
    chThdSleepMilliseconds(40);
    return recv_error_or_buflen;
}

int write_log(struct netconn *conn, modbus_package *query){
	int recv_err_or_length;
	int tcp_code;
	while(isConnection){
		recv_err_or_length = read_data(conn, 1000, &query);
		if(recv_err_or_length==ERR_TIMEOUT){
			netconn_write(conn, "hi,i am log. I can't do something workful now, i just wanna say u got this\r\n",76, NETCONN_NOCOPY);
		}
		else if(recv_err_or_length > ERR_OK){
				if(sscanf((char*)query,"%d",&tcp_code)==1){
					dbgprintf("tcp_code log:%d", tcp_code);
					if(tcp_code==100){
						return ERR_OK;
					}
				}
		}else
		{
			return recv_err_or_length;
		}
	}
	return ERR_RTE;
}

THD_WORKING_AREA(tcp_conn_handler1, 1024);
THD_WORKING_AREA(tcp_conn_handler2, 1024);
THD_WORKING_AREA(tcp_conn_handler3, 1024);
THD_WORKING_AREA(tcp_conn_handler4, 1024);
THD_FUNCTION(conn_handler, p){
	(void) p;
	chRegSetThreadName("conn_handler");
	msg_t cast_letter;
	modbus_package modbus_answer;
	modbus_package *query = NULL;
	int16_t answer_len;
	struct netconn *conn;
	int recv_err_or_buflen;
	while(true)
	{
		chMBFetchTimeout (&mb_conn, &cast_letter, TIME_INFINITE);
		conn = (struct netconn*) cast_letter;
		while(true)
		{
			for(int i =0 ;i<300;i++){
				chThdSleepMilliseconds(100);
				recv_err_or_buflen = read_data(conn, 1000, &query);
				if((!isConnection)||(recv_err_or_buflen>0)||(recv_err_or_buflen!=ERR_TIMEOUT))
					break;
			}
			if (recv_err_or_buflen > 0 && isConnection){
				change_endian(query);
				if(is_modbus_query(query,recv_err_or_buflen)){
					answer_len = modbus_query_handler(query, &modbus_answer);
					netconn_write(conn, &modbus_answer, answer_len, NETCONN_NOCOPY);
				}
				else{
					change_endian(query);
					if(tcp_query_handler(query, conn)!=ERR_OK)
						break;
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
    err_t accept_err = netconn_accept(conn, &newconn);
    dbgprintf("accept_error = %d\n\r",accept_err);
    if (accept_err != ERR_OK)
    	continue;
    chMBPostTimeout (&mb_conn, (msg_t)newconn, TIME_IMMEDIATE);
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
    chThdCreateStatic(tcp_conn_handler1, 1024, NORMALPRIO, conn_handler,NULL);
    chThdCreateStatic(tcp_conn_handler2, 1024, NORMALPRIO, conn_handler,NULL);

    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}



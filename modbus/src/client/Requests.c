#include <modbus/include/client/Requests.h>
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "dataStructures.h"
#include "funcTCP.h"
#include "modbusFunc.h"
#include "serial.h"
#include "string.h"

uint16_t tid = 0;
uint16_t get_tid(void){
	if(tid==65535)
		tid=0;
	else
		tid++;
	return tid;
}


int32_t read_modbus(struct netconn *conn,int timeout, modbus_package *answer){
	int32_t recv_err_or_buflen = read_data(conn, timeout, (char*)answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	for(uint8_t i; i < recv_err_or_buflen; i++) {
		dbgprintf("%x\t", *(((uint8_t *)answer)+i));
	}
	dbgprintf("\r\n");
	dbgprintf("buflen:%d\r\n",recv_err_or_buflen);
	if(!is_modbus_package(answer, recv_err_or_buflen)){
		dbgprintf("not modbus\r\n");
		return -100;
	}
	return recv_err_or_buflen;
}



bool write_read_modbus(uint16_t length, struct netconn *conn,modbus_package *query,modbus_package *answer){
	err_t recv_err = netconn_write(conn, query, length, NETCONN_NOCOPY);
		if(recv_err!=0){
			setError(0, 1, 0, recv_err);
			return 0;
		}
		int32_t recv_err_or_buflen = read_modbus(conn, 3000, answer);
		if(recv_err_or_buflen<0){
			setError(1, 0, 0, recv_err_or_buflen);
			return 0;
		}
		resp_change_endian(answer);
		dbgprintf("query_func:%d answer->func:%d data[0]:%d\r\n",query->func,answer->func,answer->data[0]);
		if(is_exception(answer, query->func)){
			handle_exception(answer);
			setError(0, 0, 1, answer->data[0]);
			return 0;
		}
		return 1;
}
bool is_exception(modbus_package *answer,uint8_t func){
	if(answer->func==func+0x80){
		handle_exception(answer);
		return true;
	}
	else{
		return false;
	}
}
void handle_exception(modbus_package *answer){
	switch(answer->data[0]){
		case 0x01:{
			dbgprintf("0x01 exception in func\r\n");
			break;
		}
		case 0x02:{
			dbgprintf("0x02 exception in func\r\n");
			break;
		}
		case 0x03:{
			dbgprintf("0x03 exception in func\r\n");
			break;
		}
		case 0x04:{
			dbgprintf("0x04 exception in func\r\n");
			break;
		}
	}
}

bool request_0x01_0x02(modbus_package *query, uint16_t address, uint16_t count, struct netconn *conn, modbus_package *answer, uint8_t func){
	form_MBAP(get_tid(), 0, MY_ID, func, query);
	uint16_t length = fill_req_0x01_0x02((req_0x01_0x02*)query->data, address, count);
	query->length = length+2;
	req_change_endian(query);
	return write_read_modbus(length+2+6, conn, query, answer);
}
bool request_0x03_0x04(modbus_package *query,uint16_t address,uint16_t count,struct netconn *conn, modbus_package *answer, uint8_t func){
	form_MBAP(get_tid(), 0, MY_ID, func, query);
	uint16_t length = fill_req_0x03_0x04((req_0x03_0x04*) query->data, address, count);
	query->length = length+2;
	req_change_endian(query);
	return  write_read_modbus(length+2+6, conn, query, answer);
}

bool request_0x05_0x06(modbus_package *query,uint16_t address,uint16_t value,struct netconn *conn, modbus_package *answer, uint8_t func){
	form_MBAP(get_tid(), 0, MY_ID, func, query);
	uint16_t length = fill_req_0x05_0x06((req_0x05_0x06*) query->data, address, value,query->func);
	query->length = length+2;
	req_change_endian(query);
	return write_read_modbus(length+2+6, conn, query, answer);
}
bool request_0x0F(modbus_package *query,int16_t address,int16_t quantity,uint8_t byte_count,uint8_t *values, struct netconn *conn, modbus_package *answer){
	form_MBAP(get_tid(), 0, MY_ID, 0x0F, query);
	uint16_t length = fill_req_0x0F((req_0x0F*)query->data, address, quantity, byte_count,values);
	query->length = length+2;
	req_change_endian(query);
	return write_read_modbus(length+2+6, conn, query, answer);
}
bool request_0x10(modbus_package *query,int16_t address,int16_t quantity,uint8_t byte_count,uint16_t *values, struct netconn *conn, modbus_package *answer){
	form_MBAP(get_tid(), 0, MY_ID, 0x10, query);
	uint16_t length = fill_req_0x10((req_0x10*)query->data, address, quantity, byte_count, values);
	query->length = length+2;
	req_change_endian(query);
	return write_read_modbus(length+2+6, conn, query, answer);
}
bool request_0x16(modbus_package *query,uint16_t address,uint16_t and_mask,uint16_t or_mask, struct netconn *conn, modbus_package *answer){
	form_MBAP(get_tid(), 0, MY_ID, 0x16, query);
	uint16_t length = fill_req_0x16((req_0x16*)query->data, address, and_mask, or_mask);
	query->length = length+2;
	req_change_endian(query);
	return write_read_modbus(length+2+6, conn, query, answer);
}
bool request_0x17(modbus_package *query,uint16_t read_starting_address,uint16_t read_quantity,uint16_t write_starting_address,uint16_t write_quantity,uint8_t write_byte_count,uint16_t *values,struct netconn *conn, modbus_package *answer){
	form_MBAP(get_tid(), 0, MY_ID, 0x17, query);
	uint16_t length = fill_req_0x17((req_0x17*)query->data, read_starting_address, read_quantity,write_starting_address, write_quantity,write_byte_count, values);
	query->length = length+2;
	req_change_endian(query);
	return write_read_modbus(length+2+6, conn, query, answer);
}


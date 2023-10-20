#include <modbus/include/client/Requests.h>
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "dataStructures.h"
#include "funcTCP.h"
#include "modbusFunc.h"


int32_t read_modbus(struct netconn *conn,int timeout,modbus_package **answer){
	int32_t recv_err_or_buflen = read_data(conn, timeout, answer);
		if(recv_err_or_buflen<0)
			return recv_err_or_buflen;
		resp_change_endian(*answer);
		dbgprintf("buflen:%d\r\n",recv_err_or_buflen);
		if(!is_modbus_package(*answer, recv_err_or_buflen)){
			return -100;
		}
		return recv_err_or_buflen;
}

bool is_exception(modbus_package *answer,uint8_t func){
	if(answer->data[0]==func+0x80){
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
			dbgprintf("0x01 exception in func");
			break;
		}
		case 0x02:{
			dbgprintf("0x02 exception in func");
			break;
		}
		case 0x03:{
			dbgprintf("0x03 exception in func");
			break;
		}
		case 0x04:{
			dbgprintf("0x04 exception in func");
			break;
		}
	}
}

resp_0x01_0x02* request_0x01_0x02(modbus_package *query, uint16_t address, uint16_t count, struct netconn *conn){

	query->length = fill_req_0x01_0x02((req_0x01_0x02*)query->data, address, count);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0){
		return NULL;
	}
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x01_0x02*)answer->data;
}
resp_0x03_0x04* request_0x03_0x04(modbus_package *query,uint16_t address,uint16_t count,struct netconn *conn){

	fill_req_0x03_0x04((req_0x03_0x04*) query->data, address, count);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0){
		return NULL;
	}
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x03_0x04*)answer->data;
}
resp_0x05_0x06* request_0x05_0x06(modbus_package *query,uint16_t address,uint16_t count,struct netconn *conn){

	fill_req_0x05_0x06((req_0x05_0x06*) query->data, address, count);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0){
		return NULL;
	}
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x05_0x06*)answer->data;
}
resp_0x0F_0x10* request_0x0F(modbus_package *query,int16_t address,int16_t quantity,uint8_t byte_count,uint8_t *values, struct netconn *conn){

	fill_req_0x0F((req_0x0F*)query->data, address, quantity, byte_count,values);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0)
		return NULL;
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x0F_0x10*)answer->data;
}
resp_0x0F_0x10* request_0x10(modbus_package *query,int16_t address,int16_t quantity,uint8_t byte_count,uint16_t *values, struct netconn *conn){

	fill_req_0x10((req_0x10*)query->data, address, quantity, byte_count, values);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0)
		return NULL;
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x0F_0x10*)answer->data;
}
resp_0x16* request_0x16(modbus_package *query,uint16_t address,uint16_t and_mask,uint16_t or_mask, struct netconn *conn){

	fill_req_0x16((req_0x16*)query->data, address, and_mask, or_mask);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0)
		return NULL;
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x16*)answer->data;
}
resp_0x17* request_0x17(modbus_package *query,uint16_t read_starting_address,uint16_t read_quantity,uint16_t write_starting_address,uint16_t write_quantity,uint8_t write_byte_count,uint16_t *values,struct netconn *conn){

	fill_req_0x17((req_0x17*)query->data, read_starting_address, read_quantity,write_starting_address, write_quantity,write_byte_count, values);
	err_t recv_err = netconn_write(conn, query, 12, NETCONN_NOCOPY);
	if(recv_err!=0)
		return NULL;
	modbus_package *answer;
	int32_t recv_err_or_buflen = read_modbus(conn, 3000, &answer);
	if(recv_err_or_buflen<0)
		return NULL;
	if(is_exception(answer, answer->func))
		return NULL;
	return (resp_0x17*)answer->data;
}

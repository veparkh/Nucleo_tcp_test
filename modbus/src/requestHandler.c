/*
 * requestHandler.c
 *
 *  Created on: 11 сент. 2023 г.
 *      Author: vparx
 */
#include "modbusFunc.h"
#include "requestHandler.h"
#include "funcTCP.h"
#include "serial.h"

int32_t request_0x01or0x02_handler(modbus_package *query,modbus_package *modbus_answer,uint16_t start_address,uint16_t count,struct netconn *conn){
	// write_func start
	query->data[MB_TCP_ADDRESS]= start_address>>8;
	query->data[MB_TCP_ADDRESS+1]= start_address;
	query->data[MB_TCP_COUNT]= count>>8;
	query->data[MB_TCP_COUNT+1]= count;
	uint16_t len=6;
	query->length = len;
	change_endian(query);
	dbgprintf("query uid:%d", query->uid);
	err_t recv_err = netconn_write(conn, query, len+6, NETCONN_NOCOPY);
	dbgprintf("write:%d\r\n",recv_err);
	palToggleLine(LINE_LED3);
	if(recv_err!=0){
		return recv_err;
	}
	// write_func end

	// read_func start
	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);

	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	dbgprintf("buflen:%d\r\n",recv_err_or_buflen);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen)){
		return -100;
	}
	// read_func end

	// parse_exceptions start
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
	}
	// parse_exceptions end
		for(int i =1, j = 0;(i-1)*8+j<count;j++){
			dbgprintf("data[%d] = %d",8*(i-1)+j+start_address,modbus_answer->data[i]&(1<<j));
			if(modbus_answer->func == 0x01){
				write_coils(8*(i-1)+j+start_address, modbus_answer->data[i]&(1<<j));
			}
			if(j==7){
				i++;
				j=0;
			}
		}
	return recv_err_or_buflen;
}
int32_t request_0x03or0x04_handler(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer,uint16_t start_address,uint16_t count,struct netconn *conn){
	query->data[MB_TCP_ADDRESS]= start_address>>8;
	query->data[MB_TCP_ADDRESS+1]= start_address;
	query->data[MB_TCP_COUNT]= count>>8;
	query->data[MB_TCP_COUNT+1]= count;
	query->length = 6;

	err_t recv_err = netconn_write(conn, &query, query->length+6, NETCONN_NOCOPY);
	if(recv_err!=0){
		return recv_err;
	}
	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen)){
		return-100;
	}
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
	}
	if(modbus_answer->func == 0x03){
		for(int i =0;i<modbus_answer->data[0]/2;i++){
			int16_t value = modbus_answer->data[2*i+1]<<8|modbus_answer->data[2*i+2];
			dbgprintf("data[%d] = %d",i,value);
			write_holding_registers(i, value);
			}
	}
	else{
		for(int i =0; i<modbus_answer->data[0]/2;i++){
			int16_t value = modbus_answer->data[2*i+1]<<8|modbus_answer->data[2*i+2];
			dbgprintf("data[%d] = %d",i,value);//Записать inputs нельзя
			}
	}
	return recv_err_or_buflen;

}
int32_t request_0x05_handler(modbus_package *query, int16_t address,uint8_t value,modbus_package *modbus_answer,struct netconn *conn){
	query->data[MB_TCP_ADDRESS]= address>>8;
	query->data[MB_TCP_ADDRESS+1]= address;
	query->data[MB_TCP_COUNT]= value>>8;
	query->data[MB_TCP_COUNT+1]= value;
	query->length = 6;
	err_t recv_err = netconn_write(conn, &query, query->length+6, NETCONN_NOCOPY);
	if(recv_err!=0){
		return recv_err;
	}
	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen)){
		return -100;
	}
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
		}
	//Нужно ли сравнивать значения для выяснения результата запроса
	dbgprintf("func :0x05 address:%d val:%d",modbus_answer->data[0]<<8|modbus_answer->data[1],modbus_answer->data[2]);
	return recv_err_or_buflen;


}
int32_t request_0x06_handler(modbus_package *query, int16_t address, int16_t value, modbus_package *modbus_answer,struct netconn *conn){
	query->data[MB_TCP_ADDRESS]= address>>8;
	query->data[MB_TCP_ADDRESS+1]= address;
	query->data[MB_TCP_COUNT]= value>>8;
	query->data[MB_TCP_COUNT+1]= value;
	query->length = 6;

	err_t recv_err = netconn_write(conn, &query, query->length+6, NETCONN_NOCOPY);
	if(recv_err!=0){
		return recv_err;
	}
	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen))
		return -100;
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
	}
		//Нужно ли сравнивать значения для выяснения результата операции
	return recv_err_or_buflen;

}

int32_t request_0x10or0x0F_handler(modbus_package *query, int16_t address,int16_t count,uint8_t byte_count,uint8_t *values,modbus_package *modbus_answer,struct netconn *conn){
	query->data[MB_TCP_ADDRESS]= address>>8;
	query->data[MB_TCP_ADDRESS+1]= address;
	query->data[MB_TCP_COUNT]= count>>8;
	query->data[MB_TCP_COUNT+1]= count;
	query->data[MB_TCP_COUNT_BYTE] = byte_count;
	for(uint16_t i=0; i < count;i++){
		query->data[MB_TCP_MULTIPLE_REGISTER+i]=values[i];
		query->data[MB_TCP_MULTIPLE_REGISTER+i+1]=values[i+1];
	}
	query->length = 6+count;

	err_t recv_err = netconn_write(conn, &query, query->length+6, NETCONN_NOCOPY);
	if(recv_err!=0){
		return recv_err;
	}
	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen)){
		return -100;
	}
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
	}
	if(query->func==0x10){
	dbgprintf("0x10 func\n\r");
	}
	else{
		dbgprintf("0x0F func\n\r");
	}
	return recv_err_or_buflen;


}
int32_t request_0x16_handler (modbus_package *query, modbus_package *modbus_answer,uint16_t address,uint16_t and_mask,uint16_t or_mask,struct netconn *conn){
	query->data[MB_TCP_ADDRESS] = address>>8;
	query->data[MB_TCP_ADDRESS+1] = address;
	query->data[MB_TCP_COUNT] = and_mask>>8;
	query->data[MB_TCP_COUNT+1] = and_mask;
	query->data[MB_TCP_COUNT_BYTE] = or_mask>>8;
	query->data[MB_TCP_COUNT_BYTE+1] = or_mask;
	query->length=8;

	err_t recv_err = netconn_write(conn, &query, query->length+6, NETCONN_NOCOPY);
	if(recv_err!=0){
		return recv_err;
	}

	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen)){
		return -100;
	}
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
	}
	dbgprintf("0x16 func\n\r");
	return recv_err_or_buflen;


}
int32_t request_0x17_handler(modbus_package *query,modbus_package *modbus_answer, uint16_t read_starting_address,uint16_t read_count,uint16_t write_starting_address,uint16_t write_count,uint8_t write_byte_count,uint16_t *values,struct netconn *conn){
	query->data[MB_TCP_ADDRESS] = read_starting_address>>8;
	query->data[MB_TCP_ADDRESS+1] = read_starting_address;

	query->data[MB_TCP_COUNT] = read_count>>8;
	query->data[MB_TCP_COUNT+1] = read_count;

	query->data[MB_TCP_COUNT_BYTE] = write_starting_address>>8;
	query->data[MB_TCP_COUNT_BYTE+1] = write_starting_address;

	query->data[6] = write_count>>8;
	query->data[7] = write_count;

	query->data[8] = write_byte_count>>8;
	query->data[9] = write_byte_count;

	for(int i =0;i<write_count;i++){
		query->data[10+2*i] = values[i];
		query->data[10+2*i+1] = values[i]>>8;
	}
	query->length=11+write_byte_count;

	err_t recv_err = netconn_write(conn, &query, query->length+6, NETCONN_NOCOPY);
	if(recv_err!=0){
		return recv_err;
	}

	int32_t recv_err_or_buflen = read_data(conn, 3000, &modbus_answer);
	if(recv_err_or_buflen<0)
		return recv_err_or_buflen;
	change_endian(modbus_answer);
	if(!is_modbus_package(modbus_answer, recv_err_or_buflen)){
		return -100;
	}
	if(is_exception(modbus_answer, query->func)){
		handle_exception(modbus_answer);
		return -100;
	}
	dbgprintf("0x17 func\n\r");
	return recv_err_or_buflen;

}




/*
 * requestHandler.c
 *
 *  Created on: 11 сент. 2023 г.
 *      Author: vparx
 */
#include "modbusFunc.h"
void request_0x01or0x02_handler(modbus_package *query,modbus_package *modbus_answer,uint16_t start_address,uint16_t count){
	query->data[MB_TCP_ADDRESS]= start_address>>8;
	query->data[MB_TCP_ADDRESS+1]= start_address;
	query->data[MB_TCP_COUNT]= count>>8;
	query->data[MB_TCP_ADDRESS+1]= count;
	query->length = 6;
}
void request_0x03or0x04_handler(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer,uint16_t start_address,uint16_t count){
	query->data[MB_TCP_ADDRESS]= start_address>>8;
	query->data[MB_TCP_ADDRESS+1]= start_address;
	query->data[MB_TCP_COUNT]= count>>8;
	query->data[MB_TCP_COUNT+1]= count;
	query->length = 6;
}
void request_0x05_handler(modbus_package *query, int16_t address,uint16_t value,modbus_package *modbus_answer){
	query->data[MB_TCP_ADDRESS]= address>>8;
	query->data[MB_TCP_ADDRESS+1]= address;
	query->data[MB_TCP_COUNT]= value>>8;
	query->data[MB_TCP_COUNT+1]= value;
	query->length = 6;
}
void request_0x06_handler(modbus_package *query, int16_t address, int16_t value, modbus_package *modbus_answer){
	query->data[MB_TCP_ADDRESS]= address>>8;
	query->data[MB_TCP_ADDRESS+1]= address;
	query->data[MB_TCP_COUNT]= value>>8;
	query->data[MB_TCP_COUNT+1]= value;
	query->length = 6;
}
void request_0x10or0x0F_handler(modbus_package *query, int16_t address,int16_t count,uint8_t byte_count,uint8_t *values,modbus_package *modbus_answer){
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
}
void request_0x16_handler (modbus_package *query, modbus_package *modbus_answer,uint16_t address,uint16_t and_mask,uint16_t or_mask){
	query->data[MB_TCP_ADDRESS] = address>>8;
	query->data[MB_TCP_ADDRESS+1] = address;
	query->data[MB_TCP_COUNT] = and_mask>>8;
	query->data[MB_TCP_COUNT+1] = and_mask;
	query->data[MB_TCP_COUNT_BYTE] = or_mask>>8;
	query->data[MB_TCP_COUNT_BYTE+1] = or_mask;
	query->length=8;
}
void request_0x17_handler(modbus_package *query,modbus_package *modbus_answer, uint16_t read_starting_address,uint16_t read_count,uint16_t write_starting_address,uint16_t write_count,uint8_t write_byte_count,uint16_t *values){
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

	for(uint16_t i =0;i<write_count;i++){
		query->data[10+2i] = values[i];
		query->data[10+2i+1] = values[i]>>8;
	}
	query->length=11+write_byte_count;
}

void form_MBAP(uint16_t tid,uint16_t pid,uint8_t uid,uint8_t func,modbus_package *query){
	query->func = func;
	query->pid = pid;
	query->tid = tid;
	query->uid = uid;
}

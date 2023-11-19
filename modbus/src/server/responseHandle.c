
#include <hal.h>
#include <ch.h>
#include "modbusFunc.h"
#include "dataStructures.h"
#include "serial.h"
/* ----------------------- MBAP Header --------------------------------------*/
/*
 *
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |
 * (2)   (3)   (4)      (5)    (6)
 *
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte)
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 */

void func_0x01or0x02_handler (modbus_package *query, modbus_package *modbus_answer){

	resp_0x01_0x02 *resp = (resp_0x01_0x02*)modbus_answer->data;
	req_0x01_0x02 *req =(req_0x01_0x02*)query->data;

	dbgprintf("counter:%d, address:%d\r\n",req->count,req->address);
	if(req->count<0x01 || req->count>0x07D0){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if((req->address+req->count)>REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	bool (*table) (int16_t address, uint8_t *val);
	if(query->func==0x01)
	  table = read_coils;
	else
	  table = read_discrete_input;
	uint16_t length_or_err = fill_resp_0x01_0x02(resp, req->address, req->count, table);
	if(!length_or_err){
		get_exception(query->func, 0x04 , modbus_answer);
		return;
	}
	modbus_answer->length=length_or_err;
	dbgprintf("real_Length^%u\r\n",length_or_err);
}

void func_0x03or0x04_handler(modbus_package *query, modbus_package *modbus_answer)
{
  resp_0x03_0x04 *resp = (resp_0x03_0x04*)modbus_answer->data;
  req_0x03_0x04 *req =(req_0x03_0x04*)query->data;

  if(req->count<0x01 || req->count>0x007D){
	  get_exception(query->func, 0x03, modbus_answer);
	  return;
  }
  if((req->address+req->count)>REGISTER_LEN){
	  get_exception(query->func, 0x02, modbus_answer);
	  return;
  }

  bool (*read_table) (int16_t address, int16_t *val);
  if(query->func==0x03)
  	  read_table = read_holding_registers;
  	else
  	  read_table = read_inputs;
	uint16_t length_or_err = fill_resp_0x03_0x04(modbus_answer->data, req->address, req->count, read_table);
	if(!length_or_err){
		get_exception(query->func, 0x04 , modbus_answer);
		return;
	}
  modbus_answer->length = length_or_err;
}

void func_0x05_handler(modbus_package *query, modbus_package *modbus_answer)
{
	resp_0x05_0x06 *resp = (resp_0x05_0x06*)modbus_answer->data;
	req_0x05_0x06 *req = (req_0x05_0x06*)query->data;

	if(!(req->value==0x00 || req->value==0xFF00)){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if(req->address>=REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	if(!write_coils(req->address, req->value>>8)){
		get_exception(query->func, 0x04, modbus_answer);
		return;
	}
	uint16_t length_or_err = fill_resp_0x05_0x06(resp, req->address, req->value);
	modbus_answer->length= length_or_err;

}

void func_0x06_handler(modbus_package *query,modbus_package *modbus_answer)
{
	resp_0x05_0x06 *resp = (resp_0x05_0x06*)modbus_answer->data;
	req_0x05_0x06 *req = (req_0x05_0x06*)query->data;

	if(req->value>0xFFFF){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if(req->address>=REGISTER_LEN ){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	if(!write_holding_registers(req->address, req->value)){

		get_exception(query->func, 0x04, modbus_answer);
		return;
	}
	uint16_t length_or_err = fill_resp_0x05_0x06(resp, req->address, req->value);
	modbus_answer->length= length_or_err;
}

void func_0x0F_handler(modbus_package *query, modbus_package *modbus_answer)
{
	resp_0x0F_0x10 *resp = (resp_0x0F_0x10*)modbus_answer->data;
	req_0x0F *req = (req_0x0F*)query->data;
	uint16_t len_data;
	if((req->count<=0||req->quantity>0x07B0) && (req->count !=query->length-7)){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if((req->address+req->quantity)>REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	if(req->quantity%8)
		len_data=req->quantity/8+1;
	else
		len_data=req->quantity/8;

	for(int i=0;i<len_data;i++){
		for(int j=0;j<8 && j<req->quantity;j++)
	  {
			uint8_t val = get_coil_value((uint8_t*)req->bytes,i,j);
		if(!write_coils(req->address+i*8+j, val)){
			get_exception(query->func, 0x04 , modbus_answer);
			return;
		}
	  }
	}
	uint16_t length_or_err = fill_resp_0x0F_0x10(resp, req->address, req->quantity);
	modbus_answer->length= length_or_err;
}


void func_0x10_handler(modbus_package *query,modbus_package *modbus_answer){

	resp_0x0F_0x10 *resp = (resp_0x0F_0x10*)modbus_answer->data;
	req_0x10 *req = (req_0x10*)query->data;
	if((req->quantity<1)&&(req->quantity>0x07B) && (req->quantity != req->count/2)){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if((req->address+req->quantity)>REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}
	for(int i=0;i<req->count;i++){
		dbgprintf("value %d %d\t", i,req->bytes[i]);
		if(!write_holding_registers(req->address+i,*(int16_t*)&req->bytes[i*2]))
		{
			get_exception(query->func, 0x04, modbus_answer);
			return;
		}
	}
	uint16_t length_or_err = fill_resp_0x0F_0x10(resp, req->address, req->quantity);
	modbus_answer->length= length_or_err;
}

void func_0x16_handler (modbus_package *query,  modbus_package *modbus_answer){

	resp_0x16 *resp = (resp_0x16*)modbus_answer->data;
	req_0x16 *req = (req_0x16*)query->data;

	if(req->address>=REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}
	int16_t value;

	if(!read_holding_registers(req->address,&value)){
		get_exception(query->func, 0x04, modbus_answer);
		return;
	}
	value = (value & req->and_mask)|(req->or_mask & (~req->and_mask));
	if(!write_holding_registers(req->address, value)){
		get_exception(query->func, 0x04, modbus_answer);
		return;
	}

	uint16_t length_or_err = fill_resp_0x16(resp, req->address,req->and_mask, req->or_mask);
	modbus_answer->length= length_or_err;

}

void func_0x17_handler(modbus_package *query,modbus_package *modbus_answer){

	resp_0x17 *resp = (resp_0x17*)modbus_answer->data;
	req_0x17 *req = (req_0x17*)query->data;
	int16_t write_value;
	int16_t read_value;
	if((req->read_quantity<1)||(req->write_quantity<1)||(req->read_quantity>0x07D)||(req->write_quantity>0x079)||(req->write_count!=req->write_quantity*2)){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if(((req->read_address+req->read_quantity)>REGISTER_LEN)||(req->write_address+req->write_quantity)>REGISTER_LEN)
		{
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	for(int i = 0;i<req->write_quantity;i++){
		write_value = req->bytes[2*i]|req->bytes[2*i+1]<<8;
		if(!write_holding_registers(req->write_address+i,write_value)){
			get_exception(query->func, 0x04, modbus_answer);
			return;
		}
	}

	for(int i=0;i<req->read_quantity;i++){
		if(!read_holding_registers(req->read_address+i, &read_value)){
			get_exception(query->func,0x04, modbus_answer);
			return;
		}
		resp->bytes[2*i]= read_value;
		resp->bytes[2*i+1]= read_value>>8;
	}
	modbus_answer->length = 1+1+1+req->read_quantity*2;
	resp->count = req->read_quantity*2;
}





void modbus_query_handler(modbus_package* query,modbus_package *modbus_answer)
{
	form_MBAP(query->tid, query->pid, query->uid, query->func, modbus_answer);
	dbgprintf("tid: %u pid:%u  func:%u uid:%u  length:%u\n\r",query->tid, query->pid,query->func,query->uid,query->length);
	switch(query->func)
	{
		case 0x01:
		case 0x02:
		 {
			 func_0x01or0x02_handler(query, modbus_answer);
			 break;
		}
		case 0x03:
		case 0x04:
		{
			func_0x03or0x04_handler(query,modbus_answer);
			break;
		}
		case 0x05:
		{
			func_0x05_handler(query,modbus_answer);
			break;
		}
		case 0x06:
		{
			func_0x06_handler( query, modbus_answer);
			break;
		}
		case 0x0F:
		{
			func_0x0F_handler(query, modbus_answer);
			break;
		}
		case 0x10:
		{
			func_0x10_handler(query, modbus_answer);
			break;
		}
		case 0x16:{
			func_0x16_handler(query, modbus_answer);
			break;
		}
		case 0x17:
		{
			func_0x17_handler(query,modbus_answer);
			break;
		}
		default:
		{
			get_exception(query->func, 0x01, modbus_answer);
			break;
		}
	}
	query = NULL;
	resp_change_endian(modbus_answer);
}

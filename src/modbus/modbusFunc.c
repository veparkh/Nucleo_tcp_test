#include "modbusRegister.h"
#include "modbusFunc.h"
#include "modbusGet.h"
#include <hal.h>
#include <ch.h>

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

void func_0x01or0x02_handler (modbus_package *query, int16_t counter, int16_t address, modbus_package *modbus_answer){

	uint8_t len_data,i, data=0, j=0, val;
	if(counter<0x01 || counter>0x07D0){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if(address>=REGISTER_LEN || address+counter>REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}
	if(counter%8)
		len_data=counter/8+1;
	else
		len_data=counter/8;
	modbus_answer->length=3+len_data;
	modbus_answer->data[MB_TCP_LENGTH]=len_data;
	bool (*table) (int16_t address, uint8_t *val);
	if(query->func==0x01)
	  table = read_coils;
	else
	  table = read_discrete_input;

	for(int k=0;k<len_data;k++)
	{
	  i=0;
	  while((counter>0) && (i<8))
	  {
		if(!table(address+j, &val)){
			get_exception(query->func, 0x04 , modbus_answer);
			return;
		}
		data=data+(val<<i);
		i++;
		counter--;
		j++;
	  }
	  modbus_answer->data[MB_TCP_DATA+k]=data;
	  data = 0;
	}
}

void func_0x03or0x04_handler(modbus_package *query, int16_t counter,int16_t address, modbus_package *modbus_answer)
{
  int16_t value;
  uint8_t HiByteValue=0, LoByteValue=0;

  if(counter<0x01 || counter>0x007D){
	  get_exception(query->func, 0x03, modbus_answer);
	  return;
  }
  if(address>=REGISTER_LEN || address+counter>REGISTER_LEN){
	  get_exception(query->func, 0x02, modbus_answer);
	  return;
  }
  bool (*read_table) (int16_t address, int16_t *val);
  if(query->func==0x03)
  	  read_table = read_holding_registers;
  	else
  	  read_table = read_inputs;

  for(int i=0;i<counter;i++)
  {
	if(!read_table(address+i,&value)){
		get_exception(query->func,0x04 , modbus_answer);
		return;
	}
    HiByteValue=value>>8;
    LoByteValue=value;
    modbus_answer->data[MB_TCP_DATA+i*2] = HiByteValue;
    modbus_answer->data[MB_TCP_DATA+i*2+1] = LoByteValue;
  }
  modbus_answer->length = counter*2+3;
  modbus_answer->data[MB_TCP_LENGTH] = counter*2;
}

void func_0x05_handler(modbus_package *query, int16_t address,uint16_t value, modbus_package *modbus_answer)
{
	if(!(value==0x00 || value==0xFF00)){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if(address>=REGISTER_LEN){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	if(!write_coils(address, value>>8)){
		get_exception(query->func, 0x04, modbus_answer);
		return;
	}
	modbus_answer->length= 0x06;
	modbus_answer->data[MB_TCP_ADDRESS] = address>>8;
	modbus_answer->data[MB_TCP_ADDRESS+1] = address;
	modbus_answer->data[MB_TCP_TAKE_DATA]=value>>8;
	modbus_answer->data[MB_TCP_TAKE_DATA+1]=value;
}

void func_0x06_handler(modbus_package *query, int16_t address,int16_t value,modbus_package *modbus_answer)
{
	if(value>0xFFFF){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if(address>=REGISTER_LEN ){
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}

	if(!write_holding_registers(address, value)){

		get_exception(query->func, 0x04, modbus_answer);
		return;
	}
	modbus_answer->length= 0x06;
	modbus_answer->data[MB_TCP_ADDRESS] = address>>8;
	modbus_answer->data[MB_TCP_ADDRESS+1] = address;
	modbus_answer->data[MB_TCP_TAKE_DATA]=value>>8;
	modbus_answer->data[MB_TCP_TAKE_DATA+1]=value;
}

void func_0x10or0x0F_handler(modbus_package *query, int16_t address,int16_t count,modbus_package *modbus_answer)
{
	uint16_t len_data;
	if(query->func==0x0F){
		if((count<0||count>0x07B0) && (query->data[MB_TCP_COUNT_BYTE] !=query->length-7)){
			get_exception(query->func, 0x03, modbus_answer);
			return;
		}
		if(address<0||address+count>REGISTER_LEN){
			get_exception(query->func, 0x02, modbus_answer);
			return;
		}
		if(count%8)
			len_data=count/8+1;
		else
			len_data=count/8;
		for(int i=0, j = 0;i<len_data;i++,j =0){
			for(;j<8 &&j<count;j++)
		  {
				uint8_t val = get_coil_value(query,i,j);
			if(!write_coils(address+i*8+j, val)){
				get_exception(query->func, 0x04 , modbus_answer);
				return;
			}
		  }
		}
	}
	else{
		if((count<0||count>0x07B) && (query->data[MB_TCP_COUNT_BYTE] ==query->length-7)){
			get_exception(query->func, 0x03, modbus_answer);
			return;
		}
		if(address<0||address+count>REGISTER_LEN){
			get_exception(query->func, 0x02, modbus_answer);
			return;
		}
		for(int i=0;i<count;i++){
			if(!write_holding_registers(address+i, mb_get_multiple_analog_register(query,i)))
			{
				get_exception(query->func, 0x04, modbus_answer);
				return;
			}
		}
	}
	modbus_answer->length= 0x06;
	modbus_answer->data[MB_TCP_ADDRESS] = address>>8;
	modbus_answer->data[MB_TCP_ADDRESS+1] = address;
	modbus_answer->data[MB_TCP_TAKE_DATA]=count>>8;
	modbus_answer->data[MB_TCP_TAKE_DATA+1]=count;
}

void func_0x17_handler(modbus_package *query,modbus_package *modbus_answer){

	int16_t write_start_address = query->data[4]<<8|query->data[5];
	int8_t write_count = query->data[6]<<8|query->data[7];
	int16_t write_value;
	int16_t read_value;
	int16_t read_start_address = query->data[0]<<8|query->data[1];
	int16_t read_count = query->data[2]<<8|query->data[3];
	if((read_count<0||read_count>0x07D)||(write_count<0||write_count>0x079)||(query->data[8]!=write_count*2)){
		get_exception(query->func, 0x03, modbus_answer);
		return;
	}
	if((read_start_address<0||read_start_address+read_count>REGISTER_LEN)||(write_start_address<0||write_start_address+write_count>REGISTER_LEN))
		{
		get_exception(query->func, 0x02, modbus_answer);
		return;
	}
	for(int i = 0;i<write_count;i++){
		write_value = query->data[9+2*i]<<8 | query->data[9+2*i+1];
		if(!write_holding_registers(write_start_address+i,write_value)){
			get_exception(query->func, 0x04, modbus_answer);
			return;
		}
	}
	for(int i=0;i<read_count;i++){
		if(!read_holding_registers(read_start_address+i, &read_value)){
			get_exception(query->func,0x04, modbus_answer);
			return;
		}
		modbus_answer->data[2*i+1] = read_value>>8;
		modbus_answer->data[2*i+2] = read_value;
	}
	modbus_answer->length= 1+1+1+read_count*2;
	modbus_answer->data[MB_TCP_LENGTH] = 2*read_count;
}

void modbus_query_handler(modbus_package* query,modbus_package *modbus_answer)
{
	int16_t address;
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
		case MB_FUN_READ_DISCRETE_INPUT_REGISTER:
		 {
			 func_0x01or0x02_handler(query,count, address,modbus_answer);
			 break;
		}
		case MB_FUN_READ_ANALOG_OUTPUT_REGISTER:
		case MB_FUN_READ_ANALOG_INPUT_REGISTER:
		{
			func_0x03or0x04_handler(query, count, address,modbus_answer);
			break;
		}
		case MB_FUN_WRITE_DISCRETE_OUTPUT_REGISTER://count в данном случае значение
		{
			func_0x05_handler(query, address, count,modbus_answer);
			break;
		}
		case MB_FUN_WRITE_ANALOG_OUTPUT_REGISTER:
		{
			func_0x06_handler( query, address, modbustcp_get_value(query),modbus_answer);
			break;
		}
		case MB_FUN_WRITE_MULTIPLE_DIGITAL_OUTPUT_REGISTER:
		case MB_FUN_WRITE_MULTIPLE_ANALOG_OUTPUT_REGISTER:
		{
			func_0x10or0x0F_handler(query,  address, count,modbus_answer);
			break;
		}
		case MB_FUN_WRITE_READ_MULTIPLE_ANALOG_OUTPUT_REGISTER:
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
	change_endian(modbus_answer);
}











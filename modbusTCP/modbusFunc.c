#include "modbusRegister.h"
#include "modbusFunc.h"
#include "modbusGet.h"
#include <hal.h>
#include <ch.h>

extern modbus_package modbus_answer;
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



//Forming a response to reading the input analog register. Modbus TCP function-0x04.
int16_t modbusTCP_Read_Analog_Input_Register(modbus_package *query, int16_t counter,int16_t address)
{
	return modbustcp_send_answer_fun_0x03or0x04( query->tid, query->pid, query->uid, query->func, counter, address);
}

//Forming a response to reading the output analog register. Modbus TCP function-0x03.
int16_t modbusTCP_Read_Analog_Output_Register(modbus_package *query, int16_t counter, int16_t address)
{
	return modbustcp_send_answer_fun_0x03or0x04( query->tid, query->pid, query->uid, query->func, counter, address);
}

//Forming a response to reading a discrete output register. Modbus TCP function-0x01.
int16_t modbusTCP_Read_Discrete_Output_Register(modbus_package *query, int16_t counter,int16_t address)
{
	return modbustcp_send_answer_fun_0x01or0x02(query->tid, query->pid, query->uid, query->func, counter, address);
}

//Forming a response to reading a discrete input register. Modbus TCP function-0x01.
int16_t modbusTCP_Read_Discrete_Input_Register(modbus_package *query, int16_t counter,int16_t address)
{
    return modbustcp_send_answer_fun_0x01or0x02( query->tid, query->pid, query->uid, query->func, counter, address);
}

//Formation of the response to the recording of the analog register. Modbus TCP function-0x06.
int16_t modbusTCP_Write_Analog_Register(modbus_package *query, int16_t address,int16_t value)
{
	return modbustcp_send_answer_fun_0x06( query->tid, query->pid, query->uid, query->func, address, value);
}

//Formation of the response to the recording of the multiple analog register. Modbus TCP function-0x10.
int16_t modbusTCP_Write_Multiple_Analog_Register(modbus_package *query, int16_t address,int16_t count)
{
    return modbustcp_send_answer_fun_0x10or0x0F(query->tid, query->pid, query->uid, query->func, address, count);
}

//Formation of a response to the recording of a discrete register. Modbus TCP function-0x05.
int16_t modbusTCP_Write_Discrete_Register(modbus_package *query, int16_t address,uint8_t boole)
{
	return modbustcp_send_answer_fun_0x05(query->tid, query->pid, query->uid, query->func, address, boole);
}

int16_t modbusTCP_Write_Multiple_Discrete_Register(modbus_package *query, int16_t address,int16_t count){
	return modbustcp_send_answer_fun_0x10or0x0F(query->tid, query->pid, query->uid, query->func, address, count);
}

int16_t modbusTCP_Read_Write_Multiple_Analog_Output_Register(modbus_package *query, int16_t count,int16_t start_address)///////////////////////////////
{
	return modbustcp_send_answer_fun_0x17(query->tid, query->pid, query->uid, query->func, count, start_address);
}

int16_t modbustcp_send_answer_fun_0x03or0x04(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t counter,int16_t address)
{
  int16_t len=0, len_data, value;
  uint8_t HiByteValue=0, LoByteValue=0,k=0;
  modbus_answer.tid = tid>>8|tid<<8;
  modbus_answer.pid = pid;
  modbus_answer.length = ((counter*2+3)>>8)|((counter*2+3)<<8);
  modbus_answer.uid = uid;
  modbus_answer.func = func;
  len_data=counter*2;
  modbus_answer.data[MB_TCP_LENGTH] = len_data;
  while((address+k)<REGISTER_LEN && k<counter)
  {
	  if(func==0x03){
		value = Analog_Output_Register[address+k];
	  }
	  else{
		value = Analog_Input_Register[address+k];
	  }

    HiByteValue=value>>8;
    LoByteValue=value;
    modbus_answer.data[MB_TCP_DATA+k*2] = HiByteValue;
    modbus_answer.data[MB_TCP_DATA+k*2+1] = LoByteValue;
    k++;
  }
  len=counter*2+9;
  return  len;
}

int16_t modbustcp_send_answer_fun_0x05(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t address,uint8_t boole)
{
	modbus_answer.tid = tid>>8|tid<<8;;
	modbus_answer.pid = pid;
	modbus_answer.length= 0x0600;
	modbus_answer.uid = uid;
	modbus_answer.func  = func;
	modbus_answer.data[MB_TCP_ADDRESS] = address>>8;
	modbus_answer.data[MB_TCP_ADDRESS+1] = address;
  if(boole)
  {
	modbus_answer.data[MB_TCP_TAKE_DATA]=0xFF;
	modbus_answer.data[MB_TCP_TAKE_DATA+1]=0x00;
  }
  else
  {
	modbus_answer.data[MB_TCP_TAKE_DATA]=0x00;
	modbus_answer.data[MB_TCP_TAKE_DATA+1]=0x00;
  }
  return  12;
}

int16_t modbustcp_send_answer_fun_0x01or0x02(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t counter, int16_t address)
{
  uint8_t len=0,len_next=0, len_data,k=0,i, data=0, j=0;
  modbus_answer.tid = tid>>8|tid<<8;;
  modbus_answer.pid = pid;
  if(counter%8)
    len_data=counter/8+1;
  else
    len_data=counter/8;
  len_next=3+len_data;
  modbus_answer.length=(len_next>>8)|(len_next<<8);
  modbus_answer.uid = uid;
  modbus_answer.func  = func;
  modbus_answer.data[MB_TCP_LENGTH]=len_data;
  while((address+k)<REGISTER_LEN && k<len_data)
  {
      i=0;
      while((counter>0) && (i<8))
      {
    	if(func==0x01){
    		data=data+((Discrete_Output_Register[address+j])<<i);
    	}
    	else{
            data=data+((Discrete_Input_Register[address+j])<<i);
    	}
        i++;
        counter--;
        j++;
      }
      modbus_answer.data[MB_TCP_DATA+k]=data;
      data=0;
      k++;
  }
  len=9+len_data;
  return  len;
}

int16_t modbustcp_send_answer_fun_0x06(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t address,int16_t value)
{
	modbus_answer.tid = tid>>8|tid<<8;
	modbus_answer.pid = pid;
	modbus_answer.length= 0x0600;
	modbus_answer.uid = uid;
	modbus_answer.func  = func;
	modbus_answer.data[MB_TCP_ADDRESS] = address>>8;
	modbus_answer.data[MB_TCP_ADDRESS+1] = address;
	modbus_answer.data[MB_TCP_TAKE_DATA]=value>>8;
	modbus_answer.data[MB_TCP_TAKE_DATA+1]=value;
  return  12;
}

int16_t modbustcp_send_answer_fun_0x10or0x0F(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t address,int16_t count)
{
	modbus_answer.tid = tid>>8|tid<<8;
	modbus_answer.pid = pid;
	modbus_answer.length= 0x0600;
	modbus_answer.uid = uid;
	modbus_answer.func  = func;
	modbus_answer.data[MB_TCP_ADDRESS] = address>>8;
	modbus_answer.data[MB_TCP_ADDRESS+1] = address;
	modbus_answer.data[MB_TCP_TAKE_DATA]=count>>8;
	modbus_answer.data[MB_TCP_TAKE_DATA+1]=count;
	return 12;
}

int16_t modbustcp_send_answer_fun_0x17(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t count,int16_t start_address){ // @suppress("Type cannot be resolved")
		modbus_answer.tid = tid>>8|tid<<8;
		modbus_answer.pid = pid;
		modbus_answer.length= 1+1+1+count*2;
		modbus_answer.length = modbus_answer.length>>8|modbus_answer.length<<8;
		modbus_answer.uid = uid;
		modbus_answer.func  = func;
		modbus_answer.data[MB_TCP_LENGTH] = 2*count;
		dbgprintf("start_address:%d", start_address);

		for(int i=0;i<count;i++){
			dbgprintf("start_address:%d/r/n",start_address+i);
			dbgprintf("value:%d/r/n",MB_READ_ANALOG_OUTPUT_REGISTER(start_address+i));
			modbus_answer.data[2*i+1] = MB_READ_ANALOG_OUTPUT_REGISTER(start_address+i)>>8;
			modbus_answer.data[2*i+2] = MB_READ_ANALOG_OUTPUT_REGISTER(start_address+i);
		}
		return count*2+9;
}

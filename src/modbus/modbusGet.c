#include "ch.h"
#include "hal.h"
#include "modbusFunc.h"
#include "modbusGet.h"


//The Function returns the Register Address
int16_t modbustcp_get_address(modbus_package* data)
{
  int16_t address = data->data[MB_TCP_ADDRESS] << 8U;
  address |= data->data[MB_TCP_ADDRESS + 1];
  return address;
}
//The Function returns the Number of Registers
int16_t modbustcp_get_count(modbus_package* data)
{
  int16_t count = data->data[MB_TCP_COUNT] << 8U;
  count |= data->data[MB_TCP_COUNT + 1];
  return count;
}

//The Function returns 
uint8_t modbustcp_get_boll_value(modbus_package* data)
{
  uint8_t boll = data->data[MB_TCP_TAKE_DATA];
  if(boll)
    return 1;
  else
    return 0;
}

//The Function returns Value
int16_t modbustcp_get_value(modbus_package* data)
{
  int16_t value = data->data[MB_TCP_TAKE_DATA] << 8U;
  value |= data->data[MB_TCP_TAKE_DATA + 1];
  return value;
}

//The Function returns count byte
int8_t modbustcp_get_count_byte(modbus_package* data)
{
  int8_t value = data->data[MB_TCP_COUNT_BYTE];
  return value;
}

//The Function returns multiple register
int16_t mb_get_multiple_analog_register(modbus_package* data,uint8_t count)
{
  uint8_t multiple=MB_TCP_MULTIPLE_REGISTER+count*2;
  int16_t value = data->data[multiple] << 8U;
  value |= data->data[multiple + 1];
  return value;
}

int8_t get_coil_value(modbus_package *query,uint16_t i, uint8_t shift){
	if(query->data[MB_TCP_MULTIPLE_REGISTER+i] & 1<<shift)
		return 1;
	else
		return 0;
}

void change_endian(modbus_package *query){
	query->tid = query->tid>>8 | query->tid<<8;
	query->length = query->length>>8 | query->length<<8;
	query->pid = query->pid>>8 | query->pid<<8;
}

bool is_modbus_query(modbus_package *query,int16_t buflen){
		 return
				 query->pid == 0
				 &&
				 query->uid == MY_ID
				 &&
				 query->length == buflen - 6;
}

void get_exception(uint8_t func, uint8_t err_code, modbus_package *modbus_answer){
	modbus_answer->func = 0x80+func;
	modbus_answer->data[0] = err_code;
	modbus_answer->length=3;
}


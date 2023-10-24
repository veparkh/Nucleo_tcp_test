#include "ch.h"
#include "hal.h"
#include "modbusFunc.h"

int16_t Analog_Input_Register[REGISTER_LEN];

uint8_t Discrete_Input_Register[REGISTER_LEN];

int16_t Analog_Output_Register[REGISTER_LEN];

uint8_t Discrete_Output_Register[REGISTER_LEN];


void fill_tables(void){
    for(int i =0;i<REGISTER_LEN;i++){
    	Analog_Output_Register[i]=i;
    	Discrete_Input_Register[i] = i % 2;
    	Analog_Input_Register[i]=i;
    	Discrete_Output_Register[i] = i % 2;
    }
}

mb_error communication_session_error;

void setError(bool is_read_err, bool is_write_err,bool is_mb_exception, int8_t err_code ){
	communication_session_error.is_read_err = is_read_err;
	communication_session_error.is_write_err=is_write_err;
	communication_session_error.is_mb_exception=is_mb_exception;
	communication_session_error.err_code = err_code;
}

mb_error getError(void){
	return communication_session_error;
}
void form_MBAP(uint16_t tid,uint16_t pid, uint8_t uid,uint8_t func,modbus_package *query){
	query->func = func;
	query->pid = pid;
	query->tid = tid;
	query->uid = uid;
}

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

bool is_modbus_package(modbus_package *query,int16_t buflen){
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

bool read_discrete_input(int16_t address, uint8_t *val){
	*val = Discrete_Input_Register[address];
	return true;
}

bool read_coils(int16_t address,uint8_t *val){
	*val = Discrete_Output_Register[address];
	return true;
}
bool write_coils(int16_t address,uint8_t val){
	if(val)
		Discrete_Output_Register[address] = 1;
	else
		Discrete_Output_Register[address] = 0;
	return true;
}
bool read_inputs(int16_t address,int16_t *val){
	*val = Analog_Input_Register[address];
	return true;
}
bool write_holding_registers(int16_t address,int16_t val){
	Analog_Output_Register[address] = val;
	return true;
}
bool read_holding_registers(int16_t address,int16_t *val){
	*val = Analog_Output_Register[address];
	return true;
}




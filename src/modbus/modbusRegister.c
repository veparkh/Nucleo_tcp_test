#include "modbusRegister.h"
#include "stdlib.h"

int16_t Analog_Input_Register[REGISTER_LEN];
uint8_t Discrete_Input_Register[REGISTER_LEN];
int16_t Analog_Output_Register[REGISTER_LEN];
uint8_t Discrete_Output_Register[REGISTER_LEN];

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


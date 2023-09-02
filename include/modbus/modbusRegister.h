#ifndef __MODBUSREGISTER_H
#define __MODBUSREGISTER_H

#include "ch.h"
#include "hal.h"


#define REGISTER_LEN 100

extern int16_t Analog_Input_Register[REGISTER_LEN];
extern int16_t Analog_Output_Register[REGISTER_LEN];


extern uint8_t Discrete_Input_Register[REGISTER_LEN];
extern uint8_t Discrete_Output_Register[REGISTER_LEN];


bool read_discrete_input(int16_t address, uint8_t *val);

bool read_coils(int16_t address,uint8_t *val);
bool write_coils(int16_t address,uint8_t val);


bool read_inputs(int16_t address,int16_t *val);

bool write_holding_registers(int16_t address,int16_t val);
bool read_holding_registers(int16_t address,int16_t *val);




#endif /* MODBUSREGISTER */

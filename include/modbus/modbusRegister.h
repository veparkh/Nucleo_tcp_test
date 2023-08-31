#ifndef __MODBUSREGISTER_H
#define __MODBUSREGISTER_H

#include "ch.h"
#include "hal.h"


#define REGISTER_LEN 100

extern int16_t Analog_Input_Register[REGISTER_LEN];
extern int16_t Analog_Output_Register[REGISTER_LEN];


extern uint8_t Discrete_Input_Register[REGISTER_LEN];
extern uint8_t Discrete_Output_Register[REGISTER_LEN];


#define MB_READ_ANALOG_OUTPUT_REGISTER(addr)         (*(Analog_Output_Register+addr))
#define MB_WRITE_ANALOG_OUTPUT_REGISTER(addr,val)    (*(Analog_Output_Register+addr)=val)

#define MB_READ_ANALOG_INPUT_REGISTER(addr)         (*(Analog_Input_Register+addr))



#define MB_READ_DISCRETE_OUTPUT_REGISTER(addr)       (*(Discrete_Output_Register+addr))
#define MB_WRITE_DISCRETE_OUTPUT_REGISTER(addr,val)  (*(Discrete_Output_Register+addr)=val)

#define MB_READ_DISCRET_INPUT_REGISTER(addr)       (*(Discrete_Input_Register+addr))

bool read_discrete_input(int16_t address, uint8_t *val);

bool read_coils(int16_t address,uint8_t *val);
bool write_coils(int16_t address,uint8_t val);


bool read_inputs(int16_t address,int16_t *val);

bool write_holding_registers(int16_t address,int16_t val);
bool read_holding_registers(int16_t address,int16_t *val);




#endif /* MODBUSREGISTER */

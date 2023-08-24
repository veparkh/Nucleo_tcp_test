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


#endif /* MODBUSREGISTER */

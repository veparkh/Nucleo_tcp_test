#ifndef __MODBUSREGISTER_H
#define __MODBUSREGISTER_H

#include "ch.h"
#include "hal.h"


#define REGISTER_LEN 100

extern int16_t Analog_Register[REGISTER_LEN];
extern uint8_t Discrete_Register[REGISTER_LEN];

#define MB_READ_REG_INT16(addr)         (*(Analog_Register+addr))
#define MB_WRITE_REG_INT16(addr,val)    (*(Analog_Register+addr)=val)


#define MB_READ_DISCRET_REG(addr)       (*(Discrete_Register+addr))
#define MB_WRITE_DISCRET_REG(addr,val)  (*(Discrete_Register+addr)=val)

#endif /* MODBUSREGISTER */

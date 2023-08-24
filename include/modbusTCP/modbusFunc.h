#ifndef __MODBUSFUNC_H
#define __MODBUSFUNC_H

#include<modbusGet.h>
#define MY_ID							1
#define MB_TCP_LENGTH					0
#define MB_TCP_ADDRESS                  0
#define MB_TCP_COUNT                    2
#define MB_TCP_COUNT_BYTE               4
#define MB_TCP_MULTIPLE_REGISTER        5
#define MB_TCP_DATA                     1
#define MB_TCP_TAKE_DATA                2



#define MB_FUN_READ_DISCRETE_OUTPUT_REGISTER      	 	0x01
#define MB_FUN_READ_DISCRETE_INPUT_REGISTER        		0x02
#define MB_FUN_READ_ANALOG_OUTPUT_REGISTER				0x03
#define MB_FUN_READ_ANALOG_INPUT_REGISTER				0x04
#define MB_FUN_WRITE_DISCRETE_OUTPUT_REGISTER			0x05
#define MB_FUN_WRITE_ANALOG_OUTPUT_REGISTER       		0x06
#define MB_FUN_WRITE_MULTIPLE_ANALOG_OUTPUT_REGISTER  	0x10
#define MB_FUN_WRITE_MULTIPLE_DIGITAL_OUTPUT_REGISTER 	0x0F

int16_t modbusTCP_Read_Analog_Input_Register(modbus_package *query, int16_t counter, int16_t address);
int16_t modbusTCP_Read_Analog_Output_Register(modbus_package *query, int16_t counter, int16_t address);
int16_t modbusTCP_Read_Discrete_Output_Register(modbus_package *query, int16_t counter, int16_t address);
int16_t modbusTCP_Read_Discrete_Input_Register(modbus_package *query, int16_t counter, int16_t address);
int16_t modbusTCP_Write_Analog_Register(modbus_package *query, int16_t address,int16_t value);
int16_t modbusTCP_Write_Discrete_Register(modbus_package *query, int16_t address,uint8_t boole);
int16_t modbusTCP_Write_Multiple_Analog_Register(modbus_package *query, int16_t address,int16_t count);
int16_t modbusTCP_Write_Multiple_Discrete_Register(modbus_package *query, int16_t address,int16_t count);

int16_t modbustcp_send_answer_fun_0x01or0x02(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t counter, int16_t address);
int16_t modbustcp_send_answer_fun_0x03or0x04(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t counter, int16_t address);
int16_t modbustcp_send_answer_fun_0x05(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t address,uint8_t boole);
int16_t modbustcp_send_answer_fun_0x06(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t address,int16_t value);
int16_t modbustcp_send_answer_fun_0x10or0x0F(int16_t tid, int16_t pid, uint8_t uid, uint8_t func, int16_t address,int16_t count_byte);



#endif /* MODBUSFUNC */

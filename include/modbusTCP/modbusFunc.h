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



#define MB_FUN_READ_DISCRETE_OUTPUT_REGISTER      	 		0x01
#define MB_FUN_READ_DISCRETE_INPUT_REGISTER        			0x02
#define MB_FUN_READ_ANALOG_OUTPUT_REGISTER					0x03
#define MB_FUN_READ_ANALOG_INPUT_REGISTER					0x04
#define MB_FUN_WRITE_DISCRETE_OUTPUT_REGISTER				0x05
#define MB_FUN_WRITE_ANALOG_OUTPUT_REGISTER       			0x06
#define MB_FUN_WRITE_MULTIPLE_DIGITAL_OUTPUT_REGISTER 		0x0F
#define MB_FUN_WRITE_MULTIPLE_ANALOG_OUTPUT_REGISTER  		0x10
#define MB_FUN_WRITE_READ_MULTIPLE_ANALOG_OUTPUT_REGISTER	0x17


int16_t modbustcp_send_answer_fun_0x01or0x02(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer);
int16_t modbustcp_send_answer_fun_0x03or0x04(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer);
int16_t modbustcp_send_answer_fun_0x05(modbus_package *query, int16_t address,uint16_t value,modbus_package *modbus_answer);
int16_t modbustcp_send_answer_fun_0x06(modbus_package *query, int16_t address,int16_t value,modbus_package *modbus_answer);
int16_t modbustcp_send_answer_fun_0x10or0x0F(modbus_package *query, int16_t address,int16_t count_byte,modbus_package *modbus_answer);
int16_t modbustcp_send_answer_fun_0x17(modbus_package *query,modbus_package *modbus_answer);
uint8_t fill_exception(uint8_t func, uint8_t err_code, modbus_package *modbus_answer);
bool is_modbus_query(modbus_package *query,int16_t buflen);
#endif /* MODBUSFUNC */
void change_endian(modbus_package *query);
int8_t get_coil_value(modbus_package *query,uint16_t i, uint8_t shift);
//int write_log(struct netconn *conn, modbus_package *query);

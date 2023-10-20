#ifndef MODBUS_INCLUDE_SERVER_RESPONSEHANDLE_H_
#define MODBUS_INCLUDE_SERVER_RESPONSEHANDLE_H_

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
#define MB_MASK_WRITE_ANALOG_OUTPUT_REGISTER				0x16
#define MB_FUN_WRITE_READ_MULTIPLE_ANALOG_OUTPUT_REGISTER	0x17


void func_0x01or0x02_handler(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer);
void func_0x03or0x04_handler(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer);
void func_0x05_handler(modbus_package *query, int16_t address,uint16_t value,modbus_package *modbus_answer);
void func_0x06_handler(modbus_package *query, int16_t address,int16_t value,modbus_package *modbus_answer);
void func_0x10or0x0F_handler(modbus_package *query, int16_t address,int16_t count_byte,modbus_package *modbus_answer);
void func_0x17_handler(modbus_package *query,modbus_package *modbus_answer);
void func_0x16_handler (modbus_package *query, modbus_package *modbus_answer);
void modbus_query_handler(modbus_package* query,modbus_package *modbus_answer);

#endif /* MODBUS_INCLUDE_SERVER_RESPONSEHANDLE_H_ */

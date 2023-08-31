#ifndef __MODBUSGET_H
#define __MODBUSGET_H

#include <stdint.h>
#include <stdbool.h>
typedef struct modbus_package{
	uint16_t tid;
	uint16_t pid;
	uint16_t length;
	uint8_t uid;
	uint8_t func;
	uint8_t data[500];
}modbus_package;

int16_t modbustcp_get_address(modbus_package* data);
int16_t modbustcp_get_count(modbus_package* data);
uint8_t modbustcp_get_boll_value(modbus_package* data);
int16_t modbustcp_get_value(modbus_package* data);
int8_t modbustcp_get_count_byte(modbus_package* data);
int16_t mb_get_multiple_analog_register(modbus_package* data,uint8_t count);
void change_endian(modbus_package *query);
int8_t get_coil_value(modbus_package *query,uint16_t i, uint8_t shift);
void get_exception(uint8_t func, uint8_t err_code, modbus_package *modbus_answer);
bool is_modbus_query(modbus_package *query,int16_t buflen);
#endif /* MODBUSGET */

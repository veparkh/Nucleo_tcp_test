#ifndef __MODBUSGET_H
#define __MODBUSGET_H

typedef struct modbus_package{
	uint16_t tid;
	int16_t pid;
	int16_t length;
	uint8_t uid;
	uint8_t func;
	uint8_t data[100];
}modbus_package;

int16_t modbustcp_get_address(modbus_package* data);
int16_t modbustcp_get_count(modbus_package* data);
uint8_t modbustcp_get_boll_value(modbus_package* data);
int16_t modbustcp_get_value(modbus_package* data);
int8_t modbustcp_get_count_byte(modbus_package* data);
int16_t modbustcp_get_multiple_register(modbus_package* data,uint8_t count);

#endif /* MODBUSGET */

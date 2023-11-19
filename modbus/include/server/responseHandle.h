#ifndef MODBUS_INCLUDE_SERVER_RESPONSEHANDLE_H_
#define MODBUS_INCLUDE_SERVER_RESPONSEHANDLE_H_

#include "dataStructures.h"

void func_0x01or0x02_handler(modbus_package *query ,modbus_package *modbus_answer);
void func_0x03or0x04_handler(modbus_package *query, modbus_package *modbus_answer);
void func_0x05_handler(modbus_package *query, modbus_package *modbus_answer);
void func_0x06_handler(modbus_package *query, modbus_package *modbus_answer);
void func_0x0F_handler(modbus_package *query,modbus_package *modbus_answer);
void func_0x10_handler(modbus_package *query,modbus_package *modbus_answer);
void func_0x16_handler (modbus_package *query, modbus_package *modbus_answer);
void func_0x17_handler(modbus_package *query,modbus_package *modbus_answer);
void modbus_query_handler(modbus_package* query,modbus_package *modbus_answer);

#endif /* MODBUS_INCLUDE_SERVER_RESPONSEHANDLE_H_ */

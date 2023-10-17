/*
 * reguestHandler.h
 *
 *  Created on: 11 сент. 2023 г.
 *      Author: vparx
 */

#ifndef INCLUDE_MODBUS_REQUESTHANDLER_H_
#define INCLUDE_MODBUS_REQUESTHANDLER_H_

#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "modbusFunc.h"
int32_t request_0x01or0x02_handler(modbus_package *query,modbus_package *modbus_answer,uint16_t start_address,uint16_t count,struct netconn *conn);
int32_t request_0x03or0x04_handler(modbus_package *query, int16_t counter, int16_t address,modbus_package *modbus_answer,uint16_t start_address,uint16_t count,struct netconn *conn);
int32_t request_0x05_handler(modbus_package *query, int16_t address,uint8_t value,modbus_package *modbus_answer,struct netconn *conn);
int32_t request_0x06_handler(modbus_package *query, int16_t address, int16_t value, modbus_package *modbus_answer,struct netconn *conn);
int32_t request_0x10or0x0F_handler(modbus_package *query, int16_t address,int16_t count_byte,uint8_t count,uint8_t *values,modbus_package *modbus_answer,struct netconn *conn);
int32_t request_0x16_handler (modbus_package *query, modbus_package *modbus_answer,uint16_t address,uint16_t and_mask,uint16_t or_mask,struct netconn *conn);
int32_t request_0x17_handler(modbus_package *query,modbus_package *modbus_answer, uint16_t read_starting_address,uint16_t read_count,uint16_t write_starting_address,uint16_t write_count,uint8_t write_byte_count,uint16_t *values,struct netconn *conn);

void form_MBAP(uint16_t tid,uint16_t pid,uint8_t uid,uint8_t func,modbus_package *query);

bool is_exception(modbus_package *answer,uint8_t func);

void handle_exception(modbus_package *answer);




#endif /* INCLUDE_MODBUS_REQUESTHANDLER_H_ */

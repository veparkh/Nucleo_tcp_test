#ifndef MODBUS_INCLUDE_MODBUSFUNC_H_
#define MODBUS_INCLUDE_MODBUSFUNC_H_

#include "ch.h"
#include "hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "dataStructures.h"

bool read_discrete_input(int16_t address, uint8_t *val);

bool read_coils(int16_t address,uint8_t *val);
bool write_coils(int16_t address,uint8_t val);


bool read_inputs(int16_t address,int16_t *val);

bool write_holding_registers(int16_t address,int16_t val);
bool read_holding_registers(int16_t address,int16_t *val);



void setError(bool is_read_err, bool is_write_err,bool is_mb_exception, int8_t err_code);

mb_error getError(void);
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




void form_MBAP(uint16_t tid,uint16_t pid, uint8_t uid,uint8_t func,modbus_package *query);

void fill_tables(void);
int16_t modbustcp_get_address(modbus_package* data);
int16_t modbustcp_get_count(modbus_package* data);
uint8_t modbustcp_get_boll_value(modbus_package* data);
int16_t modbustcp_get_value(modbus_package* data);
int8_t modbustcp_get_count_byte(modbus_package* data);
int16_t mb_get_multiple_analog_register(modbus_package* data,uint8_t count);
int8_t get_coil_value(uint8_t *data, uint16_t i, uint8_t shift);
void get_exception(uint8_t func, uint8_t err_code, modbus_package *modbus_answer);
bool is_modbus_package(modbus_package *query,int16_t buflen);

#endif

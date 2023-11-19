#ifndef MODBUS_INCLUDE_MODBUSFUNC_H_
#define MODBUS_INCLUDE_MODBUSFUNC_H_

#include "ch.h"
#include "hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "dataStructures.h"

#define MY_ID							1
bool read_discrete_input(int16_t address, uint8_t *val);

bool read_coils(int16_t address,uint8_t *val);
bool write_coils(int16_t address,uint8_t val);


bool read_inputs(int16_t address,int16_t *val);

bool write_holding_registers(int16_t address,int16_t val);
bool read_holding_registers(int16_t address,int16_t *val);


void setError(bool is_read_err, bool is_write_err,bool is_mb_exception, int8_t err_code);
mb_error getError(void);

void form_MBAP(uint16_t tid,uint16_t pid, uint8_t uid,uint8_t func,modbus_package *query);

void fill_tables(void);

int8_t get_coil_value(uint8_t *data, uint16_t i, uint8_t shift);
void get_exception(uint8_t func, uint8_t err_code, modbus_package *modbus_answer);
bool is_modbus_package(modbus_package *query,int16_t buflen);

#endif

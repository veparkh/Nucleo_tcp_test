#ifndef MODBUS_INCLUDE_DATASTRUCTURES_H_
#define MODBUS_INCLUDE_DATASTRUCTURES_H_

#include "modbusFunc.h"

typedef struct modbus_package{
	uint16_t tid;
	uint16_t pid;
	uint16_t length;
	uint8_t uid;
	uint8_t func;
	uint8_t data[REGISTER_LEN*2+10];
}modbus_package;


typedef struct resp_0x01_0x02{
	uint8_t count;
	uint8_t bytes[REGISTER_LEN/8+1];
} resp_0x01_0x02;

typedef struct resp_0x03_0x04{
	uint8_t count;
	int16_t bytes[REGISTER_LEN];
} resp_0x03_0x04;

typedef struct resp_0x05_0x06{
	uint16_t count;
	int16_t byte;
} resp_0x05_0x06;

typedef struct resp_0x0F_ox10{
	uint16_t address;
	uint16_t count;
} resp_0x0F_0x10;

typedef struct resp_0x16{
	uint16_t address;
	uint16_t and_mask;
	uint16_t or_mask;
} resp_0x16;

typedef struct resp_0x17{

	uint8_t count;
	int16_t bytes[REGISTER_LEN];
} resp_0x17;

///////////////////////////////////////////////////////////////////////////////////////////

typedef struct req_0x01_0x02{
	uint16_t address;
	uint16_t count;
} req_0x01_0x02;

typedef struct req_0x03_0x04{
	uint16_t address;
	uint16_t count;
} req_0x03_0x04;

typedef struct req_0x05_0x06{
	uint16_t address;
	uint16_t value;
} req_0x05_0x06;

typedef struct req_0x0F{
	uint16_t address;
	uint16_t quantity;
	uint8_t count;
	uint8_t bytes[REGISTER_LEN/8+1];
} req_0x0F;

typedef struct req_0x10{
	uint16_t address;
	uint16_t quantity;
	uint8_t count;
	uint16_t bytes[REGISTER_LEN];
} req_0x10;

typedef struct req_0x16{
	uint16_t address;
	uint16_t and_mask;
	uint16_t or_mask;
} req_0x16;

typedef struct req_0x17{
	uint16_t read_address;
	uint16_t read_quantity;
	uint16_t write_address;
	uint16_t write_quantity;
	uint8_t write_count;
	int16_t bytes[REGISTER_LEN];
} req_0x17;




/*void fill_resp_0x01_0x02(resp_0x01_0x02 *resp, uint8_t count, uint8_t *bytes,);
void fill_resp_0x03_0x04(resp_0x03_0x04 *resp, uint8_t count, uint8_t *bytes);
void fill_resp_0x05_0x06(resp_0x05_0x06 *resp, uint8_t count, uint8_t *bytes);
void fill_resp_0x0F_0x10(resp_0x0F_0x10 *resp, uint8_t count, uint8_t *bytes);
void fill_resp_0x16(resp_0x16 *resp, uint8_t count, uint8_t *bytes);
void fill_resp_0x17(resp_0x17 *resp, uint8_t count, uint8_t *bytes);*/

void fill_req_0x01_0x02(req_0x01_0x02 *req, uint16_t address, uint16_t count);
void fill_req_0x03_0x04(req_0x03_0x04 *req, uint16_t address, uint16_t count);
void fill_req_0x05_0x06(req_0x05_0x06 *req, uint16_t address, uint16_t value );
void fill_req_0x0F(req_0x0F *req, uint16_t address,uint16_t quantity, uint8_t count, uint8_t *bytes);
void fill_req_0x10(req_0x10 *req, uint16_t address, uint16_t quantity,uint8_t count, uint16_t *bytes);
void fill_req_0x16(req_0x16 *req, uint16_t address, uint16_t and_mask, uint16_t or_mask);
void fill_req_0x17(req_0x17 *req, uint16_t read_address, uint16_t read_quantity, uint16_t write_address, uint16_t write_quantity,uin8_t write_count, uint16_t *write_bytes);


void resp_change_endian(modbus_package *package);
void req_change_endian(modbus_package *package);

#endif /* MODBUS_INCLUDE_DATASTRUCTURES_H_ */

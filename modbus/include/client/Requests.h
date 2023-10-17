
#ifndef MODBUS_INCLUDE_CLIENT_REQUESTS_H_
#define MODBUS_INCLUDE_CLIENT_REQUESTS_H_

#define TIMEOUT 3000
resp_0x01_0x02 request_0x01_0x02(modbus_package *query,uint16_t start_address,uint16_t count,struct netconn *conn);
resp_0x03_0x04 request_0x03_0x04(modbus_package *query,uint16_t start_address,uint16_t count,struct netconn *conn);
resp_0x05_0x06 request_0x05_0x06(modbus_package *query,uint16_t start_address,uint16_t count,struct netconn *conn);
resp_0x0F_0x10 request_0x0F(modbus_package *query,int16_t address,int16_t count,uint8_t byte_count,uint8_t *values, struct netconn *conn);
resp_0x0F_0x10 request_0x10(modbus_package *query,int16_t address,int16_t count,uint8_t byte_count,uint16_t *values,struct netconn *conn);
resp_0x16 request_0x16(modbus_package *query,uint16_t address,uint16_t and_mask,uint16_t or_mask, struct netconn *conn);
resp_0x17 request_0x17(modbus_package *query,uint16_t read_starting_address,uint16_t read_count,uint16_t write_starting_address,uint16_t write_count,uint8_t write_byte_count,uint16_t *values,struct netconn *conn);

bool is_exception(modbus_package *answer,uint8_t func);
void handle_exception(modbus_package *answer);
int32_t recv_err_or_buflen read_modbus(struct netconn *conn,int timeout,modbus_package **answer);

#endif /* MODBUS_INCLUDE_CLIENT_REQUESTS_H_ */

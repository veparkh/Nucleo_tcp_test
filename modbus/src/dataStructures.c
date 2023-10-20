#include "dataStructures.h"


/*
void fill_resp_0x01_0x02(resp_0x01_0x02 *resp, uint8_t count, uint8_t *bytes){
	resp->count = count;

}
void fill_resp_0x03_0x04(resp_0x03_0x04 *resp, uint8_t count, uint8_t *bytes){

}
void fill_resp_0x05_0x06(resp_0x05_0x06 *resp, uint8_t count, uint8_t *bytes){

}
void fill_resp_0x0F_0x10(resp_0x0F_0x10 *resp, uint8_t count, uint8_t *bytes){

}
void fill_resp_0x16(resp_0x16 *resp, uint8_t count, uint8_t *bytes){

}
void fill_resp_0x17(resp_0x17 *resp, uint8_t count, uint8_t *bytes){

}*/


uint16_t fill_req_0x01_0x02(req_0x01_0x02 *req, uint16_t address, uint16_t count){
	req->address = address;
	req->count = count;
	return 4;
}
uint16_t fill_req_0x03_0x04(req_0x03_0x04 *req, uint16_t address, uint16_t count){
	req->address = address;
	req->count = count;
	return 4;
}
uint16_t fill_req_0x05_0x06(req_0x05_0x06 *req, uint16_t address, uint16_t value ){
	req->address = address;
	if(value)
		req->value = 0xFF00;
	else
		req->value = 0x00;
	return 4;
}
uint16_t fill_req_0x0F(req_0x0F *req, uint16_t address,uint16_t quantity, uint8_t bytes_count, uint8_t *bytes){
	req->address = address;
	req->quantity = quantity;
	req->count = bytes_count;
	for(uint16_t i=0;i<quantity;i++){
			if(bytes[i])
				req->bytes[i/8]|= (1<<i%8);
			else
				req->bytes[i/8]&= ~(1<<i%8);
		}
	return 5+bytes_count;
}
uint16_t fill_req_0x10(req_0x10 *req, uint16_t address, uint16_t quantity,uint8_t bytes_count, uint16_t *bytes ){
	req->address = address;
	req->quantity = quantity;
	req->count = bytes_count;
	for(uint16_t i=0;i<quantity;i++){
		req->bytes[i] = bytes[i];
	}
	return 5+bytes_count;


}
uint16_t fill_req_0x16(req_0x16 *req, uint16_t address, uint16_t and_mask, uint16_t or_mask){

	req->address = address;
	req->or_mask = or_mask;
	req->and_mask = and_mask;
	return 6;

}
uint16_t fill_req_0x17(req_0x17 *req, uint16_t read_address, uint16_t read_quantity, uint16_t write_address, uint16_t write_quantity,uint8_t write_count, uint16_t *write_bytes){

	req->read_address =read_address;
	req->read_quantity=read_quantity;
	req->write_address =write_address;
	req->write_quantity=write_quantity;
	req->write_count=write_count;
	for(uint16_t i=0;i<write_quantity;i++){
		req->bytes[i] = write_bytes[i];
	}
	return 9+write_count;
}

void change_2bytes_endian(uint16_t *value){
	uint8_t *ptr = (uint8_t*)value;
	uint8_t  HIGH = *ptr;
	*ptr = *(ptr+1);
	*(ptr+1) = HIGH;
}

void resp_change_endian(modbus_package *resp){
	resp->tid = resp->tid>>8 | resp->tid<<8;
	resp->length = resp->length>>8 | resp->length<<8;
	resp->pid = resp->pid>>8 | resp->pid<<8;
	switch(resp->func){
		case 0x03:
		case 0x04:{
			resp_0x03_0x04 *data;
			data = (resp_0x03_0x04*)resp->data;
			for(uint16_t i =0;i<data->count/2;i++){
				change_2bytes_endian(&data->bytes[i]);
			}
			break;
		}
		case 0x05:
		case 0x06:{
			resp_0x05_0x06 *data;
			data=(resp_0x05_0x06*)resp->data;
			change_2bytes_endian(&data->count);
			change_2bytes_endian(&data->byte);
			break;
		}
		case 0x0F:
		case 0x10:{
			resp_0x0F_0x10 *data;
			data=(resp_0x0F_0x10*)resp->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->count);
			break;
		}
		case 0x16:{
			resp_0x16 *data;
			data=(resp_0x16*)resp->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->and_mask);
			change_2bytes_endian(&data->or_mask);
			break;
		}
		case 0x17:{
			resp_0x17 *data;
			data=(resp_0x17*)resp->data;
			for(uint16_t i=0;i<data->count/2;i++){
				change_2bytes_endian(&data->bytes[i]);
			}
			break;
		}
	}
}

req_change_endian(modbus_package *req){
	req->tid = req->tid>>8 | req->tid<<8;
	req->length = req->length>>8 | req->length<<8;
	req->pid = req->pid>>8 | req->pid<<8;
	switch(req->func){
		case 0x03:
		case 0x04:{
			req_0x03_0x04 *data;
			data = (req_0x03_0x04*)req->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->count);
			break;
		}
		case 0x05:
		case 0x06:{
			req_0x05_0x06 *data;
			data=(req_0x05_0x06*)req->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->value);
			break;
		}
		case 0x0F:{
			req_0x0F *data;
			data=(req_0x0F*)req->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->quantity);
			break;
		}
		case 0x10:{
			req_0x10 *data;
			data=(req_0x10*)req->data;
			for(uint16_t i=0;i<data->quantity;i++){
				change_2bytes_endian(&data->bytes[i]);
			}
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->quantity);
			break;
		}
		case 0x16:{
			req_0x16 *data;
			data=(req_0x16*)req->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->and_mask);
			change_2bytes_endian(&data->or_mask);
			break;
		}
		case 0x17:{
			req_0x17 *data;
			data=(req_0x17*)req->data;
			change_2bytes_endian(&data->read_address);
			change_2bytes_endian(&data->read_quantity);
			change_2bytes_endian(&data->write_address);
			change_2bytes_endian(&data->write_quantity);
			change_2bytes_endian(&data->read_address);
			for(uint16_t i=0;i<data->write_quantity;i++){
				change_2bytes_endian(&data->bytes[i]);
			}
			break;
		}
	}
}

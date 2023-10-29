#include "dataStructures.h"
#include "serial.h"


uint16_t fill_resp_0x01_0x02(resp_0x01_0x02 *resp, uint16_t address, uint16_t count, bool (*read_bit) (int16_t address, uint8_t *val)){

	uint16_t len_data, i =0,  data=0, j=0;
	uint8_t val;
	if(count % 8)
		len_data = count/8+1;
	else
		len_data = count/8;

	for(int k=0;k<len_data;k++)
		{
		  i=0;
		  while((count>0) && (i<8))
		  {
			if(!read_bit(address+j, &val)){
				return 0;
			}
			data=data+(val<<i);
			i++;
			count--;
			j++;
		  }
		  dbgprintf("data:%d",data);
		  resp->bytes[k]=data;
		  data = 0;
		}
	resp->count = len_data;
	return 2+1+len_data;
}
uint16_t fill_resp_0x03_0x04(resp_0x03_0x04 *resp, uint16_t address, uint16_t count, bool (*read_byte) (int16_t address, int16_t *val)){

	int16_t value;

	  for(int i=0;i<count;i++)
	  {
		if(!read_byte(address+i,&value)){
			return 0 ;
		}
		resp->bytes[i] = value;
	  }

	resp->count = 2*count;
	return 2+1+2*count;
}
uint16_t fill_resp_0x05_0x06(resp_0x05_0x06 *resp, uint16_t address, uint16_t value){

	resp->address = address;
	resp->byte = value;
	return 2+4;
}
uint16_t fill_resp_0x0F_0x10(resp_0x0F_0x10 *resp, uint16_t address, uint16_t count ){
	resp->address = address;
	resp->count = count;
	return 2+4;
}
uint16_t fill_resp_0x16(resp_0x16 *resp, uint16_t address, uint16_t and_mask,uint16_t or_mask){
	resp->address = address;
	resp->and_mask = and_mask;
	resp->or_mask = or_mask;
	return 2+6;
}
/*uint16_t fill_resp_0x17(resp_0x17 *resp ) {

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
    uint16_t val=*value;
	*value = val>>8|val<<8;
}


void resp_change_endian(modbus_package *answer){
	answer->tid = answer->tid>>8 | answer->tid<<8;
	answer->length = answer->length>>8 | answer->length<<8;
	answer->pid = answer->pid>>8 | answer->pid<<8;
	switch(answer->func){
		case 0x03:
		case 0x04:{
			resp_0x03_0x04 *resp;
			resp = (resp_0x03_0x04*)answer->data;
			for(uint16_t i =0;i<resp->count/2;i++){
				//change_2bytes_endian((uint16_t*)&data->bytes[i]);
				resp->bytes[i]=resp->bytes[i]>>8|resp->bytes[i]<<8;
			}
			break;
		}
		case 0x05:
		case 0x06:{
			resp_0x05_0x06 *data;
			data=(resp_0x05_0x06*)answer->data;
			change_2bytes_endian((uint16_t*)&data->byte);
			break;
		}
		case 0x0F:
		case 0x10:{
			resp_0x0F_0x10 *data;
			data=(resp_0x0F_0x10*)answer->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->count);
			break;
		}
		case 0x16:{
			resp_0x16 *data;
			data=(resp_0x16*)answer->data;
			change_2bytes_endian(&data->address);
			change_2bytes_endian(&data->and_mask);
			change_2bytes_endian(&data->or_mask);
			break;
		}
		case 0x17:{
			resp_0x17 *data;
			data=(resp_0x17*)answer->data;
			for(uint16_t i=0;i<data->count/2;i++){
				change_2bytes_endian((uint16_t*)&data->bytes[i]);
			}
			break;
		}
	}
}

void req_change_endian(modbus_package *req){
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
				change_2bytes_endian((uint16_t*)&data->bytes[i]);
			}
			break;
		}
	}
}

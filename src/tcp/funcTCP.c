
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <stdint.h>
#include <stdio.h>
#include "modbusGet.h"
#include "funcTCP.h"

extern  bool isConnection;

int32_t read_data(struct netconn *conn,int timeout,modbus_package **query){

    struct netbuf *inbuf = NULL;
    uint16_t buflen;
    err_t recv_error;
    netconn_set_recvtimeout(conn,timeout);
    int32_t recv_error_or_buflen;
    recv_error = netconn_recv(conn, &inbuf);
    if (recv_error==0){
    	netbuf_data(inbuf, (void **)query, &buflen);
    	recv_error_or_buflen = buflen;
    }
    else{
    	recv_error_or_buflen = recv_error;
    }
    netbuf_delete(inbuf);
    return recv_error_or_buflen;
}

int write_log(struct netconn *conn, modbus_package *query){
	int recv_err_or_length;
	int tcp_code;
	while(isConnection){
		recv_err_or_length = read_data(conn, 1000, &query);
		if(recv_err_or_length==ERR_TIMEOUT){
			netconn_write(conn, "hi,i am log. I can't do something workful now, i just wanna say u got this\r\n",76, NETCONN_NOCOPY);
		}
		else if(recv_err_or_length > ERR_OK){
				if(sscanf((char*)query,"%d",&tcp_code)==1){
					dbgprintf("tcp_code log:%d", tcp_code);
					if(tcp_code==100){
						return ERR_OK;
					}
				}
		}else
		{
			return recv_err_or_length;
		}
	}
	return ERR_RTE;
}

int tcp_query_handler(modbus_package *query, struct netconn *conn){
	int tcp_code;
	if(sscanf((char*)query,"%d",&tcp_code)==1){
		switch(tcp_code){
			case 100:{
				return write_log(conn,query);
				break;
			}
		}
	}
	return ERR_VAL;
}


#ifndef INCLUDE_TCP_TCP_H_
#define INCLUDE_TCP_TCP_H_

int32_t read_data(struct netconn *conn,int timeout,modbus_package **query);

err_t write_data(struct netconn *conn,char *data);

void tcpInit(lwipthread_opts_t *opts,struct ip4_addr ip, struct ip4_addr gateway,struct ip4_addr netmask,uint8_t *mac_address,void (*down_callback) (void),void (*up_callback) (void));
#endif /* INCLUDE_TCP_TCP_H_ */

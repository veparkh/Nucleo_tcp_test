
#ifndef INCLUDE_TCP_TCP_H_
#define INCLUDE_TCP_TCP_H_

int32_t read_data(struct netconn *conn,int timeout,modbus_package **query);

int write_log(struct netconn *conn, modbus_package *query);

int tcp_query_handler(modbus_package *query, struct netconn *conn);

#endif /* INCLUDE_TCP_TCP_H_ */

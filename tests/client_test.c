
#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include "common.h"

char msg_c[200];
thread_t *tp;
err_t write_data_c(struct netconn *conn){
    // При записи просто отправляем указатель на буфер и количество байт. Последний аргумент - флаг для записи. Но я пока про них ничего не понимаю
	if(strlen(msg_c)!=0)
	{
		err_t err = netconn_write(conn, msg_c, strlen(msg_c), NETCONN_NOCOPY);
		strcpy(msg_c, "");
		return err;
	}
	return netconn_write(conn, "hi there!", strlen("hi there!"), NETCONN_NOCOPY);
}

void read_data_c(struct netconn *conn,char *arr){
    // При чтении немного сложнее
    struct netbuf *inbuf;
    uint8_t *buf;
    uint16_t buflen;

    // Ждем что нам напишут. Функция блокирующая. В inbuf лежит информация кто и что написал
	netconn_set_recvtimeout(conn,6000);
    err_t err_recv = netconn_recv(conn, &inbuf);
    dbgprintf("recv err:%d\n\r",err_recv);
   if (err_recv ==0)
   {
	   // Память мы сами не выделяем, все делает lwip. Просто подсовываем известное имя, чтобы оно начало указывать на начало данных. Заодно узнаем количество байт
	   netbuf_data(inbuf, (void **)&buf, &buflen);
	   // Выводим по юарту полученные данные. Я пользовался tcp терминалом и отправлял буквы, так что принтфом пользоваться не надо было
	   //sdWrite(&SD3, buf, buflen);
	   strncpy(msg_c, inbuf->p->payload ,buflen);
	   dbgprintf(arr);
   }
    // Очишаем память. Если этого не делать она очень быстро закончится
    netbuf_delete(inbuf);
}

THD_WORKING_AREA(wa_tcp_client, 1024);
THD_FUNCTION(tcp_client, p) {
  (void)p;
  chRegSetThreadName("client_thread");
  err_t err_connect;
// Поленился нормально прокинуть сюда адрес, поэтому определяю его заново. Он должен быть такй же как и в мейне
  struct ip4_addr server_ip;
  IP4_ADDR(&server_ip, 192, 168, 1, 120);

  while (true) {
	  if (chThdShouldTerminateX())
	  {
		  chThdExit (MSG_OK);
	  }
	chThdSleepMilliseconds(400);
    palToggleLine(LINE_LED3);
    struct netconn *conn = netconn_new(NETCONN_TCP);
    if(conn==NULL)
    {
    	continue;
    }
    err_connect = netconn_connect(conn,&server_ip,80);
    dbgprintf("connect err:%d\n\r",err_connect);
    // Если что-то не так - ничего не делаем
    if (err_connect != ERR_OK)
    {
      netconn_close(conn);
      netconn_delete(conn);
      continue;
    }
    chThdSleepMilliseconds(1000);
    // Индикация что кто-то подключился
    palToggleLine(LINE_LED1);
    // Напишем что-нибудь

    err_t err_write =   write_data_c(conn);
    dbgprintf("write err:%d\n\r",err_write);
    // Прочитаем что-нибудь
    read_data_c(conn,msg_c);
    // По окончанию работы закрываем соединение и удаляем подключение
    netconn_close(conn);
    netconn_delete(conn);
    // После этого готовы снова подключится
  }

}

void up_callback(void *p)
{
	(void)p;
	dbgprintf("cable in\r\n");
	tp = chThdCreateStatic(wa_tcp_client, 1024, NORMALPRIO, tcp_client, NULL);
}
void down_callback(void *p)
{
	(void)p;

	dbgprintf("cable out\r\n");
	chThdTerminate(tp);

}
void client_test(void){

    halInit();
    chSysInit();
    debug_stream_init();
    dbgprintf("start\r\n");
// Задаем адрес стмки
    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 192, 168, 1, 123);
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
// Макадресс вроде может быть абсолютно любой. Главное чтобы в сети не было одинаковых
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;
    opts.link_up_cb = up_callback;
    opts.link_down_cb = down_callback;


// Запускаем сетевой драйвер. С этого момента на разьеме начнется индикация и стм будет пинговаться, если другой конец шнура в той же сети
// Можем не передавать настройки, использовав NULL в качестве аргумента. Тогда будут использованы настройки по умолчанию из файла lwipthread.h
    lwipInit(&opts);
    chThdSleepSeconds(5);

// Запустим поток сервера
    //chThdCreateStatic(wa_tcp_client, 1024, NORMALPRIO, tcp_client, NULL);

// Помигаем лампочкой, чтобы понимать что не зависли
    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}

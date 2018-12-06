
 #define _sim800_h
 
#include "stm32f10x.h"

#define RX_BUF_MAX_LEN     100                                     //最大接收缓存字节数



typedef enum
{
	at,
	cpin,
	cgatt,
	cipmode,
	cstt,
	ciicr,
	cipstart
	
}sim800_status_type;
#define     sim_Usart( fmt, ... )           USART_printf ( macESP8266_USARTx, fmt, ##__VA_ARGS__ ) 
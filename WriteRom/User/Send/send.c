#include "send.h"
#include "esp8266.h"
#include "SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "dht11.h"
#include "usart1.h"
#include "ds18b20.h"
#include "mq.h"
#include "mod.h"
#include "rtc.h"
#include "string.h"
/************************************************
*数据通过wifi传输到服务器
************************************************/
extern usart_st  usart_data;
extern uint64_t datanum;
uint16_t address;
char senddata[210];	 
char rx_buf_str[255];  //串口数据字符串格式
char body_data_1[40];        //需发送数据
char body_data_2[40]; 
char body_data_3[40]; 
char body_data_len=0;
extern  struct Data_Time  timer;
volatile uint8_t ucTcpClosedFlag = 0;


 /*字节流转字符串
**参数：  source源数组
**     dest  目标字符串
**     sourceLen长度
*/
 void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
  
    for (i = 0; i < sourceLen; i++)  
    {  
        highByte = source[i] >> 4;  
        lowByte = source[i] & 0x0f ;  
  
        highByte += 0x30;  
  
        if (highByte > 0x39)  
                dest[i * 2] = highByte + 0x07;  
        else  
                dest[i * 2] = highByte;  
  
        lowByte += 0x30;  
        if (lowByte > 0x39)  
            dest[i * 2 + 1] = lowByte + 0x07;  
        else  
            dest[i * 2 + 1] = lowByte;  
    }  
    return ;  
}  




 //数据转换与截取
void convert()
{
	  //字节流转换
     ByteToHexStr(usart_data.rx_buf,rx_buf_str,usart_data.rx_len_ed); 
	  //计算需截取的长度
	  body_data_len=usart_data.rx_buf[2]*2;
	  //待发送数据截取  
	  strncpy(body_data_1,rx_buf_str+6,body_data_len);  
   //strncpy(body_data_2,rx_buf_str+26,20);  
	  //strncpy(body_data_3,rx_buf_str+46,20);
}

//数据内容打包
/* 函数名：packagedata()
** 描述：将需要发送的参数转成json
*/
void packagedata()
{
	  Time_Get(); 
	 	address=usart_data.tx_buf[3]+40001;
  	convert();
	 int m=0;
	  m++;
	sprintf(senddata,"{\r\ndata:\"%s\",\r\ntime:\"%d.%02d.%02d %02d:%02d:%02d\",\r\n}",body_data_1,timer.w_year,timer.w_month,timer.w_date,timer.hour,timer.min,timer.sec);
 //  sprintf(senddata,"{\r\n data_2:\"%s\}",body_data_2);
}
void clear()
{
memset(body_data_1,0,20);
	memset(body_data_2,0,20);
	memset(body_data_3,0,20);
}

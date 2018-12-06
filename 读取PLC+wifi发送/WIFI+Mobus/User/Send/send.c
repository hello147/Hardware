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

/************************************************
*数据通过wifi传输到服务器
************************************************/



volatile uint8_t ucTcpClosedFlag = 0;
uint16_t address;
	uint16_t data;
extern usart_st  usart_data;
extern modbus_rtust  modbus_data;
	 char senddata[20];
char body_data_len=0;
char rx_buf_str[50];  //串口数据字符串格式
char body_data[20];        //需发送数据
void esp8266_connection()
{
   
	macESP8266_CH_ENABLE();
	ESP8266_AT_Test ();
	ESP8266_Net_Mode_Choose ( STA );
  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
	ESP8266_Enable_MultipleId ( DISABLE );
	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
	while ( ! ESP8266_UnvarnishSend () );
}



/**
  * @brief  ESP8266 （Sta Tcp Client）透传
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	uint8_t ucStatus;
	
	char cStr [ 100 ] = { "123456" };

//	DHT11_Data_TypeDef DHT11_Data;
	

	macESP8266_CH_ENABLE();
	
	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA );

  while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );	
	
	ESP8266_Enable_MultipleId ( DISABLE );
	
	while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
	
	while ( ! ESP8266_UnvarnishSend () );
	
	while ( 1 )
	{		
		//温度传感器存在且一氧化碳浓度大于0
		//if(DS18B20_Check()&&Get_Adc_Convert()>=0)
		//	sprintf (cStr, "\r\n Success\r\n Temp:%04.1f℃ \r\n CO:%05.1fm3 \r\n",DS18B20_Get_Temp(),Get_Adc_Convert());
			/*拼接两个传感器数据*/
		//	sprintf (cStr, "%s%s",Temp_convert(),CO_convert());
			//strcat(strcpy(cStr,Temp_convert()),CO_convert());
			 //strcat(cStr,Temp_convert());
			 //strcat(cStr,CO_convert());
		//else
			//printf(cStr,"Error\r\n");
		printf ( "%s", cStr );
		ESP8266_SendString ( ENABLE, cStr, 0, Single_ID_0 );               //发送信息到服务器
		delay_ms ( 1000 );
		delay_ms ( 1000 );
		delay_ms ( 1000 );
		
		if ( ucTcpClosedFlag )                                             //检测是否失去连接
		{
			ESP8266_ExitUnvarnishSend ();                                    //退出透传模式
			
			do ucStatus = ESP8266_Get_LinkStatus ();                         //获取连接状态
			while ( ! ucStatus );
			
			if ( ucStatus == 4 )                                             //确认失去连接后重连
			{
			
				
				while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
				
				while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
				
			}
			
			while ( ! ESP8266_UnvarnishSend () );		
			
		}

	}
	
		
}

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
	  strncpy(body_data,rx_buf_str+6,body_data_len);      
}

void packagedata()
{
  
	  convert();
		address=usart_data.tx_buf[3]+40001;
	 //data=(usart_data.rx_buf[3]+usart_data.rx_buf[4]);
	sprintf(senddata,"{\r\n address:\"%u\",\
		\r\n data:\"%s\"\r\n}",address,body_data);
	
}




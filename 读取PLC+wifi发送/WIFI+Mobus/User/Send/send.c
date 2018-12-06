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
*����ͨ��wifi���䵽������
************************************************/



volatile uint8_t ucTcpClosedFlag = 0;
uint16_t address;
	uint16_t data;
extern usart_st  usart_data;
extern modbus_rtust  modbus_data;
	 char senddata[20];
char body_data_len=0;
char rx_buf_str[50];  //���������ַ�����ʽ
char body_data[20];        //�跢������
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
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
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
		//�¶ȴ�����������һ����̼Ũ�ȴ���0
		//if(DS18B20_Check()&&Get_Adc_Convert()>=0)
		//	sprintf (cStr, "\r\n Success\r\n Temp:%04.1f�� \r\n CO:%05.1fm3 \r\n",DS18B20_Get_Temp(),Get_Adc_Convert());
			/*ƴ����������������*/
		//	sprintf (cStr, "%s%s",Temp_convert(),CO_convert());
			//strcat(strcpy(cStr,Temp_convert()),CO_convert());
			 //strcat(cStr,Temp_convert());
			 //strcat(cStr,CO_convert());
		//else
			//printf(cStr,"Error\r\n");
		printf ( "%s", cStr );
		ESP8266_SendString ( ENABLE, cStr, 0, Single_ID_0 );               //������Ϣ��������
		delay_ms ( 1000 );
		delay_ms ( 1000 );
		delay_ms ( 1000 );
		
		if ( ucTcpClosedFlag )                                             //����Ƿ�ʧȥ����
		{
			ESP8266_ExitUnvarnishSend ();                                    //�˳�͸��ģʽ
			
			do ucStatus = ESP8266_Get_LinkStatus ();                         //��ȡ����״̬
			while ( ! ucStatus );
			
			if ( ucStatus == 4 )                                             //ȷ��ʧȥ���Ӻ�����
			{
			
				
				while ( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
				
				while ( !	ESP8266_Link_Server ( enumTCP, macUser_ESP8266_TcpServer_IP, macUser_ESP8266_TcpServer_Port, Single_ID_0 ) );
				
			}
			
			while ( ! ESP8266_UnvarnishSend () );		
			
		}

	}
	
		
}

 /*�ֽ���ת�ַ���
**������  sourceԴ����
**     dest  Ŀ���ַ���
**     sourceLen����
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


 //����ת�����ȡ
void convert()
{
	  //�ֽ���ת��
     ByteToHexStr(usart_data.rx_buf,rx_buf_str,usart_data.rx_len_ed); 
	  //�������ȡ�ĳ���
	   body_data_len=usart_data.rx_buf[2]*2;
	  //���������ݽ�ȡ  
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




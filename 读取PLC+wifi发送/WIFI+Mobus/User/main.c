/**
  ******************************************************************************
  *主函数-传输数据
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "usart2.h"
#include "SysTick.h"
#include "esp8266.h"
#include "Send.h"
#include "dht11.h"
#include "mq.h"
#include "mod.h"
 #include "delay.h"
extern usart_st  usart_data;
extern modbus_rtust  modbus_data;
 int sec=0;
unsigned  short int  addr_arry[]={40001,40003,40005,40006,40007,40008,40010,40012,
	40016,40020,40024,40028,40032,40040,40042,40044,40046,40048,40050,40052,40054,
  40058,40060,40062,40064,40066,40068,40070,40074,40078,40100};
	unsigned short int addr_read;
	int m=0;
	extern char senddata[20];
int main ( void )
{
	uint8_t ucStatus;
	//delay_init();
  USARTx_Config();                       //初始化串口1
		SysTick_Config(SystemCoreClock / 1000); 
	
	ESP8266_Init();                      //初始化WiFi模块使用的接口和外设
	//printf ( "\r\nFire monitoring and alarm system\r\n" );        //打印测试例程提示信息
 Mod_USARTx_Config();
  //ESP8266_StaTcpClient_UnvarnishTest ();
	esp8266_connection();

	
		for(;;)   
		 {
				 if(sec>=60000)
				{ 
						addr_read=addr_arry[m]-40001;
						read_hold_register_03(1,0,3);
							sec=0;  
				} 
				
				//timcount=0;
				if(modbus_rev_deal()==1)//回复的数据帧正确
					{       
					   	packagedata();//拼接需要发送的字符串
						  ESP8266_SendString ( ENABLE, senddata,0,5);     
					   	claer(); //清空接收，发送区。长度	
					}
					
		}
  
	
}


/*********************************************END OF FILE**********************/

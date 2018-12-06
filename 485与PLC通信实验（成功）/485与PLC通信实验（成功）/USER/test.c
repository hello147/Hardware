#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "beep.h"	 	 
#include "key.h"	 	 
#include "exti.h"	 	 
#include "wdg.h" 	 
#include "timer.h"		 	 
#include "tpad.h"
#include "oled.h"		 	 
#include "lcd.h"
#include "usmart.h"	
#include "rtc.h"	 	 
#include "wkup.h"	
#include "adc.h" 	 
#include "dac.h" 	 
#include "dma.h" 	 
#include "24cxx.h" 	 
#include "flash.h" 	 
#include "rs485.h" 	 
 
 				 	
int main(void)
{		
	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 buf[]={0x02,0x64,0x0d};
	
	u8 rs485buf[]={0x64,0x0d}; 
 	Stm32_Clock_Init(9);	//系统时钟设置
	uart_init(72,9600);	 	//串口初始化为9600
	delay_init(72);	   	 	//延时初始化 
	LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD
	usmart_dev.init(72);	//初始化USMART		
	KEY_Init();				//按键初始化		 	
	RS485_Init(36,9600);	//初始化RS485

 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"RS485 TEST");	
	LCD_ShowString(60,90,200,16,16,"WarShip--PLC");
	LCD_ShowString(60,110,200,16,16,"2014/09/01");	
	LCD_ShowString(60,130,200,16,16,"KEY0:Send");	//显示提示信息		
 
 	
 	POINT_COLOR=BLUE;//设置字体为蓝色	  
	LCD_ShowString(60,150,200,16,16,"Count:");			//显示当前计数值	
	LCD_ShowString(60,170,200,16,16,"Send Data:");		//提示发送的数据	
	LCD_ShowString(60,210,200,16,16,"Receive Data:");	//提示接收到的数据		
 									  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_RIGHT)//KEY0按下,发送一次数据
		{  delay_ms(10);
			if(key==KEY_RIGHT)
			{  RS485_TX_EN=1;
			  for(i=0;i<2;i++)
			   {
				
				  LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0X80);	//显示数据
 		    	RS485_Send_Data(rs485buf,2);//发送5个字节 	
         }
		 		RS485_TX_EN=0;							   
		 }	
    }	 
		if(key==KEY_LEFT)//KEY0按下,发送一次数据
		{  delay_ms(10);
			if(key==KEY_LEFT)
			{  RS485_TX_EN=1;
			  for(i=0;i<2;i++)
			   {
				
				  LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0X80);	//显示数据
 		    	RS485_Send_Data(buf,3);//发送5个字节 	
         }
 		    	RS485_TX_EN=0;
         LED1=!LED1;
		 									   
		 }	
    }	 

		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;//提示系统正在运行	
			t=0;
			cnt++;  /* 用于随机产生数据，“rs485buf[i]=cnt+i” */
			LCD_ShowxNum(60+48,150,cnt,3,16,0X80);	//显示数据
		}		   
	}
}















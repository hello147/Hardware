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
 	Stm32_Clock_Init(9);	//ϵͳʱ������
	uart_init(72,9600);	 	//���ڳ�ʼ��Ϊ9600
	delay_init(72);	   	 	//��ʱ��ʼ�� 
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   	//��ʼ��LCD
	usmart_dev.init(72);	//��ʼ��USMART		
	KEY_Init();				//������ʼ��		 	
	RS485_Init(36,9600);	//��ʼ��RS485

 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"RS485 TEST");	
	LCD_ShowString(60,90,200,16,16,"WarShip--PLC");
	LCD_ShowString(60,110,200,16,16,"2014/09/01");	
	LCD_ShowString(60,130,200,16,16,"KEY0:Send");	//��ʾ��ʾ��Ϣ		
 
 	
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	LCD_ShowString(60,150,200,16,16,"Count:");			//��ʾ��ǰ����ֵ	
	LCD_ShowString(60,170,200,16,16,"Send Data:");		//��ʾ���͵�����	
	LCD_ShowString(60,210,200,16,16,"Receive Data:");	//��ʾ���յ�������		
 									  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_RIGHT)//KEY0����,����һ������
		{  delay_ms(10);
			if(key==KEY_RIGHT)
			{  RS485_TX_EN=1;
			  for(i=0;i<2;i++)
			   {
				
				  LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0X80);	//��ʾ����
 		    	RS485_Send_Data(rs485buf,2);//����5���ֽ� 	
         }
		 		RS485_TX_EN=0;							   
		 }	
    }	 
		if(key==KEY_LEFT)//KEY0����,����һ������
		{  delay_ms(10);
			if(key==KEY_LEFT)
			{  RS485_TX_EN=1;
			  for(i=0;i<2;i++)
			   {
				
				  LCD_ShowxNum(30+i*32,190,rs485buf[i],3,16,0X80);	//��ʾ����
 		    	RS485_Send_Data(buf,3);//����5���ֽ� 	
         }
 		    	RS485_TX_EN=0;
         LED1=!LED1;
		 									   
		 }	
    }	 

		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0=!LED0;//��ʾϵͳ��������	
			t=0;
			cnt++;  /* ��������������ݣ���rs485buf[i]=cnt+i�� */
			LCD_ShowxNum(60+48,150,cnt,3,16,0X80);	//��ʾ����
		}		   
	}
}















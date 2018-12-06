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
#include "tim.h"
#include "mod.h"
#include "delay.h"

extern usart_st  usart_data;
extern modbus_rtust  modbus_data;

extern u16 tim3_count;
extern u16 tim3_espcount;
extern u8 sim800_status;
unsigned int sec;
 uint8_t check;
unsigned  short int  addr_arry[]={40001,40003,40005,40006,40007,40008,40010,40012,
	40016,40020,40024,40028,40032,40040,40042,40044,40046,40048,40050,40052,40054,
  40058,40060,40062,40064,40066,40068,40070,40074,40078,40100};
	unsigned short int addr_read;
	int m=0;
	extern char senddata[210];
	 char newtime[26];

	 int readflag;
	 int sendflag;
	 int timeflag;
int main ( void )
{
	uint8_t ucStatus;

	SysTick_Config(SystemCoreClock / 1000); 
  Mod_USARTx_Config();//使用串口1

	NVIC3_Configuration();
		AT24CXX_Init();			//IIC初始化,使用串口3 
	//Init_TIMER();
	Init_tim3();
	Init_RTC();
	Time_Update(2018,11,28,0,0,0);
	 
		while(1)   
			 {
			     if(readflag==1)
					{ 
							 read_hold_register_03(1,0,40);	
							 tim3_count=0;
					     readflag=0;	
					} 
				  if(sendflag==1)
					{
						 //TIM_Cmd(TIM2, DISABLE); 
						if(modbus_rev_deal()==1)
						{		
						      packagedata();//拼接需要发送的字符串(json)
							   	AT24CXX_Write(0,(u8*)senddata,sizeof(senddata));
							     delay_ms(1000); 
									AT24CXX_Read(210,newtime,26);
                  memset(senddata,0,sizeof(senddata));
               
						}	
             sendflag=0;		
             tim3_espcount=0;						
					}
					if(timeflag==1)
					{
					  AT24CXX_Read(210,newtime,26);
						Time_Set();
						timeflag=0;
					}
				
							
   			}
	}
												
	   	 
	
	



/*********************************************END OF FILE**********************/

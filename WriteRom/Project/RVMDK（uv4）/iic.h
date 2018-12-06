#ifndef _IIC_H_
#define _IIC_H_
#include "delay.h"
#include "STM32F10X.h"

#include "common.h"

   	   		   
//IO
#define SDA_IN() 	GPIO_SetDir(0, 1<<10, 0)
#define SDA_OUT() 	GPIO_SetDir(0, 1<<10, 1) 

//IO 	 
#define SCL_H             GPIO_SetBits();
#define SCL_L             GP0(11)&=0 //SCL
#define SDA_H             GP0(10)|=1 //SDA
#define SDA_L             GP0(10)&=0 //SDA
#define IIC_WP            GP1(29)
#define READ_SDA   	 ((LPC_GPIO0->FIOPIN>>10)&0x01)

//IIC 		
void 	IIC_Init(void);
void 	IIC_Start(void);			
void 	IIC_Stop(void);	  			
u8 	IIC_Send_Byte(u8 txd);		
u8 	IIC_Read_Byte(u8 ack);//
	  
#endif
















  
				             

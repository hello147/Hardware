#ifndef __SYSTICK_H
#define __SYSTICK_H



#include "stm32f10x.h"



//#define delay_ms(x) delay_us(1000*x)	 //µ¥Î»ms



void SysTick_Init( void );
void TimingDelay_Decrement( void );
void delay_ms( __IO u32 nTime );


#endif /* __SYSTICK_H */

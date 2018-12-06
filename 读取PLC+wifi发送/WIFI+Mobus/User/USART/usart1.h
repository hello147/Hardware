#ifndef __USART1_H
#define	__USART1_H


#include "stm32f10x.h"
#include <stdio.h>



/**************************USART参数定义********************************/
#define             macUSART_BAUD_RATE                       115200

#define             macUSARTx                                USART2
#define             macUSART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             macUSART_CLK                             RCC_APB1Periph_USART2
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        RCC_APB2Periph_GPIOA     
#define             macUSART_TX_PORT                         GPIOA   
#define             macUSART_TX_PIN                          GPIO_Pin_2
#define             macUSART_RX_PORT                         GPIOA 
#define             macUSART_RX_PIN                          GPIO_Pin_3
#define             macUSART_IRQ                             USART2_IRQn
#define             macUSART_INT_FUN                         USART2_IRQHandler



void                USARTx_Config                           ( void );



#endif /* __USART1_H */



#include "stm32f10x.h"
#include <stdio.h>

/**************************USART参数定义********************************/
#define             simUSART_BAUD_RATE                       115200

#define             simUSARTx                                USART2
#define             simUSART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             simUSART_CLK                             RCC_APB1Periph_USART2
#define             simUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             simUSART_GPIO_CLK                        RCC_APB2Periph_GPIOA     
#define             simUSART_TX_PORT                         GPIOA   
#define             simUSART_TX_PIN                          GPIO_Pin_2
#define             simUSART_RX_PORT                         GPIOA 
#define             simUSART_RX_PIN                          GPIO_Pin_3
#define             simUSART_IRQ                             USART2_IRQn
#define             simUSART_INT_FUN                         USART2_IRQHandler
void sim_USART_NVIC_Configuration ();


void                USARTx_Config                           ( void );





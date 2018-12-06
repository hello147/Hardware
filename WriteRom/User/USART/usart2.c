/**
  ******************************************************************************
  *USART1发送函数
  ******************************************************************************
  */ 
  
#include "usart2.h"
#include "common.h"
extern u8 uart_recv[40];
 /**
  * @brief  USARTx GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* 使能时钟配置 */
	simUSART_APBxClock_FUN ( simUSART_CLK, ENABLE );
	simUSART_GPIO_APBxClock_FUN ( simUSART_GPIO_CLK, ENABLE );
	
	/* USART GPIO 配置 */
	/* 配置复用功能 */
	GPIO_InitStructure.GPIO_Pin =  simUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(simUSART_TX_PORT, &GPIO_InitStructure);  
  
	/* 输入*/
	GPIO_InitStructure.GPIO_Pin = simUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(simUSART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 模式配置 */
	USART_InitStructure.USART_BaudRate = simUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(simUSARTx, &USART_InitStructure);
	
	
	/* 中断配置 */
	USART_ITConfig ( simUSARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断 
	USART_ITConfig ( simUSARTx, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	

	sim_USART_NVIC_Configuration ();
	
	
	USART_Cmd(simUSARTx, ENABLE);
	
}
static void sim_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* 中断*/  
	NVIC_PriorityGroupConfig ( simNVIC_PriorityGroup_x );

	/* 使能串口中断*/
	NVIC_InitStructure.NVIC_IRQChannel = simUSART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



///// 重定向c库函数printf到USART
//int fputc(int ch, FILE *f)
//{
//		/* 发送一个字节数据到USART */
//		USART_SendData(simUSARTx, (uint8_t) ch);
//		
//		/* 等待发送完毕 */
//		while (USART_GetFlagStatus(simUSARTx, USART_FLAG_TXE) == RESET);		
//	
//		return (ch);
//}

/// 重定向c库函数scanf到USART
//int fgetc(FILE *f)
//{
//		/* 等待串口输入数据 */
//		while (USART_GetFlagStatus(simUSARTx, USART_FLAG_RXNE) == RESET);
//		return (int)USART_ReceiveData(simUSARTx);
//}
/*********************************************END OF FILE**********************/

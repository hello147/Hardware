/**
  ******************************************************************************
  *USART1发送函数
  ******************************************************************************
  */ 
  
#include "usart1.h"


 /**
  * @brief  USARTx GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USARTx_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* 使能时钟 */
	macUSART_APBxClock_FUN(macUSART_CLK, ENABLE);
	macUSART_GPIO_APBxClock_FUN(macUSART_GPIO_CLK, ENABLE);
		
	/* USART GPIO 配置*/
	/* Tx 
	GPIO_InitStructure.GPIO_Pin =  macUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);  
	  
	/* USART Rx 
	GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);	
			
	/* USART 模式配置 */
	USART_InitStructure.USART_BaudRate = macUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macUSARTx, &USART_InitStructure);
	
	USART_Cmd(macUSARTx, ENABLE);
	
}

/// 重定向c库函数printf到USART
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART */
		USART_SendData(macUSARTx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(macUSARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// 重定向c库函数scanf到USART
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(macUSARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(macUSARTx);
}
/*********************************************END OF FILE**********************/

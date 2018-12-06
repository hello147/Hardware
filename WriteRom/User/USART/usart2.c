/**
  ******************************************************************************
  *USART1���ͺ���
  ******************************************************************************
  */ 
  
#include "usart2.h"
#include "common.h"
extern u8 uart_recv[40];
 /**
  * @brief  USARTx GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	/* ʹ��ʱ������ */
	simUSART_APBxClock_FUN ( simUSART_CLK, ENABLE );
	simUSART_GPIO_APBxClock_FUN ( simUSART_GPIO_CLK, ENABLE );
	
	/* USART GPIO ���� */
	/* ���ø��ù��� */
	GPIO_InitStructure.GPIO_Pin =  simUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(simUSART_TX_PORT, &GPIO_InitStructure);  
  
	/* ����*/
	GPIO_InitStructure.GPIO_Pin = simUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(simUSART_RX_PORT, &GPIO_InitStructure);
	
	/* USART1 ģʽ���� */
	USART_InitStructure.USART_BaudRate = simUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(simUSARTx, &USART_InitStructure);
	
	
	/* �ж����� */
	USART_ITConfig ( simUSARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж� 
	USART_ITConfig ( simUSARTx, USART_IT_IDLE, ENABLE ); //ʹ�ܴ������߿����ж� 	

	sim_USART_NVIC_Configuration ();
	
	
	USART_Cmd(simUSARTx, ENABLE);
	
}
static void sim_USART_NVIC_Configuration ( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	/* �ж�*/  
	NVIC_PriorityGroupConfig ( simNVIC_PriorityGroup_x );

	/* ʹ�ܴ����ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = simUSART_IRQ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



///// �ض���c�⺯��printf��USART
//int fputc(int ch, FILE *f)
//{
//		/* ����һ���ֽ����ݵ�USART */
//		USART_SendData(simUSARTx, (uint8_t) ch);
//		
//		/* �ȴ�������� */
//		while (USART_GetFlagStatus(simUSARTx, USART_FLAG_TXE) == RESET);		
//	
//		return (ch);
//}

/// �ض���c�⺯��scanf��USART
//int fgetc(FILE *f)
//{
//		/* �ȴ������������� */
//		while (USART_GetFlagStatus(simUSARTx, USART_FLAG_RXNE) == RESET);
//		return (int)USART_ReceiveData(simUSARTx);
//}
/*********************************************END OF FILE**********************/

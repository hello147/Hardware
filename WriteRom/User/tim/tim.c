

#include "stm32f10x.h"

u16 TIM_Count;	           //��ʱ������

int tim2_count=0;
u16 tim2_espcount=0;
void NVIC_Configuration(void)
{ 	
	NVIC_InitTypeDef	NVIC_InitStructure;

	#ifdef  VECT_TAB_RAM  									//���������ַѡ��
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  			//��0x20000000��ַ��Ϊ���������ַ(RAM)
	#else  
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 		//��0x08000000��ַ��Ϊ���������ַ(FLASH)  
	#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 		//�����ж���Ϊ2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 		//�ж��¼�ΪTIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   	//��ռ���ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //��Ӧ���ȼ�1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //�����ж� 
	NVIC_Init(&NVIC_InitStructure);                         //����ָ��������ʼ���жϼĴ���
}


void Init_TIMER(void)
{
	TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;			//����һ����ʱ���ṹ�����

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   	//ʹ�ܶ�ʱ��2����Ҫ����

	TIM_DeInit( TIM2);                              		//��IM2��ʱ����ʼ��λ��λֵ

	TIM_InternalClockConfig(TIM2); 							//���� TIM2 �ڲ�ʱ��
	   
	TIM_BaseInitStructure.TIM_Period = 999; 				//�����Զ����ؼĴ���ֵΪ���ֵ	0~65535֮��  												
															//TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼���
															//����ֵ���� Ҳ���� 1MS���������¼�һ��
	TIM_BaseInitStructure.TIM_Prescaler = 71;  				//�Զ���Ԥ��Ƶϵ�����ṩ����ʱ����ʱ��	0~65535֮��
															//����Ԥ��Ƶ����Ƶϵ��71����APB2=72M, TIM1_CLK=72/72=1MHz 
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�Ϊ0
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
															//TIM���ϼ���ģʽ ��0��ʼ���ϼ�����������1000����������¼�
//	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 		//�˲���ֻ�ʺ���TIM1��TIM8
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 		//����ָ��������ʼ��TIMʱ������Ĵ���
	
	TIM_ClearFlag(TIM2, TIM_IT_CC1); 					//���жϣ�����һ�����жϺ����������ж� 
      
    TIM_ARRPreloadConfig(TIM2, DISABLE);					//��ֹARR Ԥװ�ػ�����

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE); 				//ʹ��TIM2�ж�Դ 
	TIM_Cmd(TIM2, ENABLE); 									//TIM2�ܿ��أ����� 
}

void TIM2_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) 		//�ж��жϱ�־λ�Ƿ�Ϊ1��Ϊ1���ж���Ӧ����0��û��
    {
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);       	//����жϹ���λ
			 
    
	}
}




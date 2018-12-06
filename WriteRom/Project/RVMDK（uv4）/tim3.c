

#include "stm32f10x.h"

u16 TIM3_Count;	           //��ʱ������

u16 tim3_count=0;
u16 tim3_espcount=0;
u16 tim3_timecount=0;
extern  int readflag;
extern  int sendflag;
extern int timeflag;
void NVIC3_Configuration(void)
{ 	
	NVIC_InitTypeDef	NVIC_InitStructure;

	#ifdef  VECT_TAB_RAM  									//���������ַѡ��
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  			//��0x20000000��ַ��Ϊ���������ַ(RAM)
	#else  
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 		//��0x08000000��ַ��Ϊ���������ַ(FLASH)  
	#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 		//�����ж���Ϊ2
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 		//�ж��¼�ΪTIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   	//��ռ���ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //��Ӧ���ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //�����ж� 
	NVIC_Init(&NVIC_InitStructure);                         //����ָ��������ʼ���жϼĴ���
}


void Init_tim3(void)
{
	TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;			//����һ����ʱ���ṹ�����

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   	//ʹ�ܶ�ʱ��3����Ҫ����

	TIM_DeInit( TIM3);                              		//��IM3��ʱ����ʼ��λ��λֵ

	TIM_InternalClockConfig(TIM3); 							//���� TIM3 �ڲ�ʱ��
	   
	TIM_BaseInitStructure.TIM_Period = 999; 				//�����Զ����ؼĴ���ֵΪ���ֵ	0~65535֮��  												
															//TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼���
															//����ֵ���� Ҳ���� 1MS���������¼�һ��
	TIM_BaseInitStructure.TIM_Prescaler = 71;  				//�Զ���Ԥ��Ƶϵ�����ṩ����ʱ����ʱ��	0~65535֮��
															//����Ԥ��Ƶ����Ƶϵ��71����APB2=72M, TIM1_CLK=72/72=1MHz 
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�Ϊ0
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
															//TIM���ϼ���ģʽ ��0��ʼ���ϼ�����������1000����������¼�
//	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 		//�˲���ֻ�ʺ���TIM1��TIM8
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure); 		//����ָ��������ʼ��TIMʱ������Ĵ���
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update|TIM_IT_CC1); 					//���жϣ�����һ�����жϺ����������ж� 
      //TIM_ClearITPendingBit(TIM3,TIM_IT_CC1|TIM_IT_CC2);
    TIM_ARRPreloadConfig(TIM3, DISABLE);					//��ֹARR Ԥװ�ػ�����
	TIM_ITConfig(TIM3, TIM_IT_Update|TIM_IT_CC1, ENABLE); 				//ʹ��TIM2�ж�Դ 
	TIM_Cmd(TIM3, ENABLE); 									//TIM2�ܿ��أ����� 
}


void TIM3_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 		//�ж��жϱ�־λ�Ƿ�Ϊ1��Ϊ1���ж���Ӧ����0��û��
    {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);       	//����жϹ���λ
		

	}
		
	if(TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)
	{
	  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);   
			tim3_count++;
		tim3_espcount++;	
		tim3_timecount++;
   if(tim3_count>10000)
	 {
	    readflag=1;
	 }
	if(tim3_espcount>15000)
	{
			 sendflag=1;
	}
	if(tim3_timecount>30000)
	{
	     timeflag=1;
	}
	}
}


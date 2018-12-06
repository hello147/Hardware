

#include "stm32f10x.h"

u16 TIM_Count;	           //定时器计数

int tim2_count=0;
u16 tim2_espcount=0;
void NVIC_Configuration(void)
{ 	
	NVIC_InitTypeDef	NVIC_InitStructure;

	#ifdef  VECT_TAB_RAM  									//向量表基地址选择
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  			//将0x20000000地址作为向量表基地址(RAM)
	#else  
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 		//将0x08000000地址作为向量表基地址(FLASH)  
	#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 		//设置中断组为2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 		//中断事件为TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   	//抢占优先级0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //响应优先级1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //允许中断 
	NVIC_Init(&NVIC_InitStructure);                         //根据指定参数初始化中断寄存器
}


void Init_TIMER(void)
{
	TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;			//定义一个定时器结构体变量

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   	//使能定时器2，重要！！

	TIM_DeInit( TIM2);                              		//将IM2定时器初始化位复位值

	TIM_InternalClockConfig(TIM2); 							//配置 TIM2 内部时钟
	   
	TIM_BaseInitStructure.TIM_Period = 999; 				//设置自动重载寄存器值为最大值	0~65535之间  												
															//TIM_Period（TIM1_ARR）=1000，计数器向上计数到1000后产生更新事件，
															//计数值归零 也就是 1MS产生更新事件一次
	TIM_BaseInitStructure.TIM_Prescaler = 71;  				//自定义预分频系数，提供给定时器的时钟	0~65535之间
															//设置预分频器分频系数71，即APB2=72M, TIM1_CLK=72/72=1MHz 
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割为0
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
															//TIM向上计数模式 从0开始向上计数，计数到1000后产生更新事件
//	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 		//此参数只适合于TIM1和TIM8
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 		//根据指定参数初始化TIM时间基数寄存器
	
	TIM_ClearFlag(TIM2, TIM_IT_CC1); 					//清中断，以免一启用中断后立即产生中断 
      
    TIM_ARRPreloadConfig(TIM2, DISABLE);					//禁止ARR 预装载缓冲器

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE); 				//使能TIM2中断源 
	TIM_Cmd(TIM2, ENABLE); 									//TIM2总开关：开启 
}

void TIM2_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) 		//判断中断标志位是否为1；为1则中断响应啦，0则没有
    {
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);       	//清楚中断挂起位
			 
    
	}
}




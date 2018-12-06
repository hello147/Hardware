
#include "sys.h"	
#include "mod.h"
#include "SysTick.h"
#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include "stm32f10x.h"
#include <string.h>
extern usart_st  usart_data;
extern modbus_rtust  modbus_data;
extern unsigned int sec;
	extern char senddata[20];
	extern char address;
	extern char data[2];
#define usart_rx_buf_len    10 //定义接收区长度
#define MAX_RE_TIME_COUNT 	10
#define RT_TIME_OUT 				2
#define MODBUSREV_TIME_OUT  100

int i=0;

//初始化IO mod串口
//bound:波特率   
void Mod_USARTx_Config(){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(modUSART_CLK|modUSART_GPIO_CLK, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(modUSARTx);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = modUSART_TX_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(modUSART_TX_PORT, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = modUSART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(modUSART_RX_PORT, &GPIO_InitStructure);  //初始化PA10


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = modUSART_BAUD_RATE;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(modUSARTx, &USART_InitStructure); //初始化串口

   //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = modUSART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    USART_ITConfig(modUSARTx, USART_IT_RXNE, ENABLE);//开启中断

    USART_Cmd(modUSARTx, ENABLE);                    //使能串口 

}


 /*  函数名：modbus_rev_deal()
**   
**功能描述：判断是否接收一帧完整数据
**
*/
unsigned char  modbus_rev_deal()
{  
    uint16_t  crc,rec_len;
rec_len=0;
	crc=0;
    if(usart_data.deal_flag==3)
		{
	  rec_len=(unsigned short int)usart_data.rx_len_ed;
		crc=(unsigned short int)usart_data.rx_buf[rec_len-2];
		crc=(crc<<8)+(unsigned short int)usart_data.rx_buf[rec_len-1];
		 if(crc==Modbus_CRC16(usart_data.rx_buf,rec_len-2))  //如果校验通过
		 {
			 usart_data.deal_flag=0; //把状态改为  0：不发送不接收不校验
			return 1;
		 }
	    return 0;
    }
		return 0;
}
//mod串口中断
void modUSART_INT_FUN()
{
unsigned char temp;
	unsigned char n=0;
	temp=0;
	if((USART_GetITStatus(modUSARTx, USART_IT_RXNE) != RESET))  //接收中断
	{
		usart_data.rx_time_outn=0;
	  temp=USART_ReceiveData(modUSARTx);
		if(usart_data.rec_over==0)
		 {
			usart_data.rx_buf[usart_data.rx_len_ed++]=temp;
		 }
	}
	
	                                 
	if((USART_GetITStatus(modUSARTx,USART_IT_TXE)!=RESET)&&usart_data.deal_flag==1)     //发送中断
	{
		if(usart_data.tx_len_ed<usart_data.tx_len)       //如果没发送完
		{
		   USART_SendData(modUSARTx,usart_data.tx_buf[usart_data.tx_len_ed++]);
		}
		else   //发送结束后
		{
			USART_ITConfig(modUSARTx, USART_IT_TXE, DISABLE);  //关闭发送
			USART_ITConfig(modUSARTx, USART_IT_RXNE, ENABLE);   //使能接收
			usart_data.tx_len=0;          //需要发送的长度归零
			usart_data.tx_len_ed=0;      //已经发送的长度归零
			usart_data.rx_len_ed=0;
			usart_data.deal_flag=2;     //改状态为允许接收
			usart_data.t_time_delay=0;
			usart_data.r_time_delay=0;
			usart_data.rx_time_outn=0;
			   
		}
}
	}

//滴答定时器中断
void SysTick_Handler(void)
{

unsigned int i;
	i=0;
		TimingDelay_Decrement();
	if((usart_data.rx_len_ed>0)&&(usart_data.rec_over==0))
	{
	  usart_data.rx_time_outn++;
		if( usart_data.rx_time_outn>10)  //超过10ms（10000us）没接收到数据认为一帧接收结束了
		 {
			  usart_data.rec_over=1;
		    usart_data.deal_flag=3;//允许校验
		 }	 
	}
	
	 if(usart_data.deal_flag==2)
	{
	  if((++usart_data.r_time_delay)>=2)
		{
			
			RT_485_R;
       usart_data.rec_over=0;//一帧没接收完毕
			if(usart_data.deal_flag==1)
			{
			  usart_data.rev_time_outcnt=100;
				usart_data.deal_flag=2;
			}	
		}
	}
   sec++;
//	   if((usart_data.rev_time_outcnt>0)&&(usart_data.deal_flag==2))
//			 usart_data.rev_time_outcnt--;
}

void claer()
{
  memset(usart_data.rx_buf,0,usart_data.rx_len);    //接收区清零
	 memset(usart_data.tx_buf,0,usart_data.tx_len);    //接收区清零en
	usart_data.rx_len_ed=0; 
	usart_data.tx_len_ed=0; 
	uint16_t address=0;
	uint16_t data=0;
}


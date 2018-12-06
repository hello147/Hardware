
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
#define usart_rx_buf_len    10 //�������������
#define MAX_RE_TIME_COUNT 	10
#define RT_TIME_OUT 				2
#define MODBUSREV_TIME_OUT  100

int i=0;

//��ʼ��IO mod����
//bound:������   
void Mod_USARTx_Config(){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(modUSART_CLK|modUSART_GPIO_CLK, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(modUSARTx);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = modUSART_TX_PIN; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(modUSART_TX_PORT, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = modUSART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(modUSART_RX_PORT, &GPIO_InitStructure);  //��ʼ��PA10


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = modUSART_BAUD_RATE;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(modUSARTx, &USART_InitStructure); //��ʼ������

   //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = modUSART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    USART_ITConfig(modUSARTx, USART_IT_RXNE, ENABLE);//�����ж�

    USART_Cmd(modUSARTx, ENABLE);                    //ʹ�ܴ��� 

}


 /*  ��������modbus_rev_deal()
**   
**�����������ж��Ƿ����һ֡��������
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
		 if(crc==Modbus_CRC16(usart_data.rx_buf,rec_len-2))  //���У��ͨ��
		 {
			 usart_data.deal_flag=0; //��״̬��Ϊ  0�������Ͳ����ղ�У��
			return 1;
		 }
	    return 0;
    }
		return 0;
}
//mod�����ж�
void modUSART_INT_FUN()
{
unsigned char temp;
	unsigned char n=0;
	temp=0;
	if((USART_GetITStatus(modUSARTx, USART_IT_RXNE) != RESET))  //�����ж�
	{
		usart_data.rx_time_outn=0;
	  temp=USART_ReceiveData(modUSARTx);
		if(usart_data.rec_over==0)
		 {
			usart_data.rx_buf[usart_data.rx_len_ed++]=temp;
		 }
	}
	
	                                 
	if((USART_GetITStatus(modUSARTx,USART_IT_TXE)!=RESET)&&usart_data.deal_flag==1)     //�����ж�
	{
		if(usart_data.tx_len_ed<usart_data.tx_len)       //���û������
		{
		   USART_SendData(modUSARTx,usart_data.tx_buf[usart_data.tx_len_ed++]);
		}
		else   //���ͽ�����
		{
			USART_ITConfig(modUSARTx, USART_IT_TXE, DISABLE);  //�رշ���
			USART_ITConfig(modUSARTx, USART_IT_RXNE, ENABLE);   //ʹ�ܽ���
			usart_data.tx_len=0;          //��Ҫ���͵ĳ��ȹ���
			usart_data.tx_len_ed=0;      //�Ѿ����͵ĳ��ȹ���
			usart_data.rx_len_ed=0;
			usart_data.deal_flag=2;     //��״̬Ϊ�������
			usart_data.t_time_delay=0;
			usart_data.r_time_delay=0;
			usart_data.rx_time_outn=0;
			   
		}
}
	}

//�δ�ʱ���ж�
void SysTick_Handler(void)
{

unsigned int i;
	i=0;
		TimingDelay_Decrement();
	if((usart_data.rx_len_ed>0)&&(usart_data.rec_over==0))
	{
	  usart_data.rx_time_outn++;
		if( usart_data.rx_time_outn>10)  //����10ms��10000us��û���յ�������Ϊһ֡���ս�����
		 {
			  usart_data.rec_over=1;
		    usart_data.deal_flag=3;//����У��
		 }	 
	}
	
	 if(usart_data.deal_flag==2)
	{
	  if((++usart_data.r_time_delay)>=2)
		{
			
			RT_485_R;
       usart_data.rec_over=0;//һ֡û�������
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
  memset(usart_data.rx_buf,0,usart_data.rx_len);    //����������
	 memset(usart_data.tx_buf,0,usart_data.tx_len);    //����������en
	usart_data.rx_len_ed=0; 
	usart_data.tx_len_ed=0; 
	uint16_t address=0;
	uint16_t data=0;
}


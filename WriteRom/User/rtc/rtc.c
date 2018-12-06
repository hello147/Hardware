#include "stm32f10x.h"
#include "rtc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"

volatile struct Data_Time  timer;  //����һ��ʱ��ṹ�����
extern char newtime[26];
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Init_RTC
** ��������: RTC��ʼ��
** ������������
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
u8 Init_RTC(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
																	//ʹ��PWR��BKP����ʱ��   		
	PWR_BackupAccessCmd(ENABLE);									//ʹ��RTC�ͺ󱸼Ĵ������� 
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x5555)						//��ָ���ĺ󱸼Ĵ����ж������ݣ��ж��Ƿ�Ϊ��һ������
	{	
      	//printf("ʱ�����á�����\r\n");																
		BKP_DeInit();												//������BKP��ȫ���Ĵ�������Ϊȱʡֵ 	
		RCC_LSEConfig(RCC_LSE_ON);									//ʹ���ⲿ����ʱ�� 32.768KHz
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)			//���ָ����RCC��־λ�������,�ȴ����پ������
  		{}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);						//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);										//ʹ��RTCʱ��  
		RTC_WaitForSynchro();										//�ȴ�RTC�Ĵ���(RTC_CNT,RTC_ALR��RTC_PRL)��RTC APBʱ��ͬ��
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//ʹ��RTC���ж�
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_SetPrescaler(32767); 									//����RTCԤ��Ƶ��ֵ  RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		Time_Set();													//ʱ������	
   	BKP_WriteBackupRegister(BKP_DR1, 0x5555);					//��ָ���ĺ󱸼Ĵ�����д���û���������0X5555���жϱ�־										
	}																 	
	else															//���ǵ�һ������ ������ʱ
	{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)			//���ָ����RCC��־λ�������:POR/PDR��λ
		{
      		;//printf("�ϵ縴λ������\r\n");
		}											
		else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)		//���ָ����RCC��־λ�������:�ܽŸ�λ
		{
      		;//printf("�ⲿ��λ������\r\n");
		}
    	//printf("����Ҫ���á�����\r\n");
		
		RTC_WaitForSynchro();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		
		RTC_ITConfig(RTC_IT_SEC, ENABLE);							//ʹ��RTC���ж�

		RTC_WaitForLastTask();										//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}		    				     
	Time_Get();														//����ʱ�� 
	
	RCC_ClearFlag();												//���RCC�ĸ�λ��־λ
	
	return 0; //ok		
}


//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 			//���������ݱ�	  

const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	//ƽ����·����ڱ�

u8 Time_Update(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)											//��������ݵ��������
		{
		if(Is_Leap_Year(t))seccount+=31622400;						//�����������
		else seccount+=31536000;			  						//ƽ���������
		}
	smon-=1;
	for(t=0;t<smon;t++)	   											//��ǰ���·ݵ����������
		{
		seccount+=(u32)mon_table[t]*86400;						//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;				//����2�·�����һ���������	   
		}
	seccount+=(u32)(sday-1)*86400;								//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;									//Сʱ������
	seccount+=(u32)min*60;	 									//����������
	seccount+=sec;													//�������Ӽ���ȥ
											    
	RTC_WaitForLastTask();											//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_SetCounter(seccount);										//����RTC��������ֵ
	RTC_WaitForLastTask();											//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
	return 0;		
}
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 Time_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
	 
	timecount = RTC_GetCounter();  //��� RTC ������ֵ(������)											 
	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		timer.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(timer.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		timer.w_month=temp1+1;//�õ��·�
		timer.w_date=temp+1;  //�õ����� 
	}
	temp=timecount%86400;     //�õ�������   	   
	timer.hour=temp/3600;     //Сʱ
	timer.min=(temp%3600)/60; //����	
	timer.sec=(temp%3600)%60; //����
	timer.week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//��ȡ����   
	return 0;
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//������������������� 
//����ֵ�����ں�																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 	 
	if (yearH>19)yearL+=100;// ���Ϊ21����,�������100 
  
	temp2=yearL+yearL/4;  	// ����������ֻ��1900��֮���
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
} 
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}
/*****************************************************************************
** ��������: RTC_IRQHandler
** ��������: RTC�жϷ�����
				ÿ�봥��һ��  
*****************************************************************************/
void RTC_IRQHandler(void)
{							    
	if(RTC_GetITStatus(RTC_IT_SEC))			//�����ж�
	{							
		Time_Get();							//����ʱ�� 	 
	}
	if(RTC_GetITStatus(RTC_IT_ALR))			//�����ж�
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);	//�������ж�		  								 				
	}
	RTC_ClearITPendingBit(RTC_IT_SEC);		//�������������жϱ�־		  								 
	RTC_WaitForLastTask();					//�ȴ�RTC�Ĵ����������
}
/*****************************************************************************
** ��������: Time_Set
** ��������: 
*****************************************************************************/
void Time_Set(void)
{
  uint8_t newhour;
	uint8_t newmin;
	uint8_t newsec;			
	//������������
	uint16_t newyear;
	uint8_t  newmonth;
	uint8_t  newdate;
	  strncpy(newyear,newtime+6,4);
	  strncpy(newmonth,newtime+11,2);
	  strncpy(newdate,newtime+14,2);
	  strncpy(newhour,newtime+17,2);
	  strncpy(newmin,newtime+20,2);
	  strncpy(newsec,newtime+13,2);
	  Time_Update(newyear,newmonth,newdate,newhour,newmin,newsec);
}

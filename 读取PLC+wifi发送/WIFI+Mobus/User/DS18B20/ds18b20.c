#include "ds18b20.h"
#include "SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

/*PA0-DQ*/

//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PA0 OUTPUT
    DS18B20_DQ_OUT=0; //����DQ
    Delay_us(750);    //����750us
    DS18B20_DQ_OUT=1; //DQ=1 
	Delay_us(15);     //15US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		Delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUT=0; 
	Delay_us(2);
    DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PA0 INPUT
	Delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    Delay_us(50);           
    return data;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            Delay_us(2);                            
            DS18B20_DQ_OUT=1;
            Delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            Delay_us(60);             
            DS18B20_DQ_OUT=1;
            Delay_us(2);                          
        }
    }
}

//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 ��ʼת��
{   						               
    DS18B20_Rst();	  //��λ 
	DS18B20_Check();	//��⴫�����Ƿ����
    DS18B20_Write_Byte(0xcc);//rom
    DS18B20_Write_Byte(0x44);// ת��
} 

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PORTA��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//PORTA0 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_0);    //���1

	DS18B20_Rst();

	return DS18B20_Check();
}  
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
float DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	float Temperature;
	short tem;
	DS18B20_Init();		//��ʼ��
    DS18B20_Start ();                    // ds1820��ʼת��
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// дrom
    DS18B20_Write_Byte(0xbe);// ת��	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õͰ�λ
    Temperature=(float)((float)tem*0.0625);//ת��     
	if(temp)return Temperature; //�����¶�ֵ
	else return -Temperature;    
	//return Temperature;
	
} 

/*����ת�������������*/
const char* Temp_convert(void){
	static char temp_str[50]={0};
	float temp;
	temp=DS18B20_Get_Temp();

	if(0<=temp<10){
		sprintf (temp_str, "%03.1f",DS18B20_Get_Temp());
	}

	else if(10<=temp<100){
		sprintf (temp_str, "%04.1f",DS18B20_Get_Temp());
	}

	else if(100<=temp<1000){
		sprintf (temp_str, "05.1f",DS18B20_Get_Temp());
	}
	return temp_str;
}

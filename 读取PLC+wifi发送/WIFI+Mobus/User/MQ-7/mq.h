#ifndef __MQ_H
#define __MQ_H	
#include "sys.h"

/*
	I/O:ADC1-PA1
*/

void Adc_Init(void);		 //ADC��ʼ��
u16  Get_Adc(u8 ch); 		   //��ȡ��ѹֵ
u16 Get_Adc_Average(u8 ch,u8 times); 	//�ȶ�ֵ
float Get_Adc_Convert(void);		//���ݴ���-ת��
//void Send_co(void);			//��������
const char* CO_convert(void);			//����ת������
 
#endif 

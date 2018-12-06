#ifndef __MQ_H
#define __MQ_H	
#include "sys.h"

/*
	I/O:ADC1-PA1
*/

void Adc_Init(void);		 //ADC初始化
u16  Get_Adc(u8 ch); 		   //获取电压值
u16 Get_Adc_Average(u8 ch,u8 times); 	//稳定值
float Get_Adc_Convert(void);		//数据处理-转换
//void Send_co(void);			//发送数据
const char* CO_convert(void);			//数据转换处理
 
#endif 

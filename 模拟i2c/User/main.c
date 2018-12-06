/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   AT24C02���ԣ����ģ��I2C��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h" 
#include "./i2c/bsp_i2c_ee.h"






#define  E2PROM_PORT       GPIOE
#define  E2PROM_SDA  			GPIO_Pin_15
#define  E2PROM_SCL  			GPIO_Pin_14
#define  E2PROM_CMD_WRITE  	0xa0
#define  E2PROM_CMD_READ   	0xa1
#define  I2C_DELAY          	5                                                        // 
#define  E2PROM_ADDR_SIZE	16

/******************************************************************************
**������:HSI_Configuration
**��      ��:
 /**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
void HSI_Configuration(void)  
{  
	RCC_ClocksTypeDef RCC_ClockFreq;
	
	    RCC_DeInit();  
    	/* enable HSI */  
    	RCC_HSICmd(ENABLE);  
    	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)  
    	{}  
      
    	/* Enable Prefetch Buffer */  
    	FLASH_PrefetchBufferCmd(ENABLE);  
    	/* set Flash Latency, 0-24M: FLASH_Latency_0, 24M < sysclk <48M: FLASH_Latency_1*/  
    	FLASH_SetLatency(FLASH_Latency_1);  
      
    	/* Configures the AHB clock (HCLK)  */  
    	RCC_HCLKConfig(RCC_CFGR_HPRE_DIV1);  
      
    	/* Configures the APB clock (PCLK) */  
    	//RCC_PCLKConfig(RCC_CFGR_PPRE_DIV1);  
    	RCC_PCLK1Config(RCC_HCLK_Div1);
      
    	/* Configures PLL source, (8M/2) * 12 = 48M */  
    	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_CFGR_PLLMULL10);   
    	RCC_PLLCmd(ENABLE);  
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)  
    	{}  
      
    	/* Configures system clock source */  
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  
    	/* Wait till PLL is used as system clock source */  
    	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)  
    	{}  
    	RCC_GetClocksFreq(&RCC_ClockFreq);//��ȡ���ú��ʱ��
}


uint8_t MoniIIC = 0;
uint16_t Cycle = 0;
uint8_t ReadBuf[70];
uint8_t WriteBuf[62]={"Id:N01-0001simid:17854153611preiod:20addr:118.024.157.134:8080"};

uint8_t bTestOK = TRUE;
int main(void)
{
  	uint16_t i;
	  uint8_t flg = 0;
    LED_GPIO_Config();
    HSI_Configuration();
    LED_BLUE;

    /* ���ڳ�ʼ�� */
		USART_Config();
    I2C_Config();
	
		printf("eeprom ���ģ��i2c�������� \r\n");		
  
  while(1)
	{
		for(i=0;i<62;i++)
		{
			E2promWriteByte(i,WriteBuf[i]);
			
			Delay(15); 
		}
		//E2promWriteBuffer(0,WriteBuf,104);
//		for(Cycle=0;Cycle<104;Cycle++)
//		{
//			ReadBuf[Cycle] = E2promReadByte(Cycle);
//			//printf("������%x\r\n",ReadBuf[i]);
//			//Delay(5); 
//		}
     E2promReadBuffer(0,ReadBuf,62);
		for(i=0;i<62;i++)
		{
			if(WriteBuf[i]!=ReadBuf[i])
			{
				bTestOK = FALSE;
				break;
			}
			else
      {
			  printf("������%c\r\n",ReadBuf[i]);
			}		
		}
		
		if(bTestOK)
		{
			MoniIIC++;
			Delay(15); 
    	//printf("������%x\r\n",ReadBuf[i]);
		}
		
	}
}
/*********************************************END OF FILE**********************/

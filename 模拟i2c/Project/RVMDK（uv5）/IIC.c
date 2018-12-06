



#include "stm32f10x.h"


#define  E2PROM1_PORT       GPIOE
#define  E2PROM1_SDA  			GPIO_Pin_15
#define  E2PROM1_SCL  			GPIO_Pin_14
#define  E2PROM1_CMD_WRITE  	0xa0
#define  E2PROM1_CMD_READ   	0xa1
#define  I2C1_DELAY                    	5                                                        // 
#define  E2PROM1_ADDR_SIZE	16




void Delay(u16 speed)
{
	u16 i;
	while(speed!=0)
	{
		speed--;
		for(i=0;i<700;i++);
	}
}


static void DelayMs(u16 uc)
{
        u16  i, j;
        for (i=0; i<uc; i++) 
        {
                for (j=0; j<I2C1_DELAY; j++);
        }
}
/************************************************************* 
*�������ƣ�I2CStart 
*�������ܣ�I2C��ʼ�ź� 
*��������� 
*��������� 
*��           ע��ʱ���߸�ʱ���������ɸߵ��͵����䣬��ʾI2C��ʼ�ź� 
**************************************************************/ 
void I2CStart( void ) 
{ 
	  GPIO_SetBits( E2PROM1_PORT, E2PROM1_SDA ); 
	  DelayMs(1); 
	  GPIO_SetBits( E2PROM1_PORT, E2PROM1_SCL ); 
	  DelayMs(1); 
	  GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SDA ); 
	  DelayMs(1); 

	  GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SCL ); 
} 

/************************************************************* 
*�������ƣ�I2CStop 
*�������ܣ�I2Cֹͣ�ź� 
*��������� 
*��������� 
*��           ע��ʱ���߸�ʱ���������ɵ͵��ߵ����䣬��ʾI2Cֹͣ�ź� 
**************************************************************/ 
void I2CStop( void ) 
{ 
	  GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SDA ); 
	  DelayMs(1); 
	  GPIO_SetBits( E2PROM1_PORT, E2PROM1_SCL ); 
	  DelayMs(1); 
	  GPIO_SetBits( E2PROM1_PORT, E2PROM1_SDA ); 
	  DelayMs(1); 

	  GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SCL ); 
} 

/************************************************************* 
*�������ƣ�I2CSlaveAck 
*�������ܣ�I2C�ӻ��豸Ӧ���ѯ 
*��������� 
*��������� 
*��           ע�� ����SET˵����Ӧ��
**************************************************************/ 
unsigned char I2CSlaveAck( void ) 
{ 
	  GPIO_InitTypeDef GPIO_InitStruct; 
	  unsigned int TimeOut; 
	  unsigned char RetValue; 

	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*����һ��Ҫ������������������ܶ�������*/ 
	  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	  GPIO_InitStruct.GPIO_Pin = E2PROM1_SDA; 
	  GPIO_Init( E2PROM1_PORT, &GPIO_InitStruct ); 

	  GPIO_SetBits( E2PROM1_PORT, E2PROM1_SCL ); 
	  TimeOut = 1000; 
	  while( TimeOut--) 
	  { 
	    	if( SET == GPIO_ReadInputDataBit( E2PROM1_PORT, E2PROM1_SDA ) ) 
	    	{ 
	      		RetValue = RESET; 
	    	} 
	    	else 
	    	{ 
	      		RetValue = SET; 
	      		break; 
	    	} 
	  } 
	  GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SCL ); 
	   
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	  GPIO_Init( E2PROM1_PORT, &GPIO_InitStruct ); 
	  
	  return RetValue; 
} 

/************************************************************* 
*�������ƣ�I2CWriteByte 
*�������ܣ�I2Cдһ�ֽ����� 
*��������� 
*��������� 
*��           ע�� 
**************************************************************/ 
void I2CWriteByte( unsigned char byte ) 
{ 
	  unsigned char i; 

	  for( i=0; i<8; i++ ) 
	  { 
	    	if( 0X80 & byte ) 
	      		GPIO_SetBits( E2PROM1_PORT, E2PROM1_SDA ); 
	    	else 
	      		GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SDA ); 
	    	byte <<= 1; 
	    	DelayMs(1); 

	    	GPIO_SetBits( E2PROM1_PORT, E2PROM1_SCL ); 
	    	DelayMs(1); 
	    	GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SCL ); 
	    	DelayMs(1); 
	  } 
} 

/************************************************************* 
*�������ƣ�I2CReadByte 
*�������ܣ�I2C��һ�ֽ����� 
*��������� 
*��������� 
*��           ע�� 
**************************************************************/ 
unsigned char I2CReadByte( void ) 
{ 
	unsigned char i; 
	unsigned char ReadValue = 0; 
	GPIO_InitTypeDef GPIO_InitStruct; 
	unsigned char bit; 

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  /*����һ��Ҫ������������������ܶ�������*/ 
	GPIO_InitStruct.GPIO_Pin = E2PROM1_SDA; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init( E2PROM1_PORT, &GPIO_InitStruct ); 
	for( i=0; i<8; i++ ) 
	{ 
		GPIO_SetBits( E2PROM1_PORT, E2PROM1_SCL ); 
		DelayMs(1); 
		if( SET == GPIO_ReadInputDataBit( E2PROM1_PORT, E2PROM1_SDA ) ) 
		  	bit = 0X01; 
		else 
		  	bit = 0x00; 
		   
		ReadValue = (ReadValue<<1)|bit; 
		GPIO_ResetBits( E2PROM1_PORT, E2PROM1_SCL ); 
		DelayMs(1); 
	} 

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init( E2PROM1_PORT, &GPIO_InitStruct ); 
	
	return ReadValue; 
} 

/************************************************************* 
*�������ƣ�E2promWriteByte 
*�������ܣ�E2PROMָ����ַдһ�ֽ����� 
*���������addr  E2PROM��ַ 
                             data  д������� 
*���������SET: д��������RESET:д����� 
*��           ע�� 
**************************************************************/ 
uint8_t E2promWriteByte( uint16_t addr, uint8_t data ) 
{ 
	//asm("CPSID I");  //���ж� 
	I2CStart(); 

	I2CWriteByte( E2PROM1_CMD_WRITE ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	}
	
#if(E2PROM1_ADDR_SIZE==16)
	I2CWriteByte(addr >> 8) ; 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 
#endif
	I2CWriteByte( addr & 0xFF); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 
	I2CWriteByte( data ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 
	I2CStop(); 
	return SET; 
} 


/************************************************************* 
*�������ƣ�E2promWriteByte 
*�������ܣ�E2PROMָ����ַдһ�ֽ����� 
*���������addr  E2PROM��ַ 
                             data  д������� 
*���������SET: д��������RESET:д����� 
*��           ע�� 
**************************************************************/ 
uint8_t E2promWriteBuffer( uint16_t addr, uint8_t *Buf ,uint16_t len) 
{  
	I2CStart(); 

	I2CWriteByte( E2PROM1_CMD_WRITE ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	}
	
#if(E2PROM1_ADDR_SIZE==16)
	I2CWriteByte(addr >> 8) ; 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 
#endif
	I2CWriteByte( addr & 0xFF); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 

	while(len--)
	{
		I2CWriteByte( *Buf ); 
		Buf++;
		if( RESET == I2CSlaveAck() ) 
		{ 
			return RESET; 
		} 
	}
	I2CStop(); 
	

	return SET; 
	 
} 


/************************************************************* 
*�������ƣ�E2promReadByte 
*�������ܣ�E2PROMָ����ַ��һ�ֽ����� 
*���������addr  E2PROM��ַ 
*������������ض��������� 
*��           ע�� 
**************************************************************/  
uint8_t E2promReadByte( unsigned  int addr ) 
{ 
	unsigned char ReadValue; 
	I2CStart(); 
	I2CWriteByte( E2PROM1_CMD_WRITE ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	}
#if(E2PROM1_ADDR_SIZE==16)	
	I2CWriteByte(addr >> 8 ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	}  
#endif

	I2CWriteByte( addr & 0xFF ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 
	I2CStart(); 
	I2CWriteByte( E2PROM1_CMD_READ ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 
	ReadValue = I2CReadByte(); 
	I2CStop(); 

	return ReadValue;   
}

/************************************************************* 
*�������ƣ�E2promReadBufer 
*�������ܣ�E2PROMָ����ַ��һ�ֽ����� 
*���������addr  E2PROM��ַ 
*������������ض��������� 
*��           ע�� 
**************************************************************/  
uint8_t E2promReadBuffer( unsigned  int addr ,unsigned char *buf,unsigned int len) 
{ 
	//unsigned char ReadValue; 
	I2CStart(); 
	I2CWriteByte( E2PROM1_CMD_WRITE ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	}
#if(E2PROM1_ADDR_SIZE==16)	
	I2CWriteByte(addr >> 8 ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	}  
#endif

	I2CWriteByte( addr & 0xFF ); 
	if( RESET == I2CSlaveAck() ) 
	{ 
		return RESET; 
	} 

		
	while(len--)
	{
		I2CStart(); 
		I2CWriteByte( E2PROM1_CMD_READ ); 
		if( RESET == I2CSlaveAck() ) 
		{ 
			return RESET; 
		} 
		*buf = I2CReadByte(); 
		buf++;
		I2CStop(); 
	}
	
	//I2CStop(); 
	//return ReadValue;   
}

/******************************************************************************
**�� �� ��: I2C_Configuration
**��	  ��:
**��	  ��:
**��	  ��:IIC����
*******************************************************************************/
void I2C_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	/* PB14,15 SCL and SDA */
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(E2PROM1_PORT, &GPIO_InitStructure);
	
}

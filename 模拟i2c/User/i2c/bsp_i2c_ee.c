/*
��������Ϊһ24C02
*/

#include "stm32f10x.h"


//#define AT24C01A		//24C01A,I2Cʱ��������24C02һ��
//#define AT24C01		//24C01,I2C��ʱ�����ͨ���е㲻ͬ

#define AT24C256		//24C01A,I2Cʱ��������24C02һ��


#define EEPROM_ADDR		0xA0
#define I2C_PAGESIZE		32		//24C01/01Aҳ������4��
char TEXT_Buffer[]={"asdf"};
char readbuffer[];
///******************************************************************************
//**�� �� ��: Delay
//**��	  ��:
//**��	  ��:
//**��	  ��:��ʱ
//*******************************************************************************/
//static void Delay(uint32_t nCount)
//{
//  	for(; nCount != 0; nCount--);
//}

/******************************************************************************
**�� �� ��: I2C_Configuration
**��	  ��:
**��	  ��:
**��	  ��:IIC����
*******************************************************************************/
void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

	/* PB6,7 SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    	I2C_DeInit(I2C2);
    	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    	I2C_InitStructure.I2C_OwnAddress1 = 0x30;	    //�����豸�����ַ
    	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    	I2C_InitStructure.I2C_ClockSpeed = 100000;      //100K�ٶ�
    
	I2C_Cmd(I2C2, ENABLE);
	I2C_Init(I2C2, &I2C_InitStructure);
	
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C2, ENABLE);

}

/******************************************************************************
**�� �� ��: EEPROM_ReadBuffer
**��	  ��:
**��	  ��:
**��	  ��:  ��ȡ����ֽ�
*******************************************************************************/ 
void EEPROM_ReadBuffer(u16 addr ,u8* pBuffer,u16 no)
{
    if(no==0)
		return;	
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
			
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C2, ENABLE);

	/* ������ʼλ */
    I2C_GenerateSTART(I2C2, ENABLE);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));         //EV5,��ģʽ

#ifdef AT24C256	
    /*����������ַ(д)*/
    I2C_Send7bitAddress(I2C2,  EEPROM_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*���͵�ַ*/
	I2C_SendData(I2C2, (addr&0xFF00)>>8);
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));   //�����ѷ���
	I2C_SendData(I2C2, (addr&0x00FF));
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
		
	/*��ʼλ*/
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	
	/*������*/
	I2C_Send7bitAddress(I2C2, EEPROM_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	
#else	
	/*����������ַ(��)24C01*/
	I2C_Send7bitAddress(I2C2, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (no)
    {
		if(no==1)
		{
     		I2C_AcknowledgeConfig(I2C2, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
    		I2C_GenerateSTOP(I2C2, ENABLE);			//����ֹͣλ
		}
	    
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)); /* EV7 */
	    *pBuffer = I2C_ReceiveData(I2C2);
	   printf("��������%s\r\n",pBuffer);
	    pBuffer++;
	    /* Decrement the read bytes counter */
	    no--;
    }
	//�ٴ�����Ӧ��ģʽ
	I2C_AcknowledgeConfig(I2C2, ENABLE);
}


/******************************************************************************
**�� �� ��: I2C_Standby_Check
**��	  ��:
**��	  ��:
**��	  ��:  ������
*******************************************************************************/ 
void I2C_Standby_Check(void)      
{
  	vu16 SR1_Tmp;
  	do
  	{
    		/*��ʼλ*/
    		I2C_GenerateSTART(I2C2, ENABLE);
    		/*��SR1*/
    		SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);
    		/*������ַ(д)*/
    	#ifdef AT24C256
		I2C_Send7bitAddress(I2C2, EEPROM_ADDR, I2C_Direction_Transmitter);
	#else
		I2C_Send7bitAddress(I2C2, 0, I2C_Direction_Transmitter);
	#endif
  }while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));
  

  	I2C_ClearFlag(I2C2, I2C_FLAG_AF);
  	/*ֹͣλ*/    
  	I2C_GenerateSTOP(I2C2, ENABLE);
}



/******************************************************************************
**�� �� ��: I2C_EE_WriteByte
**��      ��:
**��      ��:
**��      ��:  д��1���ֽ�
*******************************************************************************/ 
void I2C_EE_WriteByte(u16 addr,u8 dat)
{
    	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
    	
	/* ��ʼλ */
  	I2C_GenerateSTART(I2C2, ENABLE);
  	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C256
  	/* ����������ַ(д)*/
  	I2C_Send7bitAddress(I2C2, EEPROM_ADDR, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  	/*���͵�ַ*/
  	I2C_SendData(I2C2, (addr&0xFF00)>>8);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C2, (addr&0x00FF));
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2C2, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	/* дһ���ֽ�*/
  	I2C_SendData(I2C2, dat); 
  	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	/* ֹͣλ*/
  	I2C_GenerateSTOP(I2C2, ENABLE);
  
  	I2C_Standby_Check();
}

/******************************************************************************
**�� �� ��: I2C_EE_WritePage
**��      ��:
**��      ��:
**��      ��:  ��ҳд������
*******************************************************************************/ 
void I2C_EE_WritePage(u8 addr,u8* pBuffer, u8 no)
{
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));

	/*��ʼλ*/
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); 

#ifdef AT24C256
	/*������ַ(д)*/
	I2C_Send7bitAddress(I2C2, EEPROM_ADDR, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	/*д��ֵַ*/
	I2C_SendData(I2C2, addr);
	while(! I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C2, addr);
	while(! I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else
	I2C_Send7bitAddress(I2C2, addr<<1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); 
#endif
	while(no--)  
	{
	  I2C_SendData(I2C2, *pBuffer); 
	  pBuffer++; 
	  while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	/*ֹͣλ*/
	I2C_GenerateSTOP(I2C2, ENABLE);
}



/******************************************************************************
**�� �� ��: EEPROM_WriteBuffer
**��      ��:
**��      ��:
**��      ��:  д�����ֽ�
*******************************************************************************/
void EEPROM_WriteBuffer(u8 addr,u8* pBuffer,  u16 no)
{
	u8 temp;

	//1.�Ȱ�ҳ������Ĳ���д��
	temp=addr % I2C_PAGESIZE;
	if(temp)
	{
		temp=I2C_PAGESIZE-temp;
		I2C_EE_WritePage(addr,pBuffer,  temp);
		no-=temp;
		addr+=temp;
		pBuffer+=temp;
		printf("д����%s\r\n",pBuffer);
		I2C_Standby_Check();
	}
	//2.��ҳ���뿪ʼд
	while(no)
	{
		if(no>=I2C_PAGESIZE)
		{
			I2C_EE_WritePage(addr,pBuffer,  I2C_PAGESIZE);
			no-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			pBuffer+=I2C_PAGESIZE;
			printf("д����%s\r\n",pBuffer);
			I2C_Standby_Check();
		}
		else
		{
			I2C_EE_WritePage(addr,pBuffer,  no);
			no=0;
			I2C_Standby_Check();
		}
	}
}

u8 I2c_writeBuf[1000];
u8 I2c_readBuf[1000];

/*
//������
void I2C_Test(void)
{
	u16 i;

	//I2C_Standby_Check();
	
	//��仺��
	for(i=0;i<200;i++)
	{
		 I2c_writeBuf[i]=i+5;
		I2C_EE_WriteByte(i,I2c_writeBuf[i]); 
	}	
//	//д
//	EEPROM_WriteBuffer(0,I2c_writeBuf,100);


	//��
	EEPROM_ReadBuffer(0,I2c_readBuf,200);

	while(1);

	
}
*/

u16 TestTimeCnt = 0;

//������
void I2C_Test(void)
{
	u16 i;
	u16 flg = FALSE;
	while(1)
	{
		for(i=0;i<1000;i++)
		{
			I2C_EE_WriteByte(i,'A'); 
		}	
		EEPROM_ReadBuffer(0,I2c_readBuf,1000);
		for(i=0;i<1000;i++)
		{
			if(I2c_readBuf[i]!='A')
			{
				flg = TRUE;
				break;
			}
		}
		TestTimeCnt++;
		if(flg == TRUE)
		{
			break;
		}
	}
}

u8 ee_Test()
{

  EEPROM_WriteBuffer(0,TEXT_Buffer,5);

	
  //EEPROM_ReadBuffer(0,readbuffer,5);
		
}


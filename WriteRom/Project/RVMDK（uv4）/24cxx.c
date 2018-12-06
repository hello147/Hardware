#include "24cxx.h" 
                           

//init 24cxx
void AT24CXX_Init(void)
{
    IIC_Init();
}
//AT24CXX read one byte data
//return: 0,NO OK  1,OK
//
u8 AT24CXX_ReadOneByte(u16 ReadAddr,u8 *p)
{                  
	u8 x;                                                                                   
	IIC_Start();  
	if(EE_TYPE>AT24C16)//>2047
	{
		x=IIC_Send_Byte(0xa0);      //send write cmd and check
		IIC_Send_Byte(ReadAddr>>8);//addr of high 8-bit       0000 0000  1
	}
	else 
	{
		x=IIC_Send_Byte(0xa0+((ReadAddr>>8)<<1));   //send ic addr 0xa0,writ addr of high 3-bit
	} 
	if(x==0)return x;     
	IIC_Send_Byte(ReadAddr);   //send addr of low 8-bit       
	IIC_Start();              
	IIC_Send_Byte(0xa1);           //send read cmd             
	*p=IIC_Read_Byte(1);           
	IIC_Stop(); 
	return x;
}
//AT24CXX write one byte data
//WriteAddr  :
//return: 0,NO OK  1,OK
u8 AT24CXX_WriteOneByte(u16 WriteAddr,u8 data)
{                                                                                                  
	u8 x;
	IIC_Start(); 
	if(EE_TYPE>AT24C16)//>2047
	{
		x=IIC_Send_Byte(0xa0);      //send write cmd and check
		IIC_Send_Byte(WriteAddr>>8);//addr of high 8-bit       0000 0000  1
	}
	else 
	{
		x=IIC_Send_Byte(0xa0+((WriteAddr>>8)<<1));   //send ic addr 0xa0,writ addr of high 3-bit
	} 
	if(x==0)return x;  
	IIC_Send_Byte(WriteAddr);   //                                                         
	IIC_Send_Byte(data);                                                      
	IIC_Stop();
	delay_us(5000);
	return x;	
}

//AT24CXX write n bytes data
//WriteAddr  :
//return: 0,NO OK  1,OK
u8 AT24CXX_WriteLenByte(u16 WriteAddr,u8 *buf,u16 Len)
{                                                                                                  
	u8 x;
	IIC_Start();  
	if(EE_TYPE>AT24C16)//>2047
	{
		x=IIC_Send_Byte(0xa0);      //send write cmd and check
		IIC_Send_Byte(WriteAddr>>8);//addr of high 8-bit       0000 0000  1
	}
	else 
	{
		x=IIC_Send_Byte(0xa0+((WriteAddr>>8)<<1));   //send ic addr 0xa0,writ addr of high 3-bit
	} 
	if(x==0)return x;  
	IIC_Send_Byte(WriteAddr);   //  
	while(Len--)
	{	
		IIC_Send_Byte(*buf++);   
	}                                               
	IIC_Stop();
	delay_us(5000); 
	return x;
}




//AT24CXX read one byte data
//
//return: 0,NO OK  1,OK

u8  AT24CXX_ReadLenByte(u16 ReadAddr,u8 *buf,u16 Len)
{                  
	u8 x;                                                                                   
	IIC_Start();  
	if(EE_TYPE>AT24C16)//>2047
	{
		x=IIC_Send_Byte(0xa0);      //send write cmd and check
		IIC_Send_Byte(ReadAddr>>8);//addr of high 8-bit       0000 0000  1
	}
	else 
	{
		x=IIC_Send_Byte(0xa0+((ReadAddr>>8)<<1));   //send ic addr 0xa0,writ addr of high 3-bit
	} 
	if(x==0)return x; 
	IIC_Send_Byte(ReadAddr);   //send addr of low 8-bit       
	IIC_Start();              
	IIC_Send_Byte(0xa1);           //send read cmd    
        while(Len)
	{
		if(Len==0)*buf=IIC_Read_Byte(1);  
		else *buf=IIC_Read_Byte(0);  
		buf++;Len--;
	}		
	IIC_Stop();   
	return x;
}




/*********************************************************************
写数据，考虑跨页问题，只需要关心写入起始地址即可
返回：0，操作失败，1操作成功
*********************************************************************/
u8 AT24CXX_PageWrite(u16 WriteAddr, u8 *buf, u16 Len)
{
	u8 wr_page,wr_offset,x;
	wr_offset=I2C_PageSize-WriteAddr%I2C_PageSize;//起始地址所在的页还可以写入数据的个数
	if(Len>wr_offset)//需要跨页写,如果wr_offset为I2C_PageSize，刚好是一个页的开始
	{
		x=AT24CXX_WriteLenByte(WriteAddr, buf, wr_offset);//x=0,操作IIC失败，下面操作没必要了
		if(x)
		{
			Len-=wr_offset;buf+=wr_offset;WriteAddr+=wr_offset;//重新定位
			wr_page=Len/I2C_PageSize;//重新计算要写入的页数量
			wr_offset=Len%I2C_PageSize;//重新计算页偏移量?
			while(wr_page)//写入整页数据
			{
				AT24CXX_WriteLenByte(WriteAddr, buf, I2C_PageSize);
				buf+=I2C_PageSize;
				WriteAddr+=I2C_PageSize;
				wr_page--;
				
			}
			if(wr_offset)//不够一页的数据
			{
				AT24CXX_WriteLenByte(WriteAddr, buf, wr_offset);
			}
				
		}
		
	}
	else//可以在当前起始地址所在的页写入
	{
		x=AT24CXX_WriteLenByte(WriteAddr, buf, Len);
	}
	return x;    
}
//
//
//
//
//
u8 AT24CXX_Check(void)
{
	u8 i,j;u16 x=30003;
	j=AT24CXX_ReadOneByte(x,&i);//    
	if(j==0)return 1;
	if(i==0x55)
	{
		return 0;
	}           
	else
	{
		AT24CXX_WriteOneByte(x,0x55);
		AT24CXX_ReadOneByte(x,&i);      
		if(i==0x55)
			return 0;
	}
	return 1;                                              
}

#ifndef _24CXX_H_
#define _24CXX_H_
#include "iic.h"
#include "delay.h"   

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    	8191
#define AT24C128	16383
#define AT24C256	32767  
//IC tpye check
#define EE_TYPE 	AT24C256
#define I2C_PageSize  	64  //24LC256Ò»Ò³64×Ö½Ú?

					  
u8 	AT24CXX_ReadOneByte(u16 ReadAddr,u8 *p);				
u8 	AT24CXX_WriteOneByte(u16 WriteAddr,u8 data);		
u8 	AT24CXX_WriteLenByte(u16 WriteAddr,u8 *buf,u16 Len);
u8  	AT24CXX_ReadLenByte(u16 ReadAddr,u8 *buf,u16 Len);	
u8 	AT24CXX_PageWrite(u16 WriteAddr, u8 *buf, u16 Len);

u8 	AT24CXX_Check(void); 
void 	AT24CXX_Init(void); 
#endif


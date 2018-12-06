

#include "stm32f10x.h"
#include "esp8266.h"
u8 getTimeFromNTPServer(void)
{
 u8 packetBuffer[48];
  u32 timeOut=0xffffff;
  u8 i;
  u16 year=1900;
  u32 yearSec;
	
	
	
}
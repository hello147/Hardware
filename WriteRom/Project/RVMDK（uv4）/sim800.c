
#include "stm32f10x.h"
#include "sim800.h"
#include "esp8266.h"
 sim800_status_type sim800_status=at;
 struct  STRUCT_USARTx_Fram_sim strsim_Fram_Record = { 0 };
typedef struct 
{
	char * str_cmd;		//发送的命令
	char * str_res;		//期待得到的应答
	u8 result;			//有没有得到想要的应答
} stuType;
stuType stu_at,stu_cpin,stu_cgatt,stu_mode,stu_cstt,stu_ciicr,stu_cipstart,stu_exit,stu_rst;
u8 uart_recv[40];		//串口的接收缓存数组
u8 uart_recv_pos = 0;			//串口缓存当前接收位置


    #define bool _Bool
    #define true 1
    #define false 0
/*******************************************************************************
* 函数名  : UARTx_SendString
* 描述    : USARTx发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void UARTx_SendString(char* s)
{
	while(*s)														//检测字符串结束符
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); //等待上次发送结束
		USART_SendData(USART2, *s++);								//发送字符
	}
}

/*********************************************************************************
 * 函数名：check_cmd
 * 描述  ：sim800发送命令后,检测接收到的应答
 * 输入  ：str:期待的应答结果
 * 输出  ：返回值:0,没有得到期待的应答结果    其它,期待应答结果的位置(str的位置)
 ********************************************************************************/
u8 check_cmd(char *str, char *buf)
{	
	if(strstr(buf, str) == 0)	
		return 0;
	else
		return 1;
}
/*********************************************************************************
 * 函数名：SendCMD
 * 描述  ：发送命令，比较接收的返回
 * 输入  ：无
 * 输出  ：0正常 1无法
*****************************************************************************/
bool sim800_sendcmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strsim_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	sim_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	delay_ms ( waittime );                 //延时
	
	strsim_Fram_Record .Data_RX_BUF [ strsim_Fram_Record .InfBit .FramLength ]  = '\0';
   memset(strsim_Fram_Record .Data_RX_BUF,0,100);
	//macPC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
  
	
 	
	 if ( reply1 != 0 )
		return ( ( bool ) strstr ( strsim_Fram_Record .Data_RX_BUF, reply1 ) );

	
}

void sim800_connetion()
{
   while(ESP8266_Cmd("AT\r\n","OK",NULL,500));
	//while(!sim800_sendcmd("AT+CPIN?","OK",1000));
	//while(!sim800_sendcmd("AT+CSQ","OK",1000));
	while(ESP8266_Cmd("AT+CPIN?\r\n","OK",NULL,500));
	while(ESP8266_Cmd("AT+CGATT?\r\n","OK",NULL,500));
	while(ESP8266_Cmd("AT+CIPMODE=1\r\n","OK",NULL,500));
	while(ESP8266_Cmd("AT+CSTT=\"CMNET\"\r\n","OK",NULL,500));
	while(ESP8266_Cmd("AT+CIPSTART=\"TCP\",\"118.24.157.134\",\"8080\"\r\n","OK",NULL,500));
	  sim800_status=1;
}

//建立sim800透传模式
void attnetwork()
{
  switch(sim800_status)
	{
	  case at:
			 stu_at.result=ESP8266_Cmd("AT\r\n","OK",NULL,500);
	      if(stu_at.result==1)
				{
					sim800_status=cpin;
				}	
				else
				{
					sim800_status=at;
			  }				
		case cpin:
			 stu_cpin.result=ESP8266_Cmd("AT+CPIN?\r\n","OK",NULL,500);
		   if(stu_cpin.result==1)
			 {
			    sim800_status=cgatt;
			 }else
			 {
				 sim800_status=cpin;
			 }
		case cgatt:
			stu_cgatt.result=ESP8266_Cmd("AT+CGATT?\r\n","OK",NULL,500);
			if(stu_cgatt.result==1)
			 {
		    	sim800_status=cipmode;
			 }else
			 {
				  sim800_status=cgatt;
			 }
		case cipmode:
			stu_mode.result=ESP8266_Cmd("AT+CIPMODE=1\r\n","OK",NULL,500);
			if(stu_mode.result==1)
			 {
			 sim800_status=cstt;
			 }else
			 {
				  sim800_status=cipmode;
			 }
		case cstt:
			stu_cstt.result=ESP8266_Cmd("AT+CSTT=\"CMNET\"\r\n","OK",NULL,500);
			if(stu_cstt.result==1)
			 {
			 sim800_status=ciicr;
			 }else
			 {
				 sim800_status=cstt;
			 }
		case ciicr:
			stu_ciicr.result=ESP8266_Cmd("AT+CIICR\"\r\n","OK",NULL,500);
		  if(stu_ciicr.result==1)
			 {
			 sim800_status=cipstart;
			 }else
			 {
				 sim800_status=ciicr;
			 }  
		
		case cipstart:
			 stu_cipstart.result=ESP8266_Cmd("AT+CIPSTART=\"TCP\",\"118.24.157.134\",\"8080\"\r\n","OK",NULL,500);
			if(stu_cipstart.result==1)
			 {
			 sim800_status=10;
				 
			 }else
			 {
				 sim800_status=cipstart;
			 }
			
	}
	
	
}

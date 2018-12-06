
#include "stm32f10x.h"
#include "sim800.h"
#include "esp8266.h"
 sim800_status_type sim800_status=at;
 struct  STRUCT_USARTx_Fram_sim strsim_Fram_Record = { 0 };
typedef struct 
{
	char * str_cmd;		//���͵�����
	char * str_res;		//�ڴ��õ���Ӧ��
	u8 result;			//��û�еõ���Ҫ��Ӧ��
} stuType;
stuType stu_at,stu_cpin,stu_cgatt,stu_mode,stu_cstt,stu_ciicr,stu_cipstart,stu_exit,stu_rst;
u8 uart_recv[40];		//���ڵĽ��ջ�������
u8 uart_recv_pos = 0;			//���ڻ��浱ǰ����λ��


    #define bool _Bool
    #define true 1
    #define false 0
/*******************************************************************************
* ������  : UARTx_SendString
* ����    : USARTx�����ַ���
* ����    : *s�ַ���ָ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void UARTx_SendString(char* s)
{
	while(*s)														//����ַ���������
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); //�ȴ��ϴη��ͽ���
		USART_SendData(USART2, *s++);								//�����ַ�
	}
}

/*********************************************************************************
 * ��������check_cmd
 * ����  ��sim800���������,�����յ���Ӧ��
 * ����  ��str:�ڴ���Ӧ����
 * ���  ������ֵ:0,û�еõ��ڴ���Ӧ����    ����,�ڴ�Ӧ������λ��(str��λ��)
 ********************************************************************************/
u8 check_cmd(char *str, char *buf)
{	
	if(strstr(buf, str) == 0)	
		return 0;
	else
		return 1;
}
/*********************************************************************************
 * ��������SendCMD
 * ����  ����������ȽϽ��յķ���
 * ����  ����
 * ���  ��0���� 1�޷�
*****************************************************************************/
bool sim800_sendcmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strsim_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

	sim_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	delay_ms ( waittime );                 //��ʱ
	
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

//����sim800͸��ģʽ
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

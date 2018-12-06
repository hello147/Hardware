#ifndef __SIM800HTTP_H__
#define __SIM800HTTP_H__	 
#include "stm32f10x.h"


/**************** Ӳ������ ****************/

//ʹ����һ�����ھʹ���һ�����ڣ����Ǵ����жϷ�������Ҫ���ⲿ�ر�
//#define USE_USART1
#define USE_USART2
//#define USE_USART3

//���ڽ���������󳤶�
#define UART_MAX_LEN  2048
//�������õĲ�����
#define BAUD 		  115200
//�������������󳤶�-���ܴ��ڴ�����󳤶�
#define HTTP_RECV_MAX 2048

/**************** Ӳ������ ****************/


typedef enum{
	METHOD_GET = 0,		//HTTP GET
	METHOD_POST,		//HTTP POST
}SIM800_METHOD_TYPE;

typedef enum
{
	UNINIT,				//δ��ʼ��
	
	AT,					//�� SIM800 ģ������
	CPIN,				//��ѯ���� SIM ��
	CGATT,				//GPRS ���ŵ�����
	SAPBR1,				//��ʼ������1
	SAPBR2,				//��ʼ������2

	
	
	CIPMODE,
	CSTT,
	CIPSTART,
	
	ATTACHED,			//����͸��ģʽ�ɹ�

	HTTPINIT,			//http ��ʼ��
	HTTPPARA1,			//http ���� POST ���� GET
	HTTPPARA2,			//���� http url		
	HTTPDATA,			//http ��������д��
	SENDDATA,			//http ��������
	HTTPACTION,			//http ��������
	HTTPREAD,			//http ��ȡ����
	HTTPTERM,			//�ر� HTTP
	
	ERROR_STA			//����״̬
	
}SIM800_STATUE_TYPE;

/********* ������Ա *********/

//ͨ���˱�����ȡSIM800Ŀǰ������״̬
extern u8 sim800sta;

/* ��������: ΪHTTP�����������ʼ������
 * ��������:
	url			http �����ַ
	method		ʹ�õķ���(METHOD_GET �� METHOD_POST)
	buf			POSTʱ������,GETʱ����д""
 * ʾ��:

1.POST

u8 recv[128]; //�����ַ�������

if(sim800sta == ATTACHED) //���ж�Ŀǰ��SIM800״̬Ϊ�Ѹ������粢�ҿ���
{
	sim_write("http://www.baidu.com/hello", METHOD_POST, "test");
}

//��û�з�������,���Բ�����sim_read_string����
if(sim_read_string(recv) > 0) //������ճɹ�
{
	printf("%s", recv); //��ӡ��������
}

2.GET

u8 recv[128]; //�����ַ�������

if(sim800sta == ATTACHED) //���ж�Ŀǰ��SIM800״̬Ϊ�Ѹ������粢�ҿ���
{
	sim_write("http://www.baidu.com/hello", METHOD_GET, "");
}

if(sim_read_string(recv) > 0) //������ճɹ�
{
	printf("%s", recv); //��ӡ��������
}

*/
extern void sim_write(u8* url, u8 method, u8* buf);
/* ��������: ��ʹ��POST�з������ݻ���ʹ��GET�������Ҫʹ�ô˺�����ȡ��������
 * ��������:
	buf		Ҫ���淵�����ݵ�����ͷָ��
 * ʾ��:	�����溯��ʾ��
*/
extern u16 sim_read_string(u8* buf);
/* ��������: ������״̬����ܵ��ƽ�
 * ��������: ��
 * ʾ��:	
 
 �˺���Ӧ�ŵ�main��������ѭ����,����ʹ�ö�ʱ������,ʹ�ö�ʱ������ʱ,�������ִ���굱ǰ
 ����ʱ�ٿ����´��ж�����,��ֹǶ��.
 ��ʱ����������Ƽ�1ms-10ms
 
*/
extern void sim_process(void);

#endif






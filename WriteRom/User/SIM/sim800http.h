#ifndef __SIM800HTTP_H__
#define __SIM800HTTP_H__	 
#include "stm32f10x.h"


/**************** 硬件配置 ****************/

//使用哪一个串口就打开哪一个串口，但是串口中断服务函数需要在外部关闭
//#define USE_USART1
#define USE_USART2
//#define USE_USART3

//串口接收数据最大长度
#define UART_MAX_LEN  2048
//串口设置的波特率
#define BAUD 		  115200
//网络接收数据最大长度-不能大于串口最大长度
#define HTTP_RECV_MAX 2048

/**************** 硬件配置 ****************/


typedef enum{
	METHOD_GET = 0,		//HTTP GET
	METHOD_POST,		//HTTP POST
}SIM800_METHOD_TYPE;

typedef enum
{
	UNINIT,				//未初始化
	
	AT,					//和 SIM800 模块握手
	CPIN,				//查询有无 SIM 卡
	CGATT,				//GPRS 附着到网络
	SAPBR1,				//初始化参数1
	SAPBR2,				//初始化参数2

	
	
	CIPMODE,
	CSTT,
	CIPSTART,
	
	ATTACHED,			//建立透传模式成功

	HTTPINIT,			//http 初始化
	HTTPPARA1,			//http 设置 POST 还是 GET
	HTTPPARA2,			//设置 http url		
	HTTPDATA,			//http 发送数据写入
	SENDDATA,			//http 发送数据
	HTTPACTION,			//http 发送数据
	HTTPREAD,			//http 读取数据
	HTTPTERM,			//关闭 HTTP
	
	ERROR_STA			//错误状态
	
}SIM800_STATUE_TYPE;

/********* 公共成员 *********/

//通过此变量获取SIM800目前的运行状态
extern u8 sim800sta;

/* 函数功能: 为HTTP的事务请求初始化参数
 * 函数参数:
	url			http 请求地址
	method		使用的方法(METHOD_GET 或 METHOD_POST)
	buf			POST时的内容,GET时请填写""
 * 示例:

1.POST

u8 recv[128]; //接收字符串缓存

if(sim800sta == ATTACHED) //先判断目前的SIM800状态为已附着网络并且空闲
{
	sim_write("http://www.baidu.com/hello", METHOD_POST, "test");
}

//若没有返回数据,可以不适用sim_read_string函数
if(sim_read_string(recv) > 0) //如果接收成功
{
	printf("%s", recv); //打印接收数据
}

2.GET

u8 recv[128]; //接收字符串缓存

if(sim800sta == ATTACHED) //先判断目前的SIM800状态为已附着网络并且空闲
{
	sim_write("http://www.baidu.com/hello", METHOD_GET, "");
}

if(sim_read_string(recv) > 0) //如果接收成功
{
	printf("%s", recv); //打印接收数据
}

*/
extern void sim_write(u8* url, u8 method, u8* buf);
/* 函数功能: 当使用POST有返回数据或者使用GET请求后需要使用此函数读取返回数据
 * 函数参数:
	buf		要保存返回数据的数组头指针
 * 示例:	见上面函数示例
*/
extern u16 sim_read_string(u8* buf);
/* 函数功能: 做整个状态机框架的推进
 * 函数参数: 无
 * 示例:	
 
 此函数应放到main函数的主循环中,或者使用定时器触发,使用定时器触发时,请务必在执行完当前
 过程时再开启下次中断请求,防止嵌套.
 定时器正常间隔推荐1ms-10ms
 
*/
extern void sim_process(void);

#endif






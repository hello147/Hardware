#include "sim800http.h"
#include <stdlib.h>
#include "string.h" 


#ifdef USE_USART1
	#define USARTx 			USART1
	#define UASRTx_RCC 		RCC_APB2Periph_USART1
	#define RCC_Periph		RCC_APB2PeriphClockCmd
	#define USARTx_IRQn		USART1_IRQn
#endif
#ifdef USE_USART2
	#define USARTx 			USART2
	#define UASRTx_RCC 		RCC_APB1Periph_USART2
	#define RCC_Periph		RCC_APB1PeriphClockCmd
	#define USARTx_IRQn		USART2_IRQn
#endif
#ifdef USE_USART3
	#define USARTx 			USART3
	#define UASRTx_RCC 		RCC_APB1Periph_USART3
	#define RCC_Periph		RCC_APB1PeriphClockCmd
	#define USARTx_IRQn		USART3_IRQn
#endif



//设置 SIM800 的状态
SIM800_STATUE_TYPE SIM800_STATUE = UNINIT;

//创建结构体给 SIM800 指令使用
typedef struct 
{
	char * str_cmd;		//发送的命令
	char * str_res;		//期待得到的应答
	u8 result;			//有没有得到想要的应答
} stuType;

//实例化 SIM800 的指令结构体
stuType stu_AT, stu_RST, stu_CPIN, stu_CGATT, stu_SAPBR1, stu_SAPBR2, stu_SAPBR3;//发送的命令种类
stuType stu_HTTPINIT, stu_HTTPPARA1, stu_HTTPPARA2, stu_HTTPACTION, stu_HTTPTERM, stu_HTTPDATA, stu_SENDDATA, stu_HTTPREAD;
stuType  stu_CIPMODE,stu_CSTT,stu_CIPSTART;
//设置 SIM800 的方法
SIM800_METHOD_TYPE SIM800_METHOD = METHOD_POST;

/************** 私有成员 **************/
u8 sim800sta = UNINIT;
char SIM_URL[128]; 		//用于存放配置的网址
char httpurl[156]; 		//用于拼接网址字符串
char httpact[24];		//用于存放POST还是GET命令串
char httpdata[48]; 		//用于拼接httpdata指令数据		
extern char senddata[256];		//用于存放要发送的数据

u8 uart_recv[UART_MAX_LEN];		//串口的接收缓存数组
u8 uart_recv_pos = 0;			//串口缓存当前接收位置
char httprecv[HTTP_RECV_MAX];	//用于存储从SIM800读取的消息
u16 httprecvlen = 0;			//记录SIM800读取信息的长度

void uart_process(void);
void uartx_irq(void);
void HTTP_PROCESS(void);
void AttachToNetwork(void);
void init_struct(void);

//public function
void sim_write(u8* url, u8 method, u8* buf);
u16 sim_read_string(u8* buf);
void sim_process(void);

/*********************************************************************************
/*********************************************************************************
 * 函数名： sim_write
 * 描述  ： 为HTTP的事务请求初始化参数
 * 输入  ： url		http 请求地址字符串的指针
			method	使用的方法(METHOD_GET 或 METHOD_POST)
			buf		POST时内容的指针,GET时请填写""
 * 输出  ： 
 ********************************************************************************/
void sim_write(u8* url, u8 method, u8* buf)
{
	u8 str[24];	//做临时数据存放
	
	SIM800_METHOD = method;
	memset(SIM_URL, 0, 256);
	strcpy(SIM_URL, url);
	
	init_struct();										//初始化结构体
	
	//修改 HTTP URL 字符串
	strcpy(httpurl, stu_HTTPPARA2.str_cmd);				//把结构体里面初始内容赋值给httpurl
	strcat(httpurl, SIM_URL);							//拼接
	strcat(httpurl, "\"\r\n");							//拼接上右双引号
	stu_HTTPPARA2.str_cmd = httpurl;					//将拼接完的字符串给结构体

	//修改 HTTP ACTION 字符串
	strcpy(httpact, stu_HTTPACTION.str_cmd);			//把结构体里面初始内容赋值给httpurl
	strcat(httpact, "0\r\n");							//拼接默认的为0-GET请求

	
	if(SIM800_METHOD == METHOD_POST)
	{
		//修改 HTTP 数据长度
		strcpy(httpdata, stu_HTTPDATA.str_cmd);			//把结构体里面初始内容赋值给httpdata
		sprintf((char *)str, "%d", strlen((char *)buf));//把数字转换为字符串
		strcat((char *)httpdata, (char *)str);
		strcat((char *)httpdata, ",10000\r\n");			//拼接
		stu_HTTPDATA.str_cmd = httpdata;				//将拼接完的字符串给结构体
		
		//补全 HTTP 发送数据
		strcpy((char *)senddata, (char *)buf);
		stu_SENDDATA.str_cmd = senddata;				//赋值全局变量senddata地址给结构体成员
		
		httpact[14] = '1';								//修改为 HTTPACTION=1
	}
	
	stu_HTTPACTION.str_cmd = httpact;					//将拼接完的HTTPACTION给结构体
	
	//清空接收数据
	//memset(httprecv, 0, strlen(httprecv));
	httprecvlen = 0;
	
	if(SIM800_STATUE >= ATTACHED)						//如果SIM800已经联网
	{
		SIM800_STATUE = HTTPINIT;						//修改目前状态为HTTPINIT，进入发送流程
	}
}

/*********************************************************************************
 * 函数名： sim_read_string
 * 描述  ： 当使用POST有返回数据或者使用GET请求后需要使用此函数读取返回数据
 * 输入  ： buf 用于存放接收数据的数组指针 
 * 输出  ： 
 ********************************************************************************/
u16 sim_read_string(u8* buf)
{
	u16 temp = 0;
	
	if(httprecvlen > 0)					//如果当前httprecv的长度>0,即证明当前有接收数据
	{
		strcpy(buf, httprecv);			//将httprecv的内容复制给buf
		temp = httprecvlen;				//暂时记录接收数据长度
		httprecvlen = 0;				//清空httprecvlen
	}
	return temp;
}


/*********************************************************************************
 * 函数名： init_struct
 * 描述  ： 为结构体赋值
 * 输入  ： 
 * 输出  ： 
 ********************************************************************************/
void init_struct(void)
{	
	stu_AT.str_cmd = "AT\r\n";											//进行AT握手
	stu_AT.str_res = "OK";
	stu_AT.result = 0;
	
	stu_CPIN.str_cmd = "AT+CPIN?\r\n";									//检测是否有SIM卡
	stu_CPIN.str_res = "OK";
	stu_CPIN.result = 0;
	
	stu_CGATT.str_cmd = "AT+CGATT?\r\n";								//检查当前GATT网络状态
	stu_CGATT.str_res = "OK";
	stu_CGATT.result = 0;
	
	stu_SAPBR1.str_cmd = "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n";		//设置GPRS接入
	stu_SAPBR1.str_res = "OK";
	stu_SAPBR1.result = 0;
	
	stu_SAPBR2.str_cmd = "AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n";			//设置CMNET为网络接入点
	stu_SAPBR2.str_res = "OK";
	stu_SAPBR2.result = 0;
	
	stu_CIPMODE.str_cmd="AT+CIPMODE=1\r\n";
  stu_CIPMODE.str_res="OK";
	stu_CIPMODE.result=0;
	
	stu_CSTT.str_cmd="AT+CSTT=\"CMNET\"\r\n";
	stu_CSTT.str_res="OK";
	stu_CSTT.result=0;
	
	stu_CIPSTART.str_cmd="AT+CIPSTART=\"TCP\",\"118.24.157.134\",\"8080\"\r\n";
  stu_CIPSTART.str_res="OK";
	stu_CIPSTART.result=0;
}

/********************************************************************************************
 * 函数名：uartx_config
 * 描述  ：对使用到的USART进行初始化
 * 输入  ：bound:波特率
 * 输出  : 
 * 调用  ：
 ********************************************************************************************/
void uartx_config(u32 bound)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_Periph(UASRTx_RCC, ENABLE);									//开启对应串口的时钟
	
	USART_DeInit(USARTx);											//禁能相应串口后才可以对相应串口进行配置
	
#ifdef USE_USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			
	
	//USART1_TX   PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    						//配置相应GPIO为推挽输出
	
	//USART1_RX   PA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//配置对应GPIO为浮空输入
#endif
#ifdef USE_USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//USART2_TX   PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef USE_USART3
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

	USART_InitStructure.USART_BaudRate = bound;						//配置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//设置字宽为8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//设置一停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//设置无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置为无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//设置为收发模式
  
	USART_Init(USARTx, &USART_InitStructure);						//初始化串口
  
	USART_Cmd(USARTx, ENABLE);                						//使能串口  

	//使能接收中断
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);					//开启中断   	
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;				//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);									//根据指定的参数初始化VIC寄存器		
}

/***************************************************************************************
 * 函数名：USARTx_IRQHandler
 * 描述  ：串口x中断服务程序
 * 输入  ：
 * 输出  ：
 * 调用  ：
 ***************************************************************************************/
#ifdef USE_USART1
void USART1_IRQHandler(void)
{	
	uartx_irq();
}
#endif
#ifdef USE_USART2
void USART2_IRQHandler(void)
{	
	uartx_irq();
}
#endif
#ifdef USE_USART3
void USART3_IRQHandler(void)
{	
	uartx_irq();
}
#endif

/*********************************************************************************
 * 函数名：uartx_irq
 * 描述  ：串口的中断函数
 * 输入  ：
 * 输出  ：
 ********************************************************************************/
void uartx_irq(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)			//如果是串口的接收中断
	{
		Res = USART_ReceiveData(USARTx);							//接收一个字节数据
		
		if(uart_recv_pos < UART_MAX_LEN)							//如果接收数据没有超长
		{
			uart_recv[uart_recv_pos++] = Res;						//向接受数组中追加信息
		}
		else
		{
			//error:接收超时！
			memset(uart_recv, 0, UART_MAX_LEN);						//超长,清除接收数组
			uart_recv_pos = 0;										
		}
    } 
} 

		
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
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET); //等待上次发送结束
		USART_SendData(USARTx, *s++);								//发送字符
	}
}

/*******************************************************************************
* 函数名  : UARTx_SendString
* 描述    : USARTx发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void UARTx_SendData(u8 dat)
{
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待上次发送结束
	USART_SendData(USARTx, dat);									//发送当前字符
}

/*********************************************************************************
 * 函数名：check_cmd
 * 描述  ：sim800发送命令后,检测接收到的应答
 * 输入  ：str:期待的应答结果
 * 输出  ：返回值:0,没有得到期待的应答结果    其它,期待应答结果的位置(str的位置)
 ********************************************************************************/
u8 check_cmd(char *str, char *buf)
{	
	if(strstr(buf, str) == NULL)	
		return 0;
	else
		return 1;
}

/*********************************************************************************
 * 函数名：SendCMD
 * 描述  ：发送命令，比较接收的返回
 * 输入  ：无
 * 输出  ：0正常 1无法
 ********************************************************************************/
u8 SendCMD(char *cmd, char *str, u16 waittime)
{
	u8 result = 0;
	UARTx_SendString(cmd);								//发送相关命令
	memset(uart_recv, 0, sizeof(uart_recv));			//清空接收缓存数组
	uart_recv_pos = 0;	
	while(waittime--)									//等待倒计时
	{	
		delay_ms(10);
		if(check_cmd(str, (char *)uart_recv))			//检查接收字符串中是否有关键字
		{
			result = 1;
			return 1;
		} 												//接收到期待的应答结果			 
		if(waittime <= 1) 
		{		
			result = 0;									//没有得到想要的应答
//			for(i=0;i<uart_recv_pos;i++)	printf("%c",USART2_RX_BUF[i]);
		}	
	}
	
	return result;
}

/*********************************************************************************
 * 函数名： sim_process
 * 描述  ： 做整个状态机框架的推进
 * 输入  ：  
 * 输出  ： 
 ********************************************************************************/
void sim_process(void)
{
	
	if(SIM800_STATUE < ATTACHED)							//如果SIM800的状态还未附着网络
	{
		switch(SIM800_STATUE)
		{
			case UNINIT:									//未初始化外设
				//uartx_config(BAUD);							//初始化串口
			  USART2_Config();                       //初始化串口2
				SIM800_STATUE = AT;							//进入AT状态
			    init_struct();								//初始化结构体
			case AT:
			case CPIN:
			case CGATT:
			case SAPBR1:
			case SAPBR2:	
      case 	CIPMODE:			//在附着网络过程中
			case CIPSTART:
				AttachToNetwork();
			break;
		}
	}

	sim800sta = SIM800_STATUE;
}

/*********************************************************************************
 * 函数名： AttachToNetwork
 * 描述  ： 负责SIM800附着网络过程的处理
 * 输入  ： 
 * 输出  ： 
 ********************************************************************************/
void AttachToNetwork(void)
{	
	switch (SIM800_STATUE)
	{
		case AT:											//AT指令用于与SIM800进行握手,可以检测串口与SIM800是否可以通信
			stu_AT.result = SendCMD(stu_AT.str_cmd, stu_AT.str_res, 500);
			if(stu_AT.result == 1)
			{				
				SIM800_STATUE = CPIN;						//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = AT;							//没有得到想要的继续发送				
			}
			break;
		
		case CPIN:
			stu_CPIN.result = SendCMD(stu_CPIN.str_cmd, stu_CPIN.str_res, 500);
			if(stu_CPIN.result == 1)
			{
				SIM800_STATUE = CGATT;						//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = CPIN;						//没有得到想要的继续发送
			}
			break;
			
		case CGATT:
			stu_CGATT.result = SendCMD(stu_CGATT.str_cmd, stu_CGATT.str_res, 500);
			if(stu_CGATT.result == 1)
			{
				SIM800_STATUE = SAPBR1;						//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = CGATT;						//没有得到想要的继续发送
			}
			break;
			
		case SAPBR1:
			stu_SAPBR1.result = SendCMD(stu_SAPBR1.str_cmd, stu_SAPBR1.str_res, 500);
			if(stu_SAPBR1.result == 1)
			{
				SIM800_STATUE = SAPBR2;						//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = SAPBR1;						//没有得到想要的继续发送
			}
			break;
			
		case SAPBR2:
			stu_SAPBR2.result = SendCMD(stu_SAPBR2.str_cmd, stu_SAPBR2.str_res, 500);
			if(stu_SAPBR2.result == 1)
			{
				SIM800_STATUE = CIPMODE;						//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = SAPBR2;						//没有得到想要的继续发送
			}
			break;
			
		case CIPMODE:
			stu_CIPMODE.result = SendCMD(stu_CIPMODE.str_cmd, stu_CIPMODE.str_res, 500);
			if(stu_CIPMODE.result == 1)
			{
				SIM800_STATUE = CIPSTART;					//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = CIPMODE;						//得到想要的应答发送下一条指令
			}
			break;
		case CIPSTART:
				stu_CIPSTART.result=SendCMD(stu_CIPSTART.str_cmd, stu_CIPSTART.str_res, 500);
		  if(stu_CIPSTART.result==1)
			{
			SIM800_STATUE=ATTACHED;		
			}
			else
			{
			SIM800_STATUE=CIPSTART;
			}

	}	
}

/*********************************************************************************
 * 函数名： getNumber
 * 描述  ： 从字符串中将数字转化出来,未做错误处理,转换数字前需要确保字符串里面是数字
 * 输入  ： str,字符串指针
			len,字符串中数字长度
 * 输出  ： 无符号16位的整数数字
 ********************************************************************************/
u16 getNumber(u8* str, u8 len)
{
	u16 temp = 0;
	u8 i = 0;
	for(;i<len;i++)								//循环len次
	{
		temp = temp*10 + str[i]-0x30;			//每次都从str数组中拿一个数字与上次结果x10相加
	}
	
	return temp;
}

/*********************************************************************************
 * 函数名： HTTP_PROCESS
 * 描述  ： 负责SIM800发送进程的处理
 * 输入  ：  
 * 输出  ： 
 ********************************************************************************/
void HTTP_PROCESS(void)
{
	switch (SIM800_STATUE)
	{
		//每个状态的作用请参照SIM800_STATUE_TYPE定义中的注释
		case HTTPINIT:
			stu_HTTPINIT.result = SendCMD(stu_HTTPINIT.str_cmd, stu_HTTPINIT.str_res, 500);
			if(stu_HTTPINIT.result == 1)
			{
				SIM800_STATUE = HTTPPARA1;						//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = HTTPTERM;							//没有得到想要的继续发送
			}
			break;
			
		case HTTPPARA1:
			stu_HTTPPARA1.result = SendCMD(stu_HTTPPARA1.str_cmd, stu_HTTPPARA1.str_res, 500);
			if(stu_HTTPPARA1.result == 1)
			{
				SIM800_STATUE = HTTPPARA2;							//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = HTTPPARA1;							//没有得到想要的继续发送
			}
			break;
			
		case HTTPPARA2:
			stu_HTTPPARA2.result = SendCMD(stu_HTTPPARA2.str_cmd, stu_HTTPPARA2.str_res, 500);
			if(stu_HTTPPARA2.result == 1)
			{
				//根据目前使用的是POST还是GET方法,进入对应的过程当中
				SIM800_STATUE = (SIM800_METHOD == METHOD_POST?HTTPDATA:HTTPACTION);
			}
			else
			{
				SIM800_STATUE = HTTPPARA2;							//没有得到想要的继续发送
			}
			break;
			
		case HTTPDATA:
			stu_HTTPDATA.result = SendCMD(stu_HTTPDATA.str_cmd, stu_HTTPDATA.str_res, 500);
			if(stu_HTTPDATA.result == 1)
			{
				SIM800_STATUE = SENDDATA;							//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = HTTPDATA;							//没有得到想要的继续发送
			}
			break;
			
		case SENDDATA:
			stu_SENDDATA.result = SendCMD(stu_SENDDATA.str_cmd, stu_SENDDATA.str_res, 500);
			if(stu_SENDDATA.result == 1)
			{
				SIM800_STATUE = HTTPACTION;							//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = HTTPDATA;							//没有得到想要的继续发送
			}
			break;
			
		case HTTPACTION:
			stu_HTTPACTION.result = SendCMD(stu_HTTPACTION.str_cmd, stu_HTTPACTION.str_res, 500);
			if(stu_HTTPACTION.result == 1)
			{
				SIM800_STATUE = HTTPREAD;							//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = HTTPACTION;							//没有得到想要的继续发送
			}
			break;
			
		case HTTPREAD:
			stu_HTTPREAD.result = SendCMD(stu_HTTPREAD.str_cmd, stu_HTTPREAD.str_res, 500);
			if(stu_HTTPREAD.result == 1)
			{
				u8 *found;
				u8 numlen;
				u16 datalen;
				//接下来要对返回数据中的字符串进行截取
				SIM800_STATUE = HTTPTERM;
				found = strstr(uart_recv, "+HTTPREAD: ");			//在串口接收数据中搜索关键字的位置并存储在found中
				if(found != NULL)									//如果找到了关键字的位置
				{
					found += 11;									//调整found指针向后推移关键字长度个位置,其实就是将指针指向存储有数据长度的位置
					numlen = strstr(found, "\r\n") - found;			//数字的长度很明显就是从目前为止开始到下一个/r/n出现的位置之差
					datalen = getNumber(found, numlen);		 		//通过getNumber将字符串形式的数字转换为数字,得到了真正数据的长度
					
					memcpy(httprecv, found+numlen+2, datalen);		//使用memcpy将指定位置开始的字符串复制指定长度到指定数组中
					httprecvlen = datalen;							//将数据长度保存到httprecvlen中
				}
			}
			else
			{
				SIM800_STATUE = HTTPREAD;							//没有得到想要的继续发送
			}
			break;
			
		case HTTPTERM:
			stu_HTTPTERM.result = SendCMD(stu_HTTPTERM.str_cmd, stu_HTTPTERM.str_res, 500);
			if(stu_HTTPTERM.result == 1)
			{
				SIM800_STATUE = ATTACHED;							//得到想要的应答发送下一条指令
			}
			else
			{
				SIM800_STATUE = ATTACHED;							//没有得到想要的继续发送
			}
			break;
	}
	
}


/************************************* END OF FILE **************************************/

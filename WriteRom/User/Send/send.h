#ifndef  __SEND_H
#define	 __SEND_H



#include "stm32f10x.h"



/********************************** 用户需要设置的参数**********************************/
//要连接的热点的名称，即WIFI名称
#define      macUser_ESP8266_ApSsid           "1234" 
//#define      macUser_ESP8266_ApSsid           "Xiaomi_V4S48" 
//要连接的热点的密钥
#define      macUser_ESP8266_ApPwd            "12345678" 

//要连接的服务器的 IP，即电脑的IP 
#define      macUser_ESP8266_TcpServer_IP     "192.168.0.102" 
//#define      macUser_ESP8266_TcpServer_IP     "10.0.2.12" 
//要连接的服务器的端口
#define      macUser_ESP8266_TcpServer_Port    "80"  

/********************************** 外部全局变量 ***************************************/
extern volatile uint8_t ucTcpClosedFlag;
/********************************** 测试函数声明 ***************************************/
void                     ESP8266_StaTcpClient_UnvarnishTest  ( void );



#endif



本例程演示如何使用串口调试助手测试WF-ESP8266 WiFi模块

通过串口调试助手配置模块，并与外界进行数据传输

ESP8266 作为 STA TCP Client 向网络助手上传 DHT11 温湿度



AT+GMRAT version:0.22.0.0(Mar 20 2015 10:04:26)
SDK version:1.0.0
compile time:Mar 20 2015 11:00:32

OK

引脚定义：
---ESP---
TX-PB10
RX-PB11
RST-PB9
I/O-PB8

---串口---
USARTX---USART1:PA9、PA10
USART3---PB10、PB11

---传感器---
DHT11-DATA：PC6
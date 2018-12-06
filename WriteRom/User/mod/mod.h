
#include "stm32f10x_gpio.h"
#define RT_485_PIN                    GPIO_Pin_15        
#define RT_485_GPIO_PORT              GPIOA  

#define RT_485_R   GPIO_ResetBits(RT_485_GPIO_PORT, RT_485_PIN)
#define RT_485_T  GPIO_SetBits(RT_485_GPIO_PORT, RT_485_PIN)

#define      modUSART_BAUD_RATE                        9600
#define      modUSART_APBxClock_FUN                    RCC_APB2PeriphClockCmd
#define      modUSART_CLK                              RCC_APB2Periph_USART1
#define      modUSART_GPIO_APBxClock_FUN               RCC_APB2PeriphClockCmd
#define      modUSART_GPIO_CLK                         RCC_APB2Periph_GPIOA 
#define      modUSARTx                                USART1
#define      modUSART_TX_PORT                         GPIOA   
#define      modUSART_TX_PIN                          GPIO_Pin_9
#define      modUSART_RX_PORT                         GPIOA
#define      modUSART_RX_PIN                          GPIO_Pin_10
#define      modUSART_IRQ                             USART1_IRQn
#define      modUSART_INT_FUN                         USART1_IRQHandler

void Mod_USARTx_Config(void);

typedef struct
{
	 char rx_buf[255];//接收的缓冲区
	unsigned char tx_buf[255];//要发送的缓冲区

	unsigned int  tx_len;//需要发送的数据长度
	unsigned int  rx_len;//需要接收的数据长度
	unsigned int  tx_len_ed;//已发送长度
	unsigned int  rx_len_ed;//已接收长度
  unsigned int   rx_time_outn;   //接收超时
	unsigned int   rev_time_outcnt;  //回复超时
  unsigned char   deal_flag;  //1:发送   2：接收     3:处理接收的数据
	unsigned char   rec_over;  //1：一帧接收结束   2：没结束
	unsigned int t_time_delay;
	  unsigned int  r_time_delay;
	unsigned char error;
}usart_st;

typedef struct
{
	unsigned char dev_add;  //从机地址
	unsigned char fun;      //功能码
	unsigned short int addr;  //通讯地址
	unsigned short int len;   //要读取的数据长度
	unsigned char rev_time_outcnt;//接收时间
	unsigned char deal;     //1代表正在等待回复，0代表不等待
}modbus_rtust;

unsigned char com_485_send(unsigned int l);
unsigned char check_485_sending(void);
signed char read_input_register_03(unsigned char dev_add,unsigned short int addr,unsigned short int len);
signed char read_input_register_04(unsigned char dev_add,unsigned short int addr,unsigned short int len);
signed char read_discrete_register_02(unsigned char dev_add,unsigned short int addr,unsigned short int len);
signed char set_hold_register_06(unsigned char dev_add,unsigned short int addr,unsigned short int val);
signed char set_hold_registers_16(unsigned char dev_add,unsigned short int addr,unsigned short int len);
uint16_t Modbus_CRC16( uint8_t *data_buf,uint16_t data_len); 
u16 mc_check_crc16(u8 *buf,u16 len);
void claer();










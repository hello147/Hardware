
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
	 char rx_buf[255];//���յĻ�����
	unsigned char tx_buf[255];//Ҫ���͵Ļ�����

	unsigned int  tx_len;//��Ҫ���͵����ݳ���
	unsigned int  rx_len;//��Ҫ���յ����ݳ���
	unsigned int  tx_len_ed;//�ѷ��ͳ���
	unsigned int  rx_len_ed;//�ѽ��ճ���
  unsigned int   rx_time_outn;   //���ճ�ʱ
	unsigned int   rev_time_outcnt;  //�ظ���ʱ
  unsigned char   deal_flag;  //1:����   2������     3:������յ�����
	unsigned char   rec_over;  //1��һ֡���ս���   2��û����
	unsigned int t_time_delay;
	  unsigned int  r_time_delay;
	unsigned char error;
}usart_st;

typedef struct
{
	unsigned char dev_add;  //�ӻ���ַ
	unsigned char fun;      //������
	unsigned short int addr;  //ͨѶ��ַ
	unsigned short int len;   //Ҫ��ȡ�����ݳ���
	unsigned char rev_time_outcnt;//����ʱ��
	unsigned char deal;     //1�������ڵȴ��ظ���0�����ȴ�
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










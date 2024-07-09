#ifndef __RS485_H
#define __RS485_H			

#include "sys.h"	 								  


/* ����������ã�����������ڸ��������⼸��� */
#define	RS485_PROT		GPIOA					//���ڶ˿�
#define RS485_GPIO_RCC	RCC_APB2Periph_GPIOA	//���ڶ˿�GPIOʱ��
#define RS485_TX_Pin	GPIO_Pin_2				//����TXD����
#define RS485_RX_Pin	GPIO_Pin_3				//����RXD����
#define RS485_USART_RCC	RCC_APB1Periph_USART2	//���ڹ���ʱ��
#define RS485_USART_NUM	USART2					//����ͨ��

/* 485ģʽ������������ ����������������⼸��� */
#define	RS485_MODE_PROT	GPIOA					//RS485ģʽ���ƶ˿�
#define RS485_MODE_Pin	GPIO_Pin_1				//RS485ģʽ��������
#define RS485_MODE_RCC	RCC_APB2Periph_GPIOA	//RS485ģʽ���ƶ˿�GPIOʱ��
#define RS485_TX_EN		PAout(1)				//485ģʽ����.0,����;1,����

/* ����봮���жϽ��գ��벻Ҫע�����º궨�� */
#define EN_USART2_RX 	1						//0,������;1,����.


extern u8 RS485Busy ;

void RS485_Init(u32 bound);
void RS485_RW_Opr(u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate);
void UartDriver(void);
void UartRxMonitor(u8 ms);


#endif	   

















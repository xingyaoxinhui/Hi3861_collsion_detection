#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "Delay.h"
#include <stdarg.h>
#include "Serial.h"

uint8_t Serial_RxPacket[8];       //8���ֽ�
uint8_t Serial_RxFlag;            //������ɵı�ʶ��

void Serial_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //ʹ��USART1ʱ��
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      //�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;            //485-TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ1
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ģʽ������/����
	USART_Init(USART1, &USART_InitStructure);       //��ʼ��USART1
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte)                        //���͵����ֽ�
{                                        
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	delay_ms(2);
	                                       
}
 
void Serial_SendArray(uint8_t *Array, uint16_t Length)    //��������ʽ�ֽ�
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}

}
void Serial_SendString(char *string)
{
	uint8_t i;
	for(i=0;string[i]!='\0';i++)
	   Serial_SendByte(string[i]);
}

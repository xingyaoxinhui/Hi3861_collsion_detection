#include "led.h"		
#include "key.h"
#include "delay.h"		
#include "sys.h"
#include "timer.h"
#include "rs485.h"
#include "demo.h"
#include "OLED.h"
extern uint8_t uart_data[7];
uint8_t str[20]={0};
int main(void)
{ 
	delay_init();	    							//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//����ϵͳ�ж����ȼ�����4	 
	LED_Init();		  								//��ʼ��LED
	KeyInit();										//��ʼ������
	RS485_Init(9600);								//��ʼ��485���� ������Ϊ9600
	TIMER3_Init(999,71);							//��ʱ��3��ʼ�� ����1ms
	OLED_Init();
	Serial_Init();
	while(1)
	{
		UartDriver();								
		RS485_RW_Opr(0x01,0x04,0x01A6,0x0001);
		sprintf((unsigned char*)str,"%d%d    ",uart_data[3],uart_data[4]);
		OLED_ShowString(2, 1,str);
		 Serial_SendString(str);
		delay_ms(100);
		
		
	}
}

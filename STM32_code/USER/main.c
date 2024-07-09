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
	delay_init();	    							//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//设置系统中断优先级分组4	 
	LED_Init();		  								//初始化LED
	KeyInit();										//初始化按键
	RS485_Init(9600);								//初始化485串口 波物率为9600
	TIMER3_Init(999,71);							//定时器3初始化 周期1ms
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

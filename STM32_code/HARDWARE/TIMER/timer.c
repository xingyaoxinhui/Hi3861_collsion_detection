/****************************************************************************************************************
@�ļ����ƣ�timer.c
@����˵������ʼ����ʱ��3 ����ʱ��3�жϷ����� ����ʱ��
@��    ��:V1.0.0
@��Ȩ����:
@��    ��:2017-9-26
@��    д:
@�޸���־:
****************************************************************************************************************/
#include "timer.h"
#include "rs485.h"
#include "led.h"

/***************************************************************************************************************
@�������ƣ���ʱ��3��ʼ�����ú���
@��ڲ�����arr->�Զ���װֵ
		   psc->ʱ�ӳ��� Ԥ��Ƶֵ
@���ڲ�������
@����˵������ʼ����ʱ��3
***************************************************************************************************************/
void TIMER3_Init(unsigned int arr,unsigned int psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//--------------ʹ��TIM3ʱ�� 

	TIM_TimeBaseStructure.TIM_Period = arr;//---------------------------�Զ���װ�Ĵ�����ֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc;//-------------------------ʱ�ӳ���Ԥ��Ƶ��ֵ 	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//----------------------ʱ�ӷָ� 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//-------���ϼ���ģʽ 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//-------------------��ʼTIM3ʱ�� 
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );//-------------------------ʹ��TIME3�ж� ��������ж� 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//-------------------�ж�ѡ��TIM3 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//---------ռ�����ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//----------------�����ȼ�3 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//-------------------IRQͨ��ʹ�� 
	NVIC_Init(&NVIC_InitStructure);//-----------------------------------��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);//--------------------------------------------ʹ��TIM3����
}
/***************************************************************************************************************
@�������ƣ���ʱ��3�жϴ�����
@��ڲ�������
@���ڲ�������
@����˵����ÿ�ν����ж���λʱ����־ ͬʱ��תLED1״̬ ������ʾ������״̬
***************************************************************************************************************/
void TIM3_IRQHandler(void)  
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//----------------����ж�Դ�Ƿ�ΪTIM3
	{
		UartRxMonitor(1);
		LedPoint(500);//-----------------------------------------------LEDָʾ��������˸
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//------------------���TIM3�ж�
	}
}

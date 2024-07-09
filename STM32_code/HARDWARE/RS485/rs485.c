/*************************************************************************************
�ļ����ƣ�RS485.c
��    ����V1.0
��    �ڣ�2020-5-11
��    ����Eric Xie
˵    ���������������
�޸���־��

modbusЭ�������ʽ˵����
	1.�����Դӻ�д���ݲ������ӻ����յ����ĺ�Ա��Ľ��н�����Ȼ��ִ����Ӧ�Ĵ���ͬʱ��Ҫ������Ӧ��
	  ΪʲôҪӦ����Ϊ������Ҫȷ�ϴӻ��Ƿ��յ����ģ�Ȼ�����ӻ��������������ݣ�ִ�������Ĳ�����
	  ��Щ�����ǻ�����һ�β����������ģ�������ҪӦ���Ա�֤ϵͳ�Ľ�׳�Ժ��ȶ��ԡ���ȻҲ�ɲ����
	  
	����������ӻ�����һ�����ĸ�ʽ����(16����)
		01		  06	 00 01	   00 17	 98 04
	  �ӻ���ַ	���ܺ�	���ݵ�ַ	����	CRCУ����	
	�⣺�������ַΪ01�Ĵӻ� �Ĵ�����ַΪ0001�ĵط� д������0017
	    �ӻ��ɰ���������ԭ��������ΪӦ�������յ����ʾд���ݳɹ�����ִ����һ������
	
	2.�����Դӻ����ж����ݲ������ӻ����յ����ĺ���н�����Ȼ����ݱ��ĵ����ݣ�����Ҫ���ظ���������
	  �ݷ��ظ����������صı��ļ�ͬ����Ӧ��
	
	����������ӻ�����һ�����ĸ�ʽ����(16����)
		 01		  03	 00 01		   00 01		 d5 ca
	  �ӻ���ַ	���ܺ�	���ݵ�ַ	��ȡ���ݸ���	CRCУ����
	�⣺����ӻ��Ĵ�����0001�ĵط���ŵ�����Ϊaa����ô���ظ�����������Ϊ 01 03 02 00 aa 38 3b
		�����յ�����������Ľ�����Ѷ��������ݱ�����ָ���ı����м��ɡ�
		
��ע˵���������ǻ���ͨѶ��ʽ����Щ���ݿ�ͨ��������ʽ�߼�ʵ�ָ���Ĺ��ܣ������������о�

**************************************************************************************/   
#include "rs485.h"	 
#include "delay.h"
#include "crc16.h"
#include "usart.h"

/* �����жϽ��յ����ݵĳ��ȱ��� */
static u8 USART_RX_CNT = 0;
/* �����жϽ��յ����ݱ���Ļ����� */
static u8 USART2_RX_BUF[64];
/* ���ڱ�ʶ���ڽ������ݰ��Ƿ���ɱ�־ */
static u8 From_Flag = 0;
/* ͨѶ��־ �����������ݺ���1 ���յ�Ӧ������� */
u8 RS485Busy = 0;
/* ���ջ����� */
u8 RS485_RX_BUF[64];  	//���ջ���,���64���ֽ�.
/* ���ڱ���������õ����� */
u16 ReadDateVal = 0;

/***************************************************************************************************
 * ����˵��: 485���ڳ�ʼ��
 * ��ڲ�����u32 bound
 * ���ڲ�����
 * �������ܣ���ʼ������2 ����485ͨѶ 
 *			 ������Ҫ�ʹӻ��Ĳ�����һ��
 *			 �������ÿ���rs485.h�ǲ鿴���� ��ӦӲ��
***************************************************************************************************/ 
void RS485_Init(u32 bound)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RS485_GPIO_RCC | RS485_MODE_RCC, ENABLE);	//�򿪴���GPIOʱ�� RS485��������GPIOʱ�� 
	RCC_APB1PeriphClockCmd(RS485_USART_RCC,ENABLE);				//ʹ�ܶ�Ӧʹ�ô���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);			//���ù���ʱ�ӣ������������Ϊ���ù��ܼ���Ҫ����һ��

	GPIO_InitStructure.GPIO_Pin   = RS485_MODE_Pin;				//RS485ģʽ��������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//��ת�ٶ�50MHz
	GPIO_Init(RS485_MODE_PROT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = RS485_TX_Pin;				//TXD����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;			//��������
	GPIO_Init(RS485_PROT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = RS485_RX_Pin;				//RXD����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; 		//��������
	GPIO_Init(RS485_PROT, &GPIO_InitStructure);  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 			//ʹ�ܴ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 	//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 			//�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 							//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
	USART_InitStructure.USART_BaudRate = bound;					//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ���ݳ���
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ

	USART_Init(RS485_USART_NUM, &USART_InitStructure); ; 		//��ʼ������
	USART_ITConfig(RS485_USART_NUM, USART_IT_RXNE, ENABLE);		//�����ж�
	USART_Cmd(RS485_USART_NUM, ENABLE);                    		//ʹ�ܴ��� 
	RS485_TX_EN = 0;											//Ĭ��Ϊ����ģʽ 
}

/****************************************************************************************************
 * �������ƣ� void Send_Data(u8 *buf,u8 len)
 * ��ڲ�����u8 *buf u8 len
 * ����  ֵ����
 * ����˵�������ڷ�������
 * 			 buf:�������׵�ַ
 *			 len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
 ***************************************************************************************************/
void Send_Data(u8 *buf,u8 len)
{
	u8 t;
	RS485_TX_EN=1;			//����Ϊ����ģʽ
	delay_ms(1);
	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(RS485_USART_NUM, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 
	delay_ms(1);
	USART_RX_CNT=0;	  
	RS485_TX_EN=0;				//����Ϊ����ģʽ	
}

/****************************************************************************************************
 * �������ƣ�u8 UartRead(u8 *buf, u8 len) 
 * ��ڲ�����u8 *buf u8 len
 * ����  ֵ��u8
 * ����˵����������յ����ݳ��ȣ����ҽ����ݷŵ�*buf������
 ***************************************************************************************************/   
u8 UartRead(u8 *buf, u8 len)  
{
	u8 i;
	if(len > USART_RX_CNT)  		//ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ
	{
		len = USART_RX_CNT; 		//��ȡ��������Ϊʵ�ʽ��յ������ݳ���
	}
	
	for(i = 0;i < len;i ++)  		//�������յ������ݵ�����ָ����
	{
		*buf = USART2_RX_BUF[i];  	//�����ݸ��Ƶ�buf��
		buf  ++;
	}
	USART_RX_CNT=0;              	//���ռ���������
	return len;                   	//����ʵ�ʶ�ȡ����
}
/****************************************************************************************************
 * �������ƣ�void UartRxMonitor(u8 ms)
 * ��ڲ�����u8 ms
 * ����  ֵ����
 * ����˵�����ڶ�ʱ���е��ã����ڼ�����ݽ���
 ***************************************************************************************************/   
void UartRxMonitor(u8 ms) 					
{
	static u8 USART_RX_BKP = 0;  			//����USART2_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
	static u8 idletmr      = 0;        		//������ʱ��
	
	if(USART_RX_CNT > 0)					//���ռ�����������ʱ��������߿���ʱ��
	{
		if(USART_RX_BKP != USART_RX_CNT) 	//���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
		{
			USART_RX_BKP = USART_RX_CNT;  	//��ֵ��������ʵ�ʳ��ȸ�USART2_RX_BKP
			idletmr      = 0;               //�����ʱ������
		}
		else                              	//���ռ�����δ�ı䣬�����߿���ʱ���ۼƿ���ʱ��
		{
											//�����һ֡�������֮ǰ�г���3.5���ֽ�ʱ���ͣ�٣������豸��ˢ�µ�ǰ����Ϣ���ٶ���һ���ֽ���һ���µ�����֡�Ŀ�ʼ
			if(idletmr < 3)                 //����ʱ��С��3msʱ�������ۼ�
			{
				idletmr += ms;
				if(idletmr >= 3)            //����ʱ��ﵽ3msʱ�����ж�Ϊ1֡�������
				{
					From_Flag = 1;			//����������־��֡������ϱ�־
				}
			}
		}
	}
	else
	{
		USART_RX_BKP = 0;
	}
}

/****************************************************************************************************
 * �������ƣ�void UartRxMonitor(u8 ms)
 * ��ڲ�����u8 ms
 * ����  ֵ����
 * ����˵�������������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
 *           �����������������ڹ��ܵ��Ⱥ�����ֻ��Ҫ�õ�������ص����ݣ�
 *           �õ������ݸ������ѵ���Ҫʹ�ü��ɣ������ǻ���modbusЭ�飬����
 *           ��Ҫ����modbusЭ��ȥ��⣬������������modbus����    ���㲻������ͷ
 ***************************************************************************************************/   
void UartDriver(void)
{
//	unsigned char i;
	
	unsigned int crc;
	unsigned char crch,crcl;
	static unsigned char len;
	static unsigned char buf[64];
	if (From_Flag)            									//֡������ɱ�־�������յ�һ֡������
	{
		From_Flag = 0;           								//֡������ɱ�־����
		len       = UartRead(buf,sizeof(buf));   				//�����յ������������������
		crc       = GetCRC16(buf,len-2);       					//����CRCУ��ֵ����ȥCRCУ��ֵ
		crch=crc  >> 8;    										//crc��λ
		crcl=crc  &  0xFF;										//crc��λ
//		printf("485��������\r\n");
//		for(i = 0;i < len;i ++)
//			printf("485���յ�ֵ = buf[%d] = %x \r\n",i,buf[i]);
		if((buf[len-2] == crch) && (buf[len-1] == crcl))  		//�ж�CRCУ���Ƿ���ȷ
		{
			if (buf[1] == 0x03)									//0x03 ������
			{
				if((buf[2] == 0x00) && (buf[3] <= 0x05))  		//�Ĵ�����ַ֧��0x0000~0x0005
				{
					/* ͨ���������֤�жϺ� �������ֱ�ӻ�ȡ���� ������ReadDateVal�� */
					ReadDateVal = buf[3] * 256 + buf[4];		//modbus������16λ��
				}
			}
			/* д�����Ҫ����ֻ��ҪӦ�𼴿� */
			
			/* �ж�У������ȷ�� �����Ƕ�����д ������485æ��־����ʾ�յ�Ӧ���ͷ�485���ɽ�������������� */
			RS485Busy = 0;										
		}
		
	}
}

/****************************************************************************************************
 * �������ƣ�void RS485_RW_Opr(u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate)
 * ��ڲ�����u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate
 * 			 ucAddr���ӻ���ַ
 *			 ucCmd �������� 03->��	06->д
 *			 ucReg ���Ĵ�����ַ
 *			 uiDate��д�������Ƿ��͵����� ���������Ƕ�ȡ���ݸ���
 * ����  ֵ����
 * ����˵����485��д��������
 ***************************************************************************************************/   
void RS485_RW_Opr(u8 ucAddr,u8 ucCmd,u16 ucReg,u16 uiDate)
{
	unsigned int crc;
	unsigned char crcl;
	unsigned char crch;	
	unsigned char ucBuf[8];
	
	ucBuf[0] = ucAddr;				/* �ӻ���ַ */
	ucBuf[1] = ucCmd;				/* ���� 06 д 03 �� */
	ucBuf[2] = ucReg >> 8;			/* �Ĵ�����ַ��λ */
	ucBuf[3] = ucReg;				/* �Ĵ�����ַ��λ */
	ucBuf[4] = uiDate >> 8;			/* ���ݸ�8λ */
	ucBuf[5] = uiDate;				/* ���ݵ�8λ */
	crc      = GetCRC16(ucBuf,6);   /* ����CRCУ��ֵ */
	crch     = crc >> 8;    		/* crc��λ */
	crcl     = crc &  0xFF;			/* crc��λ */
	ucBuf[6] = crch;				/* У���8λ */
	ucBuf[7] = crcl;				/* У���8λ */
	
	Send_Data(ucBuf,8);				/* �������� */
	RS485Busy = 1;					/* ������ɺ� æ��־��λ */
}

/****************************************************************************************************
 * �������ƣ�void UartRxMonitor(u8 ms)
 * ��ڲ�������
 * ����  ֵ����
 * ����˵�������ڽ����жϴ�����
 ***************************************************************************************************/   
unsigned int ucTemp;
uint16_t uart_rxne_flag=0;
uint8_t i=0;
uint8_t uart_data[7]={0};
void USART2_IRQHandler(void)
{
		    
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 	//���յ�����
	{	 
		ucTemp =USART_ReceiveData(USART2); 					//��ȡ���յ�������
		if(USART_RX_CNT <7)
		{
			uart_rxne_flag=0;
			USART2_RX_BUF[USART_RX_CNT] = ucTemp;			//��¼���յ���ֵ
			USART_RX_CNT ++;								//������������1 
		if(USART_RX_CNT==7)
		{
			for(i=0;i<7;i++)
			{
				uart_data[i]=USART2_RX_BUF[i];
			}
		}
		}
	}  											 
} 




















/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "iot_gpio_ex.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_uart.h"
#include "hi_uart.h"
#include "iot_watchdog.h"
#include "iot_errno.h"

#define UART_BUFF_SIZE 100
#define U_SLEEP_TIME   100000
#define Tvalue 100
#define Ttime 3
#define longth 380000
unsigned int distance=0;
int uartsign=0;
void Uart1GpioInit(void)
{
    IoTGpioInit(IOT_IO_NAME_GPIO_0);
    // 设置GPIO0的管脚复用关系为UART1_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_0, IOT_IO_FUNC_GPIO_0_UART1_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_1);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_1, IOT_IO_FUNC_GPIO_1_UART1_RXD);
    
}
void Uart2GpioInit(void)
{
IoTGpioInit(IOT_IO_NAME_GPIO_11);
    // 设置GPIO0的管脚复用关系为UART2_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_11, IOT_IO_FUNC_GPIO_11_UART2_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_12);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_12, IOT_IO_FUNC_GPIO_12_UART2_RXD);
}
void Uart1Config(void)
{
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(HI_UART_IDX_1, &uart_attr);
    if (ret != IOT_SUCCESS) {
        printf("Init Uart1 Falied Error No : %d\n", ret);
        return;
    }
}
void Uart2Config(void)
{
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(HI_UART_IDX_2, &uart_attr);
    if (ret != IOT_SUCCESS) {
        printf("Init Uart2 Falied Error No : %d\n", ret);
        return;
    }
}
static void UartTask(void)
{
    const char *dat1 = "12\n";
    const char *dat2 = "200\n";
    uint32_t count = 0;
    uint32_t len1 = 0;
    uint32_t len2 = 0;
    unsigned char uartReadBuff1[UART_BUFF_SIZE] = {0};
    unsigned char uartReadBuff2[UART_BUFF_SIZE] = {0};
    uint32_t data1=0 ;
    uint32_t data2=0 ;
    uint8_t time1=0;
    uint8_t time2=0;
    uint32_t rogg1=0;
    uint32_t rogg2=0;
    uint32_t average1=0;
    uint32_t average2=0;
    uint32_t all=0;

    // 对UART1的一些初始化 Some initialization of UART1
    Uart1GpioInit();
    Uart2GpioInit();
    // 对UART1参数的一些配置 Some configurations of UART1 parameters
    Uart1Config();
    Uart2Config();
    while (1) {
        uartsign=0;
        // 通过UART1 发送数据 Send data through UART1
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)dat1, strlen(dat1));
        // 通过UART1 接收数据 Receive data through UART1
        len1 = IoTUartRead(HI_UART_IDX_1, uartReadBuff1, UART_BUFF_SIZE);
        if (len1 > 0) {
            // 把接收到的数据打印出来 Print the received data
            data1=atoi(uartReadBuff1);
            printf("Uart1 Read Data is: [ %d ] %d \r\n", count, data1);
        }
        IoTUartWrite(HI_UART_IDX_2, (unsigned char*)dat2, strlen(dat2));
        // 通过UART1 接收数据 Receive data through UART1
        len2 = IoTUartRead(HI_UART_IDX_2, uartReadBuff2, UART_BUFF_SIZE);
        if (len2 > 0) {
            // 把接收到的数据打印出来 Print the received data
            data2=atoi(uartReadBuff2);
            printf("Uart2 Read Data is: [ %d ] %d \r\n", count, data2);
        }
        usleep(U_SLEEP_TIME);
        count++;
        


        if((len1>0&&len2>0)&&(data1>Tvalue||data2>Tvalue))
        {   rogg1=rogg1+data1;
            rogg2=rogg2+data2;        
                time1++;
                time2++;

        }      
        if((time1>Ttime||time2>Ttime)&&(data1<Tvalue)&&(data1>data2))
        {
            average1=(rogg1/time1);
            average2=(rogg2/time2);
            all=average1+average2;
            distance=(longth/all)*average2;
            uartsign=1;
            printf("distance1= %d\r\naverage1=%d,average2=%d\n",distance,average1,average2);
            rogg1=0;
            rogg2=0;
           time1=0;
           time2=0;
        }
        else if((time1>Ttime||time2>Ttime)&&(data2<Tvalue)&&(data1<data2))
        {
            average1=(rogg1/time1);
            average2=(rogg2/time2);
            all=average1+average2;
            distance=(longth/all)*average2;
            uartsign=1;
            printf("distance2= %d\r\naverage1=%d,average2=%d\n",distance,average1,average2);
            rogg1=0;
            rogg2=0;
            time1=0;
            time2=0;
        }
      
       
        
    }
}

void UartExampleEntry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();

    attr.name = "UartTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 5 * 1024; // 任务栈大小*1024 stack size 5*1024
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)UartTask, NULL, &attr) == NULL) {
        printf("[UartTask] Failed to create UartTask!\n");
    }
}

APP_FEATURE_INIT(UartExampleEntry);

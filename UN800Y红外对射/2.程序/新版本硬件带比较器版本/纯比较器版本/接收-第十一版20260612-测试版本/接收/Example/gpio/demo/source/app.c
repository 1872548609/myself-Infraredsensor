/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "common.h"
#include "uart0.h"
#include "adc.h"
#include "gpio.h"
#include "gtimer.h"

#define adtime 3

uint8_t adc_irq_flag = 0;
uint8_t adc_count = 0;
uint16_t adc_data_buf[10];
uint32_t adc_datav = 0;
	uint16_t temp=0;
uint16_t i=2;
volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

bit adcget = 0;
bit cleanadctime =0;
bit jici = 0;


/************************************************************************
 * Function   	: uart0_rec_pro
 * Description	: uart0_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart0_rec_pro(void)
{
    uint8_t rx_byte;

    rx_byte = uart0_recv_byte();

    /* 作为循环缓冲区使用，保证任何情况下都不会越界写 RAM */
    if (rx_count >= sizeof(uart0_rx_buf))
    {
        rx_count = 0;
    }

    uart0_rx_buf[rx_count] = rx_byte;
    rx_count++;

    rx_flag = 1;
}

/************************************************************************
 * Function   	: uart_init
 * Description	: uart_init 串口初始化
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart_init(void)
{
	rx_flag = 0;
	uart0_init(UART0_BAUD_RATE);
  uart0_irq_init(UART0_IRQ_ENABLE,uart0_rec_pro);
}

 /************************************************************************
 * function   	: adc_continuous_pro
 * Description	: adc call back function of continuous mode
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_continuous_pro(void)
{
		REG_GTIM1_CR0 &= ~(1<<0);					//不使能计数器
		temp=0;
		temp = REG_GTIM1_CNT0;
		temp |= REG_GTIM1_CNT1<<8;
		REG_GTIM1_CNT0=0;
		REG_GTIM1_CNT1=0;
		printfS("%u\r\n", temp);
		REG_GTIM1_CR0 |= (1<<0);						//使能计数器	

}

 /************************************************************************
 * function   	: adc_continuous_mode_test
 * Description	: adc_continuous_mode_test 
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_continuous_mode_test(void)
{
	if(adcget)
	{
		adcget=0;
		adc_datav/=adtime;		//求平均
		if(adc_datav >500)		//大于阈值
		{
			gpio_io_set(P1_3, GPIO_LOW);		//拉低信号
			cleanadctime=1;									//延时拉高
		}
		adc_datav=0;					//清除十次计数
		adc_count=0;					//清除ad值
			adc_controller_config(ADC_ENABLE);									//ADC控制器启动	
			adc_convert_start(ADC_CHANNEL_1);									//启动转换	
	}
		
}

/************************************************************************
 * Function   	: soc_test
 * Description	: soc_test
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void soc_test(void)
{
	
}



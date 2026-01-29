/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-31
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "config.h"
#include "common.h"
#include "uart3.h"

volatile uint8_t uart3_rx_flag ;
volatile uint8_t uart3_rx_buf[32];
volatile uint8_t uart3_tx_buf[32];
volatile uint8_t uart3_rx_count = 0;	
volatile uint8_t uart3_tx_count = 0;

/************************************************************************
 * Function   	: uart3_rec_pro
 * Description	: uart3_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 	: none
 * Return		: none
 ************************************************************************/
void uart3_rec_pro(void)
{
	uart3_rx_buf[uart3_rx_count++]= uart3_recv_byte();	
	uart3_rx_flag = 1;
}

/************************************************************************
 * Function   	: uart_init
 * Description	: uart_init 串口初始化
 * Input 		: none
 * Output 	: none
 * Return		: none
 ************************************************************************/
void uart_init(void)
{
	uart3_rx_flag = 0;
	uart3_init(UART3_BAUD_RATE);
  uart3_irq_init(UART3_IRQ_ENABLE,uart3_rec_pro);

}

/************************************************************************
 * Function   	: my_memcpy
 * Description	: my_memcpy
 * Input 		: uint8_t *dst	目的地址
 *				  	uint8_t *src	源地址
 *				  	uint16_t n   数据长度
 * Output 	: none
 * Return		: none
 ************************************************************************/
void my_memcpy(uint8_t *dst,uint8_t *src,uint16_t n)
{
	while(n--)
	{	
		*dst++ = *src++;
	}
}

/************************************************************************
 * Function   	: uart3_test
 * Description	: uart3_test
 * Input 		: none
 * Output 	: none
 * Return		: none
 ************************************************************************/
void uart3_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test uart! \n");
	printfS("waiting for output data.....! \n");
	
	while(1)
	{		
		if(uart3_rx_flag)
		{
			my_memcpy(uart3_tx_buf, uart3_rx_buf, uart3_rx_count);  
			uart3_tx_count = uart3_rx_count;
			uart3_rx_count = 0;  
			uart3_send_bytes(uart3_tx_buf,uart3_tx_count);	
			uart3_rx_flag = 0;		   
		}
		delay1ms(50); 	
	}    
	
}



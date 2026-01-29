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
#include "uart2.h"

volatile uint8_t uart2_rx_flag ;
volatile uint8_t uart2_rx_buf[32];
volatile uint8_t uart2_tx_buf[32];
volatile uint16_t uart2_rx_count = 0;
volatile uint16_t uart2_tx_count = 0;


/************************************************************************
 * Function   	: uart2_rec_pro
 * Description	: uart2_rec_pro 串口接收处理回调函数
 * Input 				: none
 * Output 			: none
 * Return				: none
 ************************************************************************/
void uart2_rec_pro(void)
{
	uart2_rx_buf[uart2_rx_count++]= uart2_recv_byte();	
	uart2_rx_flag = 1;
}

/************************************************************************
 * Function   	: uart_init
 * Description	: uart_init 串口初始化
 * Input 				: none
 * Output 			: none
 * Return				: none
 ************************************************************************/
void uart_init(void)
{
	uart2_rx_flag = 0;
	uart2_init(UART2_BAUD_RATE);
  uart2_irq_init(UART2_IRQ_ENABLE,uart2_rec_pro);
	
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
 * Function   	: uart2_test
 * Description	: uart2_test
 * Input 				: none
 * Output 			: none
 * Return				: none
 ************************************************************************/
void uart2_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test uart! \n");
	printfS("waiting for output data.....! \n");
	
	while(1)
	{		
		if(uart2_rx_flag)
		{
			my_memcpy(uart2_tx_buf, uart2_rx_buf, uart2_rx_count);  
			uart2_tx_count = uart2_rx_count;
			uart2_rx_count = 0;  
			uart2_send_bytes(uart2_tx_buf,uart2_tx_count);	
			uart2_rx_flag = 0;
  
		}
		delay1ms(50); 	
	}    
}


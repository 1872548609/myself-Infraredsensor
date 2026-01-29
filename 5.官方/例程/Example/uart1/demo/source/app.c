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
#include "config.h"
#include "common.h"
#include "uart1.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart1_rx_buf[32];
volatile uint8_t uart1_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

/************************************************************************
 * Function   	: uart0_rec_pro
 * Description	: uart0_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart1_rec_pro(void)
{
	uart1_rx_buf[rx_count++]= uart1_recv_byte();
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
	uart1_init(UART1_BAUD_RATE);
    uart1_irq_init(UART1_IRQ_ENABLE,uart1_rec_pro);
}

/************************************************************************
 * Function   	: uart_test
 * Description	: uart_test
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test uart1! \n");
	printfS("waiting for output data.....! \n");
	
	while(1)
	{		
		if(rx_flag)
		{
            my_memcpy(uart1_tx_buf, uart1_rx_buf, rx_count);  
            tx_count = rx_count;
            rx_count = 0;   
            uart1_send_bytes(uart1_tx_buf,tx_count);			
			rx_flag = 0;
			   
		}
		delay1ms(50); 	
	}    
	
}

/************************************************************************
 * Function   	: my_memcpy
 * Description	: my_memcpy
 * Input 		: uint8_t *dst	目的地址
 *				  uint8_t *src	源地址
 *				  uint16_t n   数据长度
 * Output 		: none
 * Return		: none
 ************************************************************************/
void my_memcpy(uint8_t *dst,uint8_t *src,uint16_t n)
{
	while(n--)
	{	
		*dst++ = *src++;
	}
}

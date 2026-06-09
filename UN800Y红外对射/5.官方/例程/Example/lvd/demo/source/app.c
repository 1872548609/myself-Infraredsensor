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
#include "uart0.h"
#include "lvd.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;
extern uint8_t lvd_flag;
/************************************************************************
 * Function   	: uart0_rec_pro
 * Description	: uart0_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart0_rec_pro(void)
{
	uart0_rx_buf[rx_count++]= uart0_recv_byte();
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
 * Function   	: lvd_test
 * Description	: lvd_test
 * Input 		: none
 * Output 	: none
 * Return		: none
 ************************************************************************/
void lvd_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
 	printfS("start test lvd! \n");	
	
  lvd_init();			//低压检测初始化
	
	while(1)
	{
		if(lvd_flag == 1)
		{
			printfS("LVD IRQ!\r\n");
			lvd_flag = 0;
		}
	}
}




/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2020-07-27
 ***********************************************************************/
#include  "system_um800y.h"
#include  "app.h"
#include  "config.h"
#include  "common.h"
#include  "uart0.h"
#include  "beeper.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;
/************************************************************************
 * function   : uart0_rec_pro
 * Description: uart0_rec_pro 串口接收处理回调函数
 * input : none
 * return: none
 ************************************************************************/
void uart0_rec_pro(void)
{
	uart0_rx_buf[rx_count++]= uart0_recv_byte();
	rx_flag = 1;	
}

/************************************************************************
 * function   : uart_init
 * Description: uart_init 串口初始化
 * input : none
 * return: none
 ************************************************************************/
void uart_init(void)
{
	rx_flag = 0;
	uart0_init(UART0_BAUD_RATE);
    uart0_irq_init(UART0_IRQ_ENABLE,uart0_rec_pro);
}


/************************************************************************
 * function   : beeper_test
 * Description: beeper_test
 * input : none
 * return: none
 ************************************************************************/
void beeper_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("beep test start! \n");
	beeper_init();
	beeper_level_set(HIGH,SEL_1KHz);
	
	while(1)
	{			
    
	}    
	
}

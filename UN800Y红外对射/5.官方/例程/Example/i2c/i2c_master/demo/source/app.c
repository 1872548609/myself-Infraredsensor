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
#include "i2c_master.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;
volatile uint8_t master_irq_flag = 0;

/************************************************************************
 * Function   	: uart0_rec_pro
 * Description	: uart0_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 	: none
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
 * Output 	: none
 * Return		: none
 ************************************************************************/
void uart_init(void)
{
	rx_flag = 0;
	uart0_init(UART0_BAUD_RATE);
  uart0_irq_init(UART0_IRQ_ENABLE,uart0_rec_pro);
}

/************************************************************************
 * Function   	: i2c_irq_pro
 * Description	: i2c_irq_pro I2C中断回调函数
 * Input 			: none
 * Output 		: none
 * Return			: none
 ************************************************************************/
void i2c_irq_pro(void)
{
	master_irq_flag = 1;
}


void i2c_master_send_test(void)
{
	uint8_t xdata status;
	uint8_t xdata i;
	uint8_t xdata i2c_send_data[10];
	for(i=0 ; i<8; i++)
	{
		i2c_send_data[i] = i+0x10;
	
	}
	printfS("i2c master send start\r\n");
	i2c_master_init(MASTER_I2C_SPEED_400K);	
	
	while(1)
	{
		status = i2c_master_write_data(I2C_SLAVE_ADDR1,i2c_send_data,8);
	
		printfS("status = 0x%bx\r\n",status);
		printfS("i2c master send end\r\n");
		delay1ms(1000);
	}
}


void i2c_master_recv_test(void)
{
	uint8_t xdata status;
	uint8_t xdata i2c_recv_data[10];
	printfS("i2c master recv start\r\n");
	i2c_master_init(MASTER_I2C_SPEED_100K);	
	
	while(1)
	{
		status = i2c_master_read_data(I2C_SLAVE_ADDR1,i2c_recv_data,8);
			
		printfS("status = 0x%bx\r\n",status);
		printfS("i2c master recv end\r\n");
		printfB8(i2c_recv_data,8);
		delay1ms(1000);
	}

}


void soc_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test i2c! \n");

	i2c_master_send_test();
//	i2c_master_recv_test();
}



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
#include "i2c_slave.h"


volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;
volatile uint8_t slave_irq_flag = 0;
volatile uint8_t  slave_irq_len;
volatile uint8_t i2c_slave_irq_recv_data[20];

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
 * Function   	: i2c_irq_pro
 * Description	: i2c_irq_pro I2C中断回调函数
 * Input 			: none
 * Output 		: none
 * Return			: none
 ************************************************************************/
void i2c_irq_pro(void)
{
	slave_irq_flag = 1;
	slave_irq_len = i2c_slave_read(i2c_slave_irq_recv_data);

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

void i2c_slave_write_test(void)
{
	uint8_t xdata i,num=0;
	uint8_t xdata len;
	uint8_t xdata i2c_slave_send_data[20];
	for(i=0 ; i<8; i++)
	{
		i2c_slave_send_data[i] = i+0xa0;
	
	}
	printfS("i2c slave write test start\r\n");
	i2c_slave_init();											//从机初始化
	i2c_set_slave_addr1(I2C_SLAVE_ADDR1);	//设置从机地址1
	i2c_set_slave_addr2(I2C_SLAVE_ADDR2);	//设置从机地址2
	
	while(1)
	{	
		if(i2c_slave_is_match_addr1() == i2c_tx) 		//maas1匹配
		{				
			len = i2c_slave_write(i2c_slave_send_data);
			printfS("write len:%bd\n", len);
			printfS("match addr1,i2c slave send\n");
		
		}
		if(i2c_slave_is_match_addr2()	== i2c_tx) 		//maas2匹配
		{			
			len = i2c_slave_write(i2c_slave_send_data);
			printfS("write len:%bd\n", len);
			printfS("match addr2,i2c slave send\n");
		}

	}
}

void i2c_slave_read_test(void)
{
	printfS("i2c slave read test start\r\n");
	
	i2c_slave_init();													//从机初始化
	i2c_set_slave_addr1(I2C_SLAVE_ADDR1);			//设置从机地址
	i2c_irq_enable(I2C_IRQ_MAAS1,i2c_irq_pro);	//地址1匹配中断
	i2c_ack();										//使能应答ACK

	while(1)
	{
		if(slave_irq_flag == 1)
		{
			slave_irq_flag = 0;
			printfB8(i2c_slave_irq_recv_data,slave_irq_len);
			printfS("match addr1,i2c slave receive\n");
		}
	}	
}

void soc_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test i2c! \n");

	i2c_slave_read_test();				//I2C从机接收测试
//	i2c_slave_write_test();				//I2C从机发送测试
	
	
}


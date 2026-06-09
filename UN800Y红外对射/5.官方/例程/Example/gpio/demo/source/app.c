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
#include "gpio.h"
#include "res_gpio.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

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
 * function   : gpio_int_pro
 * Description: gpio_int_pro GPIO中断处理回调函数
 * input : none
 * return: none
 ************************************************************************/
void gpio_int_pro(void)
{
  printfS("gpio interrupt !!! \n\r");
}


/************************************************************************
 * function   	: gpio_in_test
 * Description	: gpio_in_test
 * input 				: uint8_t pin
 * Output 			: none
 * return				: none
 ************************************************************************/
void gpio_in_test(uint8_t pin)
{
	uint8_t temp; 
	
	gpio_init(pin);                       //时钟初始化，模块正常工作
	gpio_dir_set(pin, GPIO_DIR_IN);       //配置为输入
	gpio_pu_set(pin, GPIO_PU_ENABLE);     //使能内部上拉电阻
	gpio_in_enable(pin, IN_ENABLE);       //输入使能 

	while (1) 
	{
		temp = gpio_io_get(pin);
		printfS("gpio level is: =  %bd\n\r",temp);
		delay1ms(1000);
	}
}

/************************************************************************
 * function   	: gpio_out_test
 * Description	: gpio_out_test
 * input 				: none
 * Output 			: none
 * return				: none
 ************************************************************************/
void gpio_out_test(uint8_t pin)
{ 
	gpio_init(pin);                      //时钟初始化，模块正常工作

	gpio_dir_set(pin, GPIO_DIR_OUT);     //配置为输出

  while(1) 
	{
		gpio_io_set(pin, GPIO_HIGH);						
		delay1ms(1000);
		
		gpio_io_set(pin, GPIO_LOW); 
		delay1ms(1000);
	}                
}


/************************************************************************
 * function   : gpio_irq_test
 * Description: gpio_irq_test
 * GPIO中断测试
 * input : none
 * return: none
 ************************************************************************/
void gpio_irq_test(uint8_t pin)
{   
	gpio_init(pin);                          //时钟初始化，模块正常工作
	gpio_dir_set(pin, GPIO_DIR_IN);          //配置为输入
	gpio_pu_set(pin, GPIO_PU_ENABLE);        //使能内部上拉电阻
	gpio_in_enable(pin, IN_ENABLE);          //输入使能 
	gpio_irq_set(pin, GPIO_IRQ_ENABLE, gpio_int_pro);	//使能中断
	
	while(1)
	{
		
	}
    
}

void soc_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
 	printfS("start test gpio! \n");

//	gpio_in_test(P0_3);		//GPIO输入测试

//	gpio_out_test(P0_3);		//GPIO输出测试
	
	gpio_irq_test(P0_3);			//GPIO中断测试
	
//	res_gpio_test();				//复位脚P0_2复用成GPIO功能
	
}


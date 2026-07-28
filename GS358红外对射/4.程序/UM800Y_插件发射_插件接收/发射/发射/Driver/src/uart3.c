/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart3.c
 * Description : uart3 driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-09
 ***********************************************************************/
#include "uart3.h"
#include "config.h"

volatile uint8_t uart3_tx_flag = 0;

extern uint32_t system_core_clock;

void (*uart3_func)(void) = { 0 };

/************************************************************************
 * Function   	: UART3_IRQHandler
 * Description	: uart3 interrupt handling
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void UART3_IRQHandler(void) interrupt 17
{
	if(REG_UART3_ISR & 0x20)						//FIFO非空中断
	{
		if(uart3_func != NULL)
		{
			((void(*)())(uart3_func))();  	//uart3的接收处理回调函数
			REG_UART3_ISR &= ~(1<<5);				//清除FIFO_NE非空标志位
		}
	}
#ifdef UART3_TX_INT_MODE				
	else if(REG_UART3_ISR & 0x02)				//发送完成中断
	{
		if(!(REG_UART3_ISR & (1<<1)))
		{
			uart3_tx_flag = 1;
			REG_UART3_ISR &= ~(1<<1);				//清除发送完成标志位
		}
	}
#endif
	else
	{
		REG_UART3_ISR = 0x00; 						//清除中断状态
	}
	
}

/************************************************************************
 * Function   	: uart3_set_baud_rate
 * Description	: uart3 set baud rate
 * Input 		: uint32_t clk_hz: cpu frequency
 *         		uint32_t baud_rate: Series rate	  		   				
 * Output		: none  
 * Return		: none
 ************************************************************************/
static void uart3_set_baud_rate(uint32_t clk_hz, uint32_t baud_rate)	
{	
	uint16_t temp;
	uint8_t MSbyte, LSbyte;
    
	temp = clk_hz / baud_rate;
  
	MSbyte = temp>>8;
	LSbyte = temp;
	
	REG_UART3_BRPH = MSbyte;
	REG_UART3_BRPL = LSbyte+1;		//+1：波特率精度补偿
}

/************************************************************************
 * Function   	: uart3_init
 * Description	: uart3 initial for  baud_rate
 * Input 		: uint32_t baud_rate: Series rate 
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart3_init(uint32_t baud_rate)
{
	BEEPCTR |= (1<<7); 									//开uart3时钟使能	
	BEEPCTR |= (1<<6);									//uart3正常工作

	REG_P03_CFG = 0x3;									//P03复用为UART3_RX
	REG_P25_CFG = 0x1;									//P25复用为UART3_TX
	
	P2PU &= ~(1<<5);										//使能P25内部上拉  
	P0PU &= ~(1<<3);										//使能P03内部上拉 	
	
	REG_UART3_CR = 0x4;									//清除接收FIFO中数据和指针
	REG_UART3_CR = 0x0a; 								//无奇偶校验,发送数据使能

	uart3_set_baud_rate(system_core_clock,baud_rate);			//设置波特率
}

/************************************************************************
 * Function   	: uart3_irq_init
 * Description	: uart3 interrupt enable
 * Input 		: uint8_t irqstate: 0 中断失能，1 中断使能
 * 		   		  *pfunc_recv)(): 接收处理回调函数
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart3_irq_init(uint8_t irqstate,void (*pfunc_recv)())
{
	if(irqstate == UART3_IRQ_ENABLE)
	{
		EA = 1;										//打开总中断开关
		IEN2 = (1<<0);						//使能uart3中断
		
#ifdef UART3_TX_INT_MODE			//发送采用中断方式
		REG_UART3_IER = 0x22;   	//接收FIFO非空中断使能，发送完成中断使能,enable Rx/Tx_INT,disable else int
#else													//发送采用查询方式
		REG_UART3_IER = 0x20;   	//接收FIFO非空中断使能，发送完成中断不使能,enable Rx_INT,disable Tx_INT and else int	

#endif	
		uart3_func = pfunc_recv;	//接收中断回调函数
	}
	else
	{
		IEN2 = 0;									//关闭uart3中断
	}
		
}

/************************************************************************
 * Function   	: uart3_send_byte
 * Description	: uart3 发送一个字节数据
 * Input 		: char c: out byte
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart3_send_byte(char c)
{
	//输出一个字符
	uint16_t timeout;
	timeout = 65535;
	
	REG_UART3_TDR = c;

#ifdef UART3_TX_INT_MODE        		//中断方式
	while(!uart3_tx_flag);		
	uart3_tx_flag = 0; 		
#else 															//查询方式
	while((REG_UART3_ISR & 0x02) != 0x02) 		//等待数据发送完成
	{		
		if((timeout--)==0)   						//超时等待退出
		{		
			break;		
		}		
	}		
	REG_UART3_ISR &= ~(1<<1);			   	//清除发送完成标志位

#endif
}

/************************************************************************
 * Function   	: uart3_send_bytes
 * Description	: uart3 send bytes	发送多个字节
 * Input 		: uint8_t* buff: out buffer
 *       		  uint32_t length: buffer length
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart3_send_bytes(uint8_t *buff, uint32_t length)
{
	while(length--)
	{
		uart3_send_byte(*buff++);
	}	
}

/************************************************************************
 * Function   	: uart3_recv_byte
 * Description	: uart3 接收一个字节数据
 * Input 		: none 
 * Output		: none  
* Return		: REG_uart3_RDR：返回一个byte	
 ************************************************************************/
uint8_t uart3_recv_byte(void)
{ 
	return REG_UART3_RDR;
}


/************************************************************************
 * Function   	: putchar
 * Description	: 打印函数重映射
 * Input 		: char c: 一个字符
 * Output		: none  
 * Return		: none
 ************************************************************************/

char putchar(char c)
{
	uart3_send_byte(c); 
	return c;
}

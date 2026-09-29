/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart2.c
 * Description : uart2 driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-31
 ***********************************************************************/
#include "uart2.h"
#include "config.h"
volatile uint8_t uart2_tx_flag = 0;

extern uint32_t system_core_clock;
void (*uart2_func)(void) = { 0 };

/************************************************************************
 * Function   	: UART2_IRQHandler
 * Description	: uart2 interrupt handling
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void UART2_IRQHandler(void) interrupt 16
{
	if(REG_UART2_ISR & 0x20)											//FIFO非空中断
	{
		if(uart2_func != NULL)
		{
			((void(*)())(uart2_func))();  					//uart2的接收处理回调函数
			REG_UART2_ISR &= ~(1<<5);							//清除FIFO_NE非空标志位
		}
	}	
#ifdef UART2_TX_INT_MODE			
	else if(REG_UART2_ISR & 0x02)							//发送完成中断
	{
		if(!(REG_UART2_ISR & (1<<1)))
		{
			uart2_tx_flag = 1;
			REG_UART2_ISR &= ~(1<<1);							//清除发送完成标志位
		}
	}	
#endif
	else
	{
		REG_UART2_ISR = 0x00; 								//清除中断状态
	}

}

/************************************************************************
 * Function   	: uart2_set_baud_rate
 * Description	: uart2 set baud rate
 * Input 		: uint32_t clk_hz: cpu frequency
 *         		  uint32_t baud_rate: Series rate	  		   				
 * Output		: none  
 * Return		: none
 ************************************************************************/
static void uart2_set_baud_rate(uint32_t clk_hz, uint32_t baud_rate)	
{	
	uint16_t temp;
	uint8_t MSbyte, LSbyte;
    
	temp = clk_hz / baud_rate;
  
	MSbyte = temp>>8;
	LSbyte = temp;
	
	REG_UART2_BRPH = MSbyte;
	REG_UART2_BRPL = LSbyte+1;		
}

/************************************************************************
 * Function   	: uart2_init
 * Description	: uart2 initial for  baud_rate
 * Input 		: uint32_t baud_rate: Series rate 
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart2_init(uint32_t baud_rate)
{
	
	PCLK1 |= (1<<7); 									//开UART2时钟使能	
	PRESET1 |= (1<<7);								//UART2正常工作
	
	REG_P03_CFG = 0x2;									//P03复用为UART2_TX
	REG_P04_CFG = 0x1;									//P04复用为UART2_RX
	
	REG_UART2_CR |= (1<<3); 					//无奇偶校验
	REG_UART2_CR |= (1<<1); 					//发送数据使能	

	uart2_set_baud_rate(system_core_clock,baud_rate);			//设置波特率
	
	P0PU &= ~(3<<3);									//使能P03,P04内部上拉
}

/************************************************************************
 * Function   	: uart2_irq_init
 * Description	: uart2 interrupt enable
 * Input 		: uint8_t irqstate: 0 中断失能，1 中断使能
 * 		   		  *pfunc_recv)(): 接收处理回调函数
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart2_irq_init(uint8_t irqstate,void (*pfunc_recv)())
{
	if(irqstate == UART2_IRQ_ENABLE)
	{
		EA = 1;										//打开总中断开关
		UART2INTEN = 1;						//使能UART2中断
		
#ifdef UART2_TX_INT_MODE			//发送采用中断方式
		REG_UART2_IER = 0x22;     //接收FIFO非空中断使能，发送完成中断使能,enable Rx/Tx_INT,disable else int
#else													//发送采用查询方式
		REG_UART2_IER = 0x20;        					//接收FIFO非空中断使能，发送完成中断不使能,enable Rx_INT,disable Tx_INT and else int		
#endif	
		uart2_func = pfunc_recv;	//接收中断回调函数
	}
	else
	{
		UART2INTEN = 0;						//关闭UART2中断
	}
		
}

/************************************************************************
 * Function   	: uart2_send_byte
 * Description	: uart2 发送一个字节数据
 * Input 		: char c: out byte
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart2_send_byte(char c)
{
	//输出一个字符
	uint16_t timeout;
	timeout = 65535;
	
	REG_UART2_TDR = c;

#ifdef UART2_TX_INT_MODE        						//中断方式
	while(!uart2_tx_flag);		
	uart2_tx_flag = 0; 		
#else 													//查询方式
	while((REG_UART2_ISR & 0x02) != 0x02) 				//等待数据发送完成
	{		
		if((timeout--)==0)   							//超时等待退出
		{		
			break;		
		}		
	}		
	REG_UART2_ISR &= ~(1<<1);			   				//清除发送完成标志位

#endif
}

/************************************************************************
 * Function   	: uart2_send_bytes
 * Description	: uart2 send bytes	发送多个字节
 * Input 		: uint8_t* buff: out buffer
 *       		  uint32_t length: buffer length
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart2_send_bytes(uint8_t *buff, uint32_t length)
{
	while(length--)
	{
		uart2_send_byte(*buff++);
	}	
}

/************************************************************************
 * Function   	: uart2_recv_byte
 * Description	: uart2 接收一个字节数据
 * Input 		: none 
 * Output		: none  
* Return		: REG_UART2_RDR：返回一个byte	
 ************************************************************************/
uint8_t uart2_recv_byte(void)
{ 
	return REG_UART2_RDR;
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
	uart2_send_byte(c); 
	return c;
}


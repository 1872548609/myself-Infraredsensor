/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart1.c
 * Description : uart1 driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include  "uart1.h"
#include "config.h"

volatile uint8_t tx_flag = 0;
extern uint32_t system_core_clock;
void (*uart1_func)(void) = { 0 };

/************************************************************************
 * Function   	: UART1_IRQHandler
 * Description	: uart interrupt handling
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void UART1_IRQHandler(void) interrupt 2
{

	if((S1CON&0x1) == 0x1)						//Rx int
	{	
    if(uart1_func != NULL)
		{
			((void(*)())(uart1_func))();   		//uart1的接收处理回调函数
		}
		S1CON &= ~(0x1<<0);    					//清除接收完成标志位
	}	
	
#ifdef UART1_TX_INT_MODE						//中断方式
	else if((S1CON&0x2) == 0x2)   				// Tx int
	{
		tx_flag = 1;
		S1CON &= ~(0x1<<1);	    				//清除发送完成标志位
	}	
#endif		
}

/************************************************************************
 * Function   	: uart1_set_baud_rate
 * Description	: uart1 set baud rate
 * Input 		: uint32_t clk_hz: cpu frequency
 *         		  uint32_t baud_rate: Series rate	  Baud Rate = SYSCK/(16*(1024-S1REL))   SYSCK:RC16M 		   				
 * Output		: none  
 * Return		: none
 ************************************************************************/
static void uart1_set_baud_rate(uint32_t clk_hz, uint32_t baud_rate)	
{	
	uint32_t temp;
	uint8_t MSbyte, LSbyte;
    
    temp = baud_rate * 16;
    temp = (clk_hz + (temp / 2))/ temp;
	temp = 1024 - temp ;

	MSbyte = temp>>8;
	LSbyte = temp;
	
	S1RELH = MSbyte & 0x3;
	S1RELL = LSbyte;			
}

/************************************************************************
 * Function   	: uart1_init
 * Description	: uart1 initial for  baud_rate
 * Input 		: uint32_t baud_rate: Series rate 
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart1_init(uint32_t baud_rate)
{

	PCLK0 |= (1<<1);       								//开UART1时钟使能	
	PRESET0 |= (1<<1);	   								//UART1正常工作
	EUARTEN |= (1<<1);	   								//打开EUART1功能，P1.5作UART1_TX，P1.4作UART1_RX

  tx_flag = 0;
    
	S1CON |= (0x1<<7);      							//选择mode B 8位通信
							
	S1CON &= ~(0x1<<1);									//发送完成中断状态清除
	S1CON &= ~(0x1<<0);									//接收完成中断状态清除
	
	uart1_set_baud_rate(system_core_clock, baud_rate);		//设置波特率
	S1CON |= (0x1<<4);      							//使能接收
	
	P1PU &= ~(3<<4);											//使能P1.5,P1.4内部上拉	
}

/************************************************************************
 * Function   	: uart1_irq_init
 * Description	: uart1 interrupt enable
 * Input 		: uint8_t irqstate: 0 中断失能，1 中断使能
 * 		   		  *pfunc_recv)(): 接收处理回调函数
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart1_irq_init(uint8_t irqstate,void (*pfunc_recv)())
{
    if(irqstate == UART1_IRQ_ENABLE)
    {
        EA = 1;											//EA总中断开启
        ES1 = 1;										//UART1中断使能   
        
        uart1_func = pfunc_recv;       
    } 
    else
    {
      //  EA = 0;											//EA总中断关闭
        ES1 = 0;										//UART1中断关闭   
    } 
}

/************************************************************************
 * Function   	: uart1_send_byte
 * Description	: uart1 发送一个字节数据
 * Input 		: char c: out byte
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart1_send_byte(char c)
{
	//输出一个字符
	uint16_t timeout;
	timeout = 65535;
		
	S1BUF = c;

#ifdef UART1_TX_INT_MODE								//中断方式
	while(!tx_flag);
	tx_flag = 0;	
#else	
	//查询方式
    while(((S1CON&0x2)!=0x2))    						//等待数据发送完成
    {
        if((timeout--)==0)      						//超时等待退出
        {
			break;
        }
    }
    S1CON &= ~(0x1<<1);       							//清除发送完成标志位

#endif
}

/************************************************************************
 * Function   	: uart1_send_bytes
 * Description	: uart1 send bytes	发送多个字节
 * Input 		: uint8_t* buff: out buffer
 *       		  uint32_t length: buffer length
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart1_send_bytes(uint8_t *buff, uint32_t length)
{
	while(length--)
	{
		uart1_send_byte(*buff++);
	}	
}

/************************************************************************
 * Function   	: uart1_recv_byte
 * Description	: uart1 接收一个字节数据
 * Input 		: none 
 * Output		: none  
* Return		: S1BUF：返回一个byte	
 ************************************************************************/
uint8_t uart1_recv_byte(void)
{  
	return S1BUF;
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
	uart1_send_byte(c); 
	return c;
}



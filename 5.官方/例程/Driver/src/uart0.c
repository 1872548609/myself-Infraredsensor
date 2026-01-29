/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart0.c
 * Description : uart0 driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include "uart0.h"
#include "config.h"
volatile uint8_t tx_flag = 0;

extern uint32_t system_core_clock;
void (*uart0_func)(void) = { 0 };

/************************************************************************
 * Function   	: UART0_IRQHandler
 * Description	: uart interrupt handling
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void UART0_IRQHandler(void) interrupt 4
{
	if(RI0)                								//Rx irq
	{	
        if(uart0_func != NULL)
		{
			((void(*)())(uart0_func))();   				//uart0的接收处理回调函数
		}	
		RI0 = 0;            							//清除接收完成标志位
	}	

#ifdef UART0_TX_INT_MODE								//中断方式
    else if(TI0)            							//Tx irq 
	{
		tx_flag = 1;
		TI0	= 0;           								//清除发送完成标志位
	}	
#endif		
} 

/************************************************************************
 * Function   	: uart0_set_baud_rate
 * Description	: uart0 set baud rate
 * Input 		: uint32_t clk_hz: cpu frequency
 *         		  uint32_t baud_rate: Series rate	  Baud Rate = SYSCK/(16*(1024-S0REL))   SYSCK:RC16M 		   				
 * Output		: none  
 * Return		: none
 ************************************************************************/
static void uart0_set_baud_rate(uint32_t clk_hz, uint32_t baud_rate)	
{	
	uint32_t temp;
	uint8_t MSbyte, LSbyte;
    
	temp = baud_rate * 16;
    temp = (clk_hz + (temp / 2))/ temp;
	temp = 1024 - temp ;
  
	MSbyte = temp>>8;
	LSbyte = temp;
	
	S0RELH = MSbyte & 0x3;
	S0RELL = LSbyte;		
}

/************************************************************************
 * Function   	: uart0_init
 * Description	: uart0 initial for  baud_rate
 * Input 		: uint32_t baud_rate: Series rate 
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart0_init(uint32_t baud_rate)
{
 	
	PCLK0 |= (1<<0); 									       //开UART0时钟使能	
	PRESET0 |= (1<<0);										   //UART0正常工作
	EUARTEN |= (1<<0);										   //打开EUART0功能，P2.6作UART0_TX，P2.7作UART0_RX

  tx_flag = 0;
    
	SM0 = 0;
	SM1 = 1;		                                          //选择模式一
                
	TI0 = 0;                                                  //发送完成中断状态清除
	RI0 = 0;	                                              //接收完成中断状态清除
	
	uart0_set_baud_rate(system_core_clock, baud_rate);        //设置波特率
	REN0 = 1;                                                 //使能接收    
	
	P2PU &= ~(3<<6);												//使能P2.6,P2.7内部上拉  
}

/************************************************************************
 * Function   	: uart0_irq_init
 * Description	: uart0 interrupt enable
 * Input 		: uint8_t irqstate: 0 中断失能，1 中断使能
 * 		   		  *pfunc_recv)(): 接收处理回调函数
 * Output		: none
 * Return		: none
 ************************************************************************/
void uart0_irq_init(uint8_t irqstate,void (*pfunc_recv)())
{
    if(irqstate == UART0_IRQ_ENABLE)
    {
        EA = 1;						//EA总中断开启
        ES0 = 1;					//UART0中断使能   
        
        uart0_func = pfunc_recv;       
    } 
    else
    {
//      EA = 0;						//EA总中断关闭
        ES0 = 0;					//UART0中断关闭   
    } 
}

/************************************************************************
 * Function   	: uart0_send_byte
 * Description	: uart0 发送一个字节数据
 * Input 		: char c: out byte
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart0_send_byte(char c)
{
	//输出一个字符
	uint16_t timeout;
	timeout = 65535;
	
	S0BUF = c;

#ifdef UART0_TX_INT_MODE        	//中断方式
	while(!tx_flag);
	tx_flag = 0; 
#else 
	//查询方式
    while(!(TI0))               	//等待数据发送完成
	{
		if((timeout--) == 0)      	//超时等待退出
        {
			break;
        }
	}
	TI0 = 0;                   		//清除发送完成标志位
#endif
}

/************************************************************************
 * Function   	: uart0_send_bytes
 * Description	: uart0 send bytes	发送多个字节
 * Input 		: uint8_t* buff: out buffer
 *       		  uint32_t length: buffer length
 * Output		: none  
 * Return		: none
 ************************************************************************/
void uart0_send_bytes(uint8_t *buff, uint32_t length)
{
	while(length--)
	{
		uart0_send_byte(*buff++);
	}	
}

/************************************************************************
 * Function   	: uart0_recv_byte
 * Description	: uart0 接收一个字节数据
 * Input 		: none 
 * Output		: none  
* Return		: S0BUF：返回一个byte	
 ************************************************************************/
uint8_t uart0_recv_byte(void)
{ 
	return S0BUF;
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
	uart0_send_byte(c); 
	return c;
}


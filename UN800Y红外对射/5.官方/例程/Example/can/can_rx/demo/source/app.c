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
#include "common.h"
#include "config.h"
#include "uart0.h"
#include "can_rx.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

volatile uint8_t can_rx_flag ;
S_Can_Filter_Msg can_filter_msg;
S_Can_Tx_Msg can_tx_msg;
S_Can_Rx_Msg can_rx_msg;

/*用于测试接收的数据是否正确；接收和发送的数据保持一致,用户可自行设置*/
static uint8_t test_data[8] = {0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7}; 
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
 * function   : can_irq_rim
 * Description: can_irq_rim	接受中断服务函数
 * input : none
 * return: none
 ************************************************************************/
void can_irq_rim(void)
{
	can_rx_flag = 1;
}
/************************************************************************
 * function   : compare_data
 * Description: compare_data  比较数据
 * input : none
 * return: none
 ************************************************************************/
uint8_t compare_data(uint8_t *p, uint8_t *q, uint32_t len)
{
	uint32_t i;
	for(i=0; i<len; i++)
	{
		if(*p == *q)
		{
			p++;
			q++;
		}
		else
		{
			return 0;	//不相等
		}
	}
	return 1;			//相等
}

/************************************************************************
 * function   : user_app_pro
 * Description: user app pro , 添加用户代码
 * input : none
 * return: none
 ************************************************************************/
static void user_app_pro(void)
{
	uint8_t i;
	//以下代码为打印测试，用户可添加应用代码
	if(can_rx_msg.rtr)
	{//如果是遥控帧
		printfS("received remote frame!! \r\n");
		
		if(can_rx_msg.ide)
		{//扩展帧
			printfS("ext_id = 0x%x\r\n", can_rx_msg.ext_id);
		}
		else
		{//标准帧
			printfS("std_id = 0x%x\r\n", can_rx_msg.std_id);
		}
		printfS("ide = 0x%x\r\n", can_rx_msg.ide);
		printfS("rtr = 0x%x\r\n", can_rx_msg.rtr);
		
		
	}
	else
	{//如果是数据帧
		printfS("received data frame!! \r\n");
		
		if(can_rx_msg.ide)
		{//扩展帧
			printfS("ext_id = 0x%lx\r\n", can_rx_msg.ext_id);
		}
		else
		{//标准帧
			printfS("std_id = 0x%lx\r\n", can_rx_msg.std_id);
		}
		printfS("ide = 0x%bx\r\n", can_rx_msg.ide);
		printfS("rtr = 0x%bx\r\n", can_rx_msg.rtr);
		printfS("len = 0x%bx\r\n", can_rx_msg.len);
		printfS("recv data:\r\n");
		for(i=0;i<can_rx_msg.len;i++)
		{
			printfS("%02bx   ", can_rx_msg.recv_data[i]);
		}
		printfS("\r\n");
		if(compare_data(&can_rx_msg.recv_data[0], test_data, can_rx_msg.len))
		{
			printfS("can test data pass!! \r\n");
		}
		else
		{
			printfS("can test data error error!!!!! \r\n");
		}
		printfS("\r\n\r\n");
		
	}
}
/************************************************************************
 * function   : can_filter_set
 * Description: can_filter_set过滤器设置，用户可自行设置
 * input : none
 * return: none
 ************************************************************************/
void can_filter_set()
{
	/* 过滤器信息设置 */
	can_filter_msg.std_id1 = 0x592;					//使用的标准ID1
//	can_filter_msg.std_id2 = 0x127;					//使用的标准ID2 
//	can_filter_msg.ide = 0x00;					 	//标准模式
//	can_filter_msg.rtr = 0x00;				 		//接收的是数据帧
	
	can_filter_msg.ext_id1 = 0x11E8E477;			// 使用的扩展ID1
//	can_filter_msg.ext_id2 = 0x176054af;			// 使用的扩展ID2
	can_filter_msg.ide = 0x01;					 	// 扩展模式
	can_filter_msg.rtr = 0x00;				 		//接收的是数据帧
}
/************************************************************************
 * function   : can_test
 * Description: can test
 * input : none
 * return: none
 ************************************************************************/

void can_test(void)
{	
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test CAN rx function! \n");
	printfS("can rx test!!!!\r\n");
	
	can_init(CAN_RATE_500K);		//can初始化
	/*配置过滤器*/
	can_filter_set();
	can_filter_config(&can_filter_msg,CAN_DOUBLE_FILTER,CAN_FILTER_DISABLE); 

#ifdef CAN_RX_INT_MODE								//如果中断使能
	can_irq_init(CAN_IRQ_ENABLE, can_irq_rim);		//RI接收中断开启
#else
	can_irq_init(CAN_IRQ_DISABLE, NULL);			//RI接收中断关闭
#endif
	

	while(1)
	{
#ifdef CAN_RX_INT_MODE						//接收中断模式
		if(can_rx_flag)					
		{	 
			
			can_rx_flag = 0;
			can_recv_data(&can_rx_msg);
			user_app_pro();
			
		}
#else										//查询模式
		if(can_get_sr_reg()&CAN_SR_RBS)		//FIFO中有数据
		{
			while(can_get_rmc_reg())		//处理收到的数据帧
			{
				can_recv_data(&can_rx_msg);  
				user_app_pro();	
			}
		}
#endif

	}
}
/************************************************************************
 * function   : soc_test
 * Description: soc test
 * input : none
 * return: none
 ************************************************************************/
void soc_test(void)
{
	can_test(); 
 
}
/***************************file end************************************/





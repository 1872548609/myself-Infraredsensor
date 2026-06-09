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
#include "can_tx.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

volatile uint8_t can_tx_flag = 0;
S_Can_Filter_Msg can_filter_msg;
S_Can_Tx_Msg can_tx_msg;
S_Can_Rx_Msg can_rx_msg;
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
 * function   : can_irq_tim
 * Description: can_irq_tim
 * input : none
 * return: none
 ************************************************************************/
void can_irq_tim(void)
{
	can_tx_flag = 1;
}

/************************************************************************
 * function   : can_test
 * Description: can test
 * input : none
 * return: none
 ************************************************************************/
void can_test(void)
{
	uint8_t i;
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test CAN tx function! \n");
	printfS("can tx test!!!!\r\n");
	
	/* 发送信息设置 */
	
	//标准ID数据包设置
	can_tx_msg.std_id = 0x592;					//使用的标准ID1
	
	//扩展ID数据包设置
	can_tx_msg.ext_id = 0x11E8E477;				//使用的扩展ID1

	/*设置要发送的数据*/
	for(i=0; i<8; i++)
	{
		can_tx_msg.send_data[i] = 0xa0+i;		//数据
	}
	
	can_init(CAN_RATE_500K);						//CAN初始化
	
#ifdef CAN_TX_INT_MODE							//如果中断使能
	can_irq_init(CAN_IRQ_ENABLE, can_irq_tim);
#else
	can_irq_init(CAN_IRQ_DISABLE, can_irq_tim);
#endif

	while(1)
	{
		can_send_data(&can_tx_msg,CAN_IDE_STD_FORMAT, CAN_RTR_DATA,8);//帧发送，标准格式，数据帧，8个数据
		
		delay1ms(1000);
		printfS("can send data finish!\r\n");	
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


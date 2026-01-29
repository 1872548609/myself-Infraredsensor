/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-30
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "config.h"
#include "common.h"
#include "uart0.h"
#include "gtimer.h"

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
 * function   : gtimer0_ue_func
 * Description: gtimer0_ue_func GTIMER0 Update处理回调函数
 * input : none
 * return: none
 ************************************************************************/
void gtimer0_ue_func(void)
{
	printfS("gtimer0 update irq!!!\r\n");

}
/************************************************************************
 * function   : gtimer0_ccie_func
 * Description: gtimer0_ccie_func GTIMER0 Update处理回调函数
 * input : none
 * return: none
 ************************************************************************/
void gtimer0_ccie_func(void)
{
	printfS("gtimer0 ccie irq!!!\r\n");

}
/************************************************************************
 * function   : gtimer0_bke_func
 * Description: gtimer0_bke_func GTIMER0 刹车处理回调函数
 * input : none
 * return: none
 ************************************************************************/
void gtimer0_bke_func(void)
{
	printfS("gtimer0 bke irq!!!\r\n");

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
 * function   : gtimer0_count_test
 * Description: gtimer0_count_test GTIMER0定时器函数
 * input : none
 * return: none
 ************************************************************************/
void gtimer0_count_test(void)
{
	gtimer0_count_init(16000,1000-1);		//时钟源为系统时钟16M，1000分频，1S溢出产生中断
	
	gtimer0_irq_init(GTIMER_IRQ_ENABLE,GTIMER0_UIE_IRQ,gtimer0_ue_func);	//配置Update中断
	
	gtimer0_start();		//启动gtimer0计数
	
}


/************************************************************************
 * function   : gtimer0_pwm_test
 * Description: gtimer0_pwm_test GTIMER0 PWM输出函数
 * input : none
 * return: none
 ************************************************************************/
void gtimer0_pwm_test(void)
{
	gtimer0_pwm_init(160,10-1,80);	//10KHz PWM输出,占空比为50%  
	
	gtimer0_irq_init(GTIMER_IRQ_DISABLE,GTIMER0_CCIE_IRQ,gtimer0_ccie_func);		//配置CCIE中断
	
	gtimer0_start();		//启动gtimer0计数

}

/************************************************************************
 * function   : gtimer0_capture_test
 * Description: gtimer0_capture_test GTIMER0捕捉函数
 * input : none
 * return: none
 ************************************************************************/
void gtimer0_capture_test(void)
{
	gtimer0_capture_init(16000,GTIMER_DIV1,GTIMER0_CAP_CH);
	
	gtimer0_irq_init(GTIMER_IRQ_ENABLE,GTIMER0_CCIE_IRQ,gtimer0_ccie_func);		//配置CCIE中断
	
	gtimer0_start();		//启动gtimer0计数

}

/************************************************************************
 * function   : gtimer0_bke_test
 * Description: gtimer0_bke_test GTIMER0刹车函数
 * input : none
 * return: none
 ************************************************************************/	
void gtimer0_bke_test(void)
{
	gtimer0_bke_init();					//刹车初始化
	
	gtimer0_pwm_init(160,10-1,80);		//10KHz PWM输出,占空比为50%  
	
	gtimer0_irq_init(GTIMER_IRQ_ENABLE,GTIMER0_BKE_IE_IRQ,gtimer0_bke_func);	//配置刹车中断
	
	gtimer0_start();						//启动gtimer0计数

}


void gtimer0_test(void)
{
//	gtimer0_count_test();				//gtimer0 计数测试
	
	gtimer0_pwm_test();				//gtimer0 PWM输出测试
	
// gtimer0_capture_test();		//gtimer0 输入捕获测试
	
//	gtimer0_bke_test();				//gtimer0 刹车测试
}



void soc_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("gtimer0 test start\r\n");
	
	gtimer0_test();			
	
}


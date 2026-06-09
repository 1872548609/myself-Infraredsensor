/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : yanhaihua
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include "app.h"
#include "uart0.h"
#include "pwm.h"
#include "common.h"
#include "config.h"

volatile uint16_t duty0 = 70;	//占空比 = duty/cycle
volatile uint16_t cycle0 = 100;	//pwm频率=时钟频率/cycle

volatile uint16_t duty1 = 50;	//占空比 = duty/cycle
volatile uint16_t cycle1 = 100;	//pwm频率=时钟频率/cycle

volatile uint16_t duty2 = 30;	//占空比 = duty/cycle
volatile uint16_t cycle2 = 100;	//pwm频率=时钟频率/cycle

/************************************************************************
 * Function   	: uart_init
 * Description	: uart_init 串口初始化
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart_init(void)
{
	uart0_init(UART0_BAUD_RATE);

}


/***********************************************************************
 * Function   	: pwm0_irq_handler
 * Description	: pwm0中断回调函数
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
static void pwm0_irq_handler(void)
{
	printfS("PWM0 happen irq \r\n!!!!");
}

/***********************************************************************
 * Function   	: pwm0_test
 * Description	: 测试PWM0接口
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
static void pwm0_test(void)
{
	pwm0_init(cycle0, duty0, HIGH);  
	pwm0_irq_init(PWM_IRQ_DISABLE,pwm0_irq_handler);
	pwm0_start(); 
}

/***********************************************************************
 * Function   	: pwm1_irq_handler
 * Description	: pwm1中断回调函数
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
static void pwm1_irq_handler(void)
{
	printfS("PWM1 happen irq \r\n!!!!");
}

/***********************************************************************
 * Function   	: pwm1_test
 * Description	: 测试PWM1接口
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
static void pwm1_test(void)
{
	pwm1_init(cycle1, duty1, HIGH);   
	pwm1_irq_init(PWM_IRQ_DISABLE,pwm1_irq_handler);
	pwm1_start(); 
	
}

/***********************************************************************
 * Function   	: pwm2_irq_handler
 * Description	: pwm2中断回调函数
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
static void pwm2_irq_handler(void)
{
	printfS("PWM2 happen irq \r\n!!!!");
}

/***********************************************************************
 * Function   	: pwm2_test
 * Description	: 测试PWM2接口
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
static void pwm2_test(void)
{
	pwm2_init(cycle2, duty2,HIGH);
	pwm2_irq_init(PWM_IRQ_DISABLE,pwm2_irq_handler);
	pwm2_start();
}

/***********************************************************************
 * Function   	: app_run
 * Description	: app_run
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("PWM TEST START!!!\r\n");
	pwm0_test();		//P1_0输出pwm0
	pwm1_test();		//P1_1输出pwm1
	pwm2_test();		//P1_4输出pwm2
	delay1ms(100);

}

void soc_test(void)
{
	pwm_test();
}

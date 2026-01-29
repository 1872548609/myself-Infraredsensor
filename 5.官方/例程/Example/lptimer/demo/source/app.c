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
#include "uart0.h"
#include "lptimer.h"

volatile uint8_t flag_ov_int = 0;
volatile uint8_t flag_trig_int = 0;
volatile uint8_t flag_comp_int = 0;
volatile uint8_t flag_timeout = 0;
volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t count = 0;
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
 * Function   	: my_memcpy
 * Description	: my_memcpy
 * Input 		: uint8_t *dst	目的地址
 *				  uint8_t *src	源地址
 *				  uint16_t n   数据长度
 * Output 		: none
 * Return		: none
 ************************************************************************/
void my_memcpy(uint8_t *dst,uint8_t *src,uint16_t n)
{
	while(n--)
	{	
		*dst++ = *src++;
	}
}

/*******************************************************************
* Function		: lptim_comp_pro
* Description	: 比较匹配中断回调函数
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptim_comp_pro(void)
{
    flag_comp_int = 1;	
    printfS("Lptim comp irq!!\r\n");  
    
}
/*******************************************************************
* Function		: lptim_ov_pro
* Description	: 计时器溢出中断回调函数
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptim_ov_pro(void)
{ 
    flag_ov_int = 1;
    count++;
    printfS("Lptimer count times %u \r\n",count);   	        
}

void lptim_timerout_pro(void)
{
	flag_timeout = 1;
}
/*******************************************************************
* Function		: lptim_trig_pro
* Description	: 外部触发到来中断回调函数
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptim_trig_pro(void)
{
    flag_trig_int = 1;	
    printfS("Lptim trig irq!!\r\n");  
}

void lptim_capture1_pro(void)
{
	printfS("Lptim capture1 cnt value is %u!!\r\n",lptimer_get_lptcmp(CAP_CHANGE1));	
}

void lptim_capture2_pro(void)
{
	printfS("Lptim capture2 cnt value is %u!!\r\n",lptimer_get_lptcmp(CAP_CHANGE2));
}



/*******************************************************************
* Function		: lptim_timer
* Description	: lptim 普通定时器模式
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptim_timer_test(void)
{
	//普通定时器
	lptimer_init(TIMER_TMODE, LPTIMER_LSCLK, DIV1);               	//普通定时器模式
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_OV,lptim_ov_pro);    //计数器溢出中断设置
    lptimer_set_time(38000);                                        //定时1s     
    lptimer_start();                                                //定时启动
    printfS("lptimer timing start !!\r\n");   
	
	
}
void lptimer_trigger_test()
{
	//TRIGGER脉冲触发计数
	lptimer_init(TRIGGERCOUNT_TMODE, LPTIMER_SYSCLK, DIV128);       //TRIGGER计数模式
	lptimer_trigger_edge_set(DOWN);									//触发边沿
	lptimer_io_config(LPEXT);           							// P1_1作为EXTRIGGER_IO_IEN
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_TRIG,lptim_trig_pro);	//计数器溢出中断设置
	
	lptimer_set_time(65535);                                         	    
    lptimer_start();                                                //定时启动
    printfS("lptimer trigger test start !!\r\n");   
	
}

void lptimer_extcount_test()
{
	//外部异步脉冲计数  
	lptimer_init(EXTCOUNT_TMODE, LPTIMER_LPTIN, DIV1);             	//外部计数模式
	lptimer_lptin_edge_set(DOWN);									//触发边沿
	lptimer_io_config(LPIN);           								//P1_0作为LPIN
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_OV,lptim_ov_pro);	   	//计数器溢出中断设置
	
	lptimer_set_time(65535);                                              
    lptimer_start();                                                //定时启动
    printfS("lptimer extcount test start !!\r\n");   
	while(1)
	{
		printfS("extcount cnt=%u\r\n",lptim_get_cnt_value());
	}
}

void lptimer_timeout_test()
{
	// TIMREOUT 
	lptimer_init(TIMEOUT_TMODE, LPTIMER_LSCLK, DIV1);               	//timeout模式
	lptimer_io_config(LPEXT);           								// P1_1作为EXTRIGGER_IO
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_OV,lptim_timerout_pro);	//计数器溢出中断设置
	
	lptimer_set_time(65535);                                             	     
    lptimer_start();                                                  	//定时启动
    printfS("lptimer timerout test start !!\r\n");   
	
//	sys_entersleep();               //enter SLEEP mode
//	sys_enterdeepsleep();           //enter DEEPSLEEP mode
	
	while(!flag_timeout)
	{
		printfS("timer cnt=%u\r\n",lptim_get_cnt_value());
	}
		printfS("lptimer count time out\r\n");
	
}

/*******************************************************************
* Function		: lptim_pwm_test
* Description	: lptim pwm 输出
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptim_pwm_test(void)
{
    lptimer_pwm_init(LPTIMER_SYSCLK, DIV1);                                 //pwm输出初始化

#ifdef PWM1
	/* pwm通道1 */
	printfS("lptimer PWM1 test start!!\r\n");
	lptimer_pwm_channel_config(PWM_CHANGE1, POLAR1);						//通道1使能
	//pwm频率 =时钟频率/16000 =1000hz, 占空比 50%
	lptimer_pwm_set(PWM_CHANGE1,16000,50);									//通道1配置 
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_COMP1,lptim_comp_pro);        //计数器匹配中断设置
	lptimer_io_config(LPOUT1);                                               //P0_3作为LPOUT1
#endif	
	
#ifdef PWM2	
	//pwm通道2
	printfS("lptimer PWM2 test start!!\r\n");
	lptimer_pwm_channel_config(PWM_CHANGE2, POLAR1); 						//通道2使能
	//pwm频率 =时钟频率/8000 =2000hz, 占空比 30% 
	lptimer_pwm_set(PWM_CHANGE2,8000, 30);									//通道2
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_COMP2,lptim_comp_pro);        //计数器匹配中断设置 
	lptimer_io_config(LPOUT2);                                               //p04作为LPOUT2
#endif
	
  lptimer_start();                                                       	//定时启动 
	printfS("lptimer pwm test start !!\r\n"); 
	
}
/*******************************************************************
* Function		: lptim_capture_test
* Description	: lptim_capture 输入捕获
* Input			: none
* Output		: none
* Return		: none
********************************************************************/

void lptim_capture_test()
{
	lptimer_init(TIMER_TMODE, LPTIMER_SYSCLK, DIV128);                        	//普通定时器模式
	lptimer_set_time(65535);    
#ifdef CAPTURE1	
	printfS("lptimer capture1 test start!!\r\n");
	lptimer_capture_init(CAP_CHANGE1, EDGE_DOWN);
	lptimer_io_config(LPCAP1);           									 	//P1_2作为LPCAP
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_COMP1,lptim_capture1_pro);      	//捕获中断1初始化设置
#endif	
	
#ifdef CAPTURE2	
	printfS("lptimer capture2 test start!!\r\n");
	lptimer_capture_init(CAP_CHANGE2, EDGE_DOWN);
	lptimer_io_config(LPCAP2);           									 //P1_5作为LPCAP
	lptimer_irq_init(LPTIMER_IRQ_ENABLE,LPTIE_COMP2,lptim_capture2_pro); 		//捕获中断2初始化设置 
#endif
	lptimer_start();
	printfS("lptimer capture test start !!\r\n");  
}
/*******************************************************************
* Function		: lptimer_test
* Description	: lptimer_test
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptimer_test(void)
{    
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("lptimer test start!!\r\n");
	
//	lptim_timer_test();
//	lptimer_trigger_test();
//	lptimer_extcount_test();
//	lptimer_timeout_test();
	lptim_pwm_test();
//	lptim_capture_test();
}

void soc_test(void)
{
	lptimer_test();
}
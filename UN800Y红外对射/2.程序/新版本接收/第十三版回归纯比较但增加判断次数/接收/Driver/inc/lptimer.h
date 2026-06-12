/***********************************************************************
 * Copyright (c)  2019, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : lptimer.h
 * Description : lpTimer driver header file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2020-09-04
 ***********************************************************************/
#ifndef __LPTIMER_H__
#define __LPTIMER_H__

#include "um800y.h"

/*中断*/
#define LPTIMER_IRQ_DISABLE          0       /*中断失能*/
#define LPTIMER_IRQ_ENABLE           1       /*中断使能*/

/*中断使能*/
#define LPTIE_COMP1                  0       /*比较匹配中断使能 */
#define LPTIE_OV                     1       /*计数器溢出中断使能*/
#define LPTIE_TRIG                   2       /*外部触发到来中断使能*/
#define LPTIE_COMP2                  3       /*比较匹配2中断使能 */

        
/*时钟源选择*/           
#define LPTIMER_LSCLK                0       /*RCL低速时钟*/
#define LPTIMER_RCLP                 1       /*clk_1hz时钟*/
#define LPTIMER_SYSCLK               2       /*系统时钟*/
#define LPTIMER_LPTIN                3       /*外部输入时钟*/
            
/*计数时钟分频*/          
#define DIV1                         0
#define DIV2                         1
#define DIV4                         2
#define DIV8                         3
#define DIV16                        4
#define DIV32                        5
#define DIV64                        6
#define DIV128                       7
            

/*工作模式*/
#define TIMER_TMODE                  0       /*普通定时器模式*/
#define TRIGGERCOUNT_TMODE           1       /*Trigger脉冲触发计数模式*/
#define EXTCOUNT_TMODE               2       /*外部异步脉冲计数模式*/
#define TIMEOUT_TMODE                3       /*Timeout模式*/

/*计数模式*/
#define CONTINUOUS_MODE              0       /*连续计数模式*/
#define SINGLE_MODE                  1       /*单次计数模式*/

/*上升沿下降沿选择*/
#define UP                           0       /*上升沿计数*/
#define DOWN                         1       /*下降沿计数*/

/* PWM通道 */
#define PWM_CHANGE1					 1
#define PWM_CHANGE2					 2

/* CAPTURE通道 */
#define CAP_CHANGE1					 1
#define CAP_CHANGE2					 2

/* 极性 */
#define POLAR0						 0
#define POLAR1						 1

/* 捕获沿 */	
#define EDGE_UP						 0
#define EDGE_DOWN					 1
#define EDGE_UP_DOWN				 2

/* LPT_IO */	
#define LPIN					 	 0
#define LPEXT				 		 1
#define LPOUT1						 2
#define LPOUT2						 3
#define	LPCAP1						 4
#define	LPCAP2						 5


void lptimer_init(uint8_t tmode, uint8_t clock_sel, uint8_t div);
void lptimer_irq_init(uint8_t irqstate,uint8_t ie,void (*pfunc)());
void lptimer_pwm_init(uint8_t clock_sel, uint8_t div);
void lptimer_pwm_channel_config (uint8_t channel, uint8_t polar);
void lptimer_pwm_set(uint8_t changesel, uint32_t target, uint16_t duty);
void lptimer_set_time(uint16_t target);
void lptimer_capture_init(uint8_t channel, uint8_t edge);
void lptimer_start(void);
void lptimer_stop(void);
void lptimer_io_config(uint8_t io);
uint16_t lptim_get_cnt_value(void);
uint16_t lptimer_get_lptcmp(uint8_t channel);
void lptimer_trigger_edge_set(uint8_t edge);
void lptimer_lptin_edge_set(uint8_t edge);

#endif

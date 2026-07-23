/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : gpio.c
 * Description : gpio driver header file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2021-2-26
 ***********************************************************************/
#ifndef _GPIO_H_
#define _GPIO_H_

#include  "um800y.h"

#define P0_0						0
#define P0_1				    1
#define P0_2				    2
#define P0_3				    3
#define P0_4				    4
#define P0_5				    5
#define P1_0				    8
#define P1_1				    9
#define P1_2				    10
#define P1_3				    11
#define P1_4				    12
#define P1_5				    13
#define P2_0				    16
#define P2_2				    18
#define P2_3				    19
#define P2_5				    21
#define P2_6				    22
#define P2_7				    23

#define PORT0  0
#define PORT1  1
#define PORT2  2

#define GPIO_DIR_IN             1		/* 1：gpio方向为输入 */
#define GPIO_DIR_OUT  	        0		/* 1：gpio方向为输出 */

#define IN_ENABLE	        	1		/* 1：输入使能 */  
#define IN_DISABLE		    	0

#define GPIO_PU_ENABLE 	        0
#define GPIO_PU_DISABLE  		1       /* 1：关闭内部上拉电阻*/

#define GPIO_PD_ENABLE 	        1		/* 1：下拉使能 */  
#define GPIO_PD_DISABLE  		0

#define GPIO_OD_ENABLE 	        1		/* 1：开漏输出使能 */  
#define GPIO_OD_DISABLE  		0

#define GPIO_HIGH 	            1		/* 1：gpio输出高电平 */  
#define GPIO_LOW  	            0

#define GPIO_IRQ_ENABLE 		1		/* 1：中断使能 */              
#define GPIO_IRQ_DISABLE  	    0


#define GPIO_IEV_FALLEDG	  		0			//允许下降沿触发中断
#define GPIO_IEV_RISEEDG   			1			//允许上升沿触发中断
#define GPIO_IEV_RISE_FALL_EDG      2			//允许上升沿、下降沿触发中断


#define	GPIO_CS_CMOS				1	
#define GPIO_CS_SCHMITT			    0		/* 0：Schmitt input buffer */

#define	GPIO_DR_HIGH				0		/* 0：高驱动能力 */
#define	GPIO_DR_LOW					1				

#define	GPIO_SR_HIGH				0		/* 0：快速 */
#define	GPIO_SR_LOW					1		

void gpio_init(uint8_t pin);
void gpio_dir_set(uint8_t pin, uint8_t dir);
void gpio_in_enable(uint8_t pin, uint8_t mode);
void gpio_pu_set(uint8_t pin, uint8_t mode);
void gpio_pd_set(uint8_t pin, uint8_t mode);
void gpio_od_set(uint8_t pin, uint8_t mode);
void gpio_cs_set(uint8_t pin, uint8_t mode);
void gpio_dr_set(uint8_t pin, uint8_t mode);
void gpio_sr_set(uint8_t pin, uint8_t mode);
uint8_t gpio_io_get(uint8_t pin);
void gpio_io_set(uint8_t pin, uint8_t level);
void gpio_irq_set(uint8_t pin, uint8_t mode, void (*pfunc)());
void gpio_irq_clr(uint8_t pin);
uint8_t gpio_irq_get(uint8_t pin);


#endif







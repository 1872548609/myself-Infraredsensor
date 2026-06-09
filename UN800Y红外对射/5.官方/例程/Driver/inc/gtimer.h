/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : gtimer.h
 * Description : gtimer driver header file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-08
 ***********************************************************************/
#ifndef __GTIMER_H__
#define __GTIMER_H__

#include "um800y.h"

#define GTIMER_IRQ_ENABLE  1				//gtimer0中断使能

#define GTIMER_IRQ_DISABLE  0				//gtimer0中断关闭



#define GTIMER0_UIE_IRQ (1)
#define GTIMER0_CCIE_IRQ (2)
#define GTIMER0_BKE_IE_IRQ (3)


#define GTIMER1_UIE_IRQ (1)
#define GTIMER1_CCIE_IRQ (2)
#define GTIMER1_BKE_IE_IRQ (3)


#define GTIMER2_UIE_IRQ (1)
#define GTIMER2_CCIE_IRQ (2)
#define GTIMER2_BKE_IE_IRQ (3)

/********        GTIM_IER寄存器         ********/
#define GTIMER0_UIE	        		0x1		    //UPTADE事件中断
#define GTIMER0_CCIE		        0x2	    //捕捉/比较事件中断
#define GTIMER0_BKE_IE	        0x4		    //刹车事件中断


#define GTIMER0_UIF	        		0x1		    //UPTADE事件
#define GTIMER0_CCIF		        0x2	    	//捕捉/比较事件
#define GTIMER0_BKE_IF	        0x4		    //刹车事件


#define GTIMER1_UIE	        		0x1		    //UPTADE事件中断
#define GTIMER1_CCIE		        0x2	    //捕捉/比较事件中断
#define GTIMER1_BKE_IE	        0x4	    //刹车事件中断

#define GTIMER1_UIF	        		0x1		    //UPTADE事件
#define GTIMER1_CCIF		        0x2	    //捕捉/比较事件
#define GTIMER1_BKE_IF	        0x4	    //刹车事件


#define GTIMER2_UIE	        		0x1		    //UPTADE中断
#define GTIMER2_CCIE		        0x2		    //捕捉/比较中断
#define GTIMER2_BKE_IE	        0x4	    //刹车中断

#define GTIMER2_UIF	        		0x1		    //UPTADE事件
#define GTIMER2_CCIF		        0x2		    //捕捉/比较事件
#define GTIMER2_BKE_IF	        0x4	    	//刹车事件

/********     GTIMER捕捉源预分频位    ********/
#define GTIMER_DIV1    0
#define GTIMER_DIV2    1
#define GTIMER_DIV4    2
#define GTIMER_DIV8    3


/********      GTIMER0捕捉源选择       ********/
#define GTIMER0_CAP_CH     				(0x0)
#define GTIMER0_CAP_UART0_RX     	(0x1)
#define GTIMER0_CAP_CLK38K     		(0x2)
#define GTIMER0_CAP_LPTIM_LPOUT0  (0x3)

/********      GTIMER1捕捉源选择       ********/
#define GTIMER1_CAP_CH     				(0x0)
#define GTIMER1_CAP_UART1_RX     	(0x1)
#define GTIMER1_CAP_CLK38K     		(0x2)
#define GTIMER1_CAP_LPTIM_LPOUT1  (0x3)


/********      GTIMER2捕捉源选择       ********/
#define GTIMER2_CAP_CH     				(0x0)
#define GTIMER2_CAP_UART2_RX     	(0x1)
#define GTIMER2_CAP_CLK38K     		(0x2)
#define GTIMER2_CAP_UART3_RX 			(0x3)



/***gtimer0***/
void gtimer0_count_init(uint16_t arr,uint16_t psc);
void gtimer0_pwm_init(uint16_t arr,uint16_t psc,uint16_t ccr);
void gtimer0_capture_init(uint16_t arr,uint8_t capture_div,uint8_t capssel);
void gtimer0_bke_init(void);
void gtimer0_irq_init(uint8_t irq_enable,uint8_t gtimer_irq_type,void (*pfunc)());
void gtimer0_start(void);
void gtimer0_stop(void);

/***gtimer1***/
void gtimer1_count_init(uint16_t arr,uint16_t psc);
void gtimer1_pwm_init(uint16_t arr, uint16_t psc, uint16_t ccr);
void gtimer1_capture_init(uint16_t arr,uint8_t capture_div,uint8_t capssel);
void gtimer1_bke_init(void);
void gtimer1_irq_init(uint8_t irq_enable,uint8_t gtimer_irq_type,void (*pfunc)());
void gtimer1_start(void);
void gtimer1_stop(void);

/***gtimer2***/
void gtimer2_count_init(uint16_t arr,uint16_t psc);
void gtimer2_pwm_init(uint16_t arr, uint16_t psc, uint16_t ccr);
void gtimer2_capture_init(uint16_t arr,uint8_t capture_div,uint8_t capssel);
void gtimer2_bke_init(void);
void gtimer2_irq_init(uint8_t irq_enable,uint8_t gtimer_irq_type,void (*pfunc)());
void gtimer2_start(void);
void gtimer2_stop(void);



#endif




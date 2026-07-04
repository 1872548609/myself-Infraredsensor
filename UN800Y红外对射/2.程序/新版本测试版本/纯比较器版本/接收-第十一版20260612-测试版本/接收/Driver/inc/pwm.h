/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : pwm.h
 * Description : pwm header file
 * Author(s)   : yanhaihua
 * version     : V1.0
 * Modify date : 2021-02-26
 ***********************************************************************/
#ifndef __PWM_H__
#define __PWM_H__

#include "system_um800y.h"
#include "config.h"

#define HIGH   						1
#define LOW    						0

typedef enum
{
	PWM_IRQ_ENABLE = 0,
	PWM_IRQ_DISABLE,
}em_pwm_irq;

void pwm0_init(uint16_t cycle, uint16_t duty, uint8_t level);
void pwm0_irq_init(em_pwm_irq mode,void (*pfunc)());
void pwm0_start(void);
void pwm0_stop(void); 

void pwm1_init(uint16_t cycle, uint16_t duty, uint8_t level);
void pwm1_irq_init(em_pwm_irq mode,void (*pfunc)());
void pwm1_start(void);
void pwm1_stop(void); 

void pwm2_init(uint16_t cycle, uint16_t duty, uint8_t level);
void pwm2_irq_init(em_pwm_irq mode,void (*pfunc)());
void pwm2_start(void);
void pwm2_stop(void); 

#endif
/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.h
 * Description : app header file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#ifndef __APP_H__
#define __APP_H__

#include "config.h"


#define PWM1
//#define PWM2
#define CAPTURE1
//#define CAPTURE2

void uart0_rec_pro(void);

void uart_init(void);

void lptimer_test(void);

void soc_test(void);


#endif


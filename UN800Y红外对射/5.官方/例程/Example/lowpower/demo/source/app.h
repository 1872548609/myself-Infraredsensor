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

#include  "common.h"
void gpio_enable_interrupt(void);
void lowpower_test(void);
void active_test(void);
void clk_out_test(void);

#endif


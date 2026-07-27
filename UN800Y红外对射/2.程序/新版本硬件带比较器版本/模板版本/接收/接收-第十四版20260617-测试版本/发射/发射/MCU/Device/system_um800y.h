/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : system_um800y.h
 * Description : system config header file (for example: clock config.....)
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#ifndef __SYSTEM_UM800Y_H__
#define __SYSTEM_UM800Y_H__

#include  "um800y.h"

extern uint32_t system_core_clock;

void clock_init(uint32_t system_clk_hz);

void system_init(void);


#endif

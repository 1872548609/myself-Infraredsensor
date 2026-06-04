/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : beeper.h
 * Description : beeper driver header file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2020-07-28
 ***********************************************************************/
#ifndef _BEEPER_H_
#define _BEEPER_H_

#include "um800y.h"

#define HIGH 	  1
#define LOW  	  0

#define SEL_1KHz 	0
#define SEL_2KHz  	1
#define SEL_4KHz  	2

void beeper_init(void);

void beeper_level_set(uint8_t level,uint8_t sel);
#endif





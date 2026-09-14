/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : wdt.h
 * Description : wdt driver header file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-30
 ***********************************************************************/
#ifndef _WDT_H_
#define _WDT_H_

#include "um800y.h"

#define  WDT_ARR_4096_MS  				0
#define  WDT_ARR_1024_MS  				1
#define  WDT_ARR_256_MS   				2
#define  WDT_ARR_128_MS   				3
#define  WDT_ARR_64_MS    				4
#define  WDT_ARR_16_MS    				5
#define  WDT_ARR_4_MS     				6
#define  WDT_ARR_1_MS     				7

void wdt_init(void);

void wdt_load(uint8_t arr);  											/* 设置溢出周期最小值 */

void wdt_feed(uint8_t arr);												/* 喂狗 */


#endif




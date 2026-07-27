/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : wdt.c
 * Description : wdt driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-30
 ***********************************************************************/
#include  "wdt.h"

/***********************************************************************
 * Function     : wdt_init
 * Description  : wdt init
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void wdt_init(void)
{
	PCLK0 |= (1<<2); 		   													// 开启WDT时钟 
	PRESET0 |= (1<<2);         											// WDT正常工作 
	WDEN = 1;                  											// WDT使能 
}

/***********************************************************************
 * Function     : wdt_load
 * Description  : 设置溢出周期最小值
 * Input        : uint8_t arr：arr_1_ms，arr_4_ms...
 * Output		: none
 * Return		: none
 ***********************************************************************/
void wdt_load(uint8_t arr)
{
  RSTSTAT &= ~(7<<0);  
	RSTSTAT |= (arr<<0);
}


/***********************************************************************
 * Function     : wdt_feed
 * Description  : 喂狗
 * Input        : uint8_t arr：arr_1_ms，arr_4_ms...
 * Output		: none
 * Return		: none
 ***********************************************************************/
void wdt_feed(uint8_t arr)
{
	RSTSTAT &= ~(7<<0); 
	RSTSTAT |= (arr<<0);
}



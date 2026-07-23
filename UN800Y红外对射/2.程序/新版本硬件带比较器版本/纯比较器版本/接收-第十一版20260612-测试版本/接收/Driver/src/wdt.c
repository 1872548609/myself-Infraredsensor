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
    /*
     * UM800Y 的 WDT 使用内部 RC38K。
     * CLKCON bit3 = RC38KEN，必须在使能 WDT 前打开。
     */
    CLKCON |= (1 << 3);
    
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
    arr &= 0x07;

    /*
     * 手册说明：读或写 RSTSTAT 都会自动清除 WDT 计数。
     * 同时更新 WDT[2:0] 超时周期。
     */
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
     arr &= 0x07;

    /*
     * 保持原驱动接口：每次喂狗时重新写入超时档位。
     */
    
	RSTSTAT &= ~(7<<0); 
	RSTSTAT |= (arr<<0);
}



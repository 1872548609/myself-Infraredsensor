/***********************************************************************
 * Copyright (c) 2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : wdt.c
 * Description : wdt driver source file
 *
 * 修改：
 * - 根据 UM800Y 手册，在使能 WDT 前显式打开 RC38K。
 * - 对 arr 做 3 bit 限制，避免非法值影响其它复位状态位。
 ***********************************************************************/
#include "wdt.h"

void wdt_init(void)
{
    /*
     * UM800Y 的 WDT 使用内部 RC38K。
     * CLKCON bit3 = RC38KEN，必须在使能 WDT 前打开。
     */
    CLKCON |= (1 << 3);

    PCLK0  |= (1 << 2);    /* 开启 WDT 外设时钟 */
    PRESET0 |= (1 << 2);   /* 释放 WDT 复位 */
    WDEN = 1;              /* 使能 WDT */
}

void wdt_load(uint8_t arr)
{
    arr &= 0x07;

    /*
     * 手册说明：读或写 RSTSTAT 都会自动清除 WDT 计数。
     * 同时更新 WDT[2:0] 超时周期。
     */
    RSTSTAT &= ~(0x07);
    RSTSTAT |= arr;
}

void wdt_feed(uint8_t arr)
{
    arr &= 0x07;

    /*
     * 保持原驱动接口：每次喂狗时重新写入超时档位。
     */
    RSTSTAT &= ~(0x07);
    RSTSTAT |= arr;
}

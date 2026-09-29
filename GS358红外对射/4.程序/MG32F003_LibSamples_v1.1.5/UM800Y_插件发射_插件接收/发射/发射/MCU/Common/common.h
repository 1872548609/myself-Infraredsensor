/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : common.h
 * Description : common header file
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#include  "um800y.h"
#include  "intrins.h"

void delay1ms(uint32_t n);
void printf_buff_byte(uint8_t* buff, uint32_t length);
void printf_buff_word(uint32_t* buff, uint32_t length);
void delay(uint32_t i);

#endif

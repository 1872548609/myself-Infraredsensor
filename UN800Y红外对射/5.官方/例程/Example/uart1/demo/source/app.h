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


void uart0_rec_pro(void);

void uart_test(void);

void uart_init(void);

void my_memcpy(uint8_t *dst,uint8_t *src,uint16_t n);

#endif


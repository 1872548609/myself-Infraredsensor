/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart1.h
 * Description : uart driver header file
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#ifndef __UART1_H__
#define __UART1_H__

#include "um800y.h"

#define UART1_IRQ_DISABLE      0
#define UART1_IRQ_ENABLE       1

void uart1_init(uint32_t baud_rate);

void uart1_send_byte(char c);

void uart1_send_bytes(uint8_t *buff, uint32_t length);

void uart1_irq_init(uint8_t irqstate,void (*pfunc_recv)());

uint8_t uart1_recv_byte(void);

#endif


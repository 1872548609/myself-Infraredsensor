/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart0.h
 * Description : uart driver header file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#ifndef __UART0_H__
#define __UART0_H__

#include "um800y.h"

#define UART0_IRQ_DISABLE      0
#define UART0_IRQ_ENABLE       1

void uart0_init(uint32_t baud_rate);

void uart0_send_byte(char c);

void uart0_send_bytes(uint8_t *buff, uint32_t length);

void uart0_irq_init(uint8_t irqstate,void (*pfunc_recv)());

uint8_t uart0_recv_byte(void);

#endif

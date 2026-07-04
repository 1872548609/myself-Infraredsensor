/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : uart3.h
 * Description : uart driver header file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-09
 ***********************************************************************/
 
#ifndef __UART3_H__
#define __UART3_H__

#include "um800y.h"

#define UART3_IRQ_DISABLE      0
#define UART3_IRQ_ENABLE       1
#define UART3_BAUD_RATE		   115200
//#define UART3_TX_INT_MODE   				//TX采用中断方式（程序中RX始终采用中断方式）

void uart3_init(uint32_t baud_rate);

void uart3_irq_init(uint8_t irqstate,void (*pfunc_recv)());

void uart3_send_byte(char c);

void uart3_send_bytes(uint8_t *buff, uint32_t length);

uint8_t uart3_recv_byte(void);

#endif
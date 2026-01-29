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


#define LEN		8
#define SPI_IRQ_MODE 	

#define FULL_DUPLEX_MODE
//#define HALF_DUPLEX_MODE


void uart0_rec_pro(void);

void uart_init(void);

void spi_test(void);

void spi_irq_func(void);

void soc_test(void);

#endif


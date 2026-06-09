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

#include "config.h"

#define march_test   

void uart0_rec_pro(void);

void uart_init(void);

void iram_bdata_test(void);
void iram_idata_test(void);
void iram_data_test(void);
void xram_xdata_test(void);
void iram_bdata_bit_test(void);
uint8_t mem_test_march_C2_bit8 (uint8_t startaddr, uint8_t length, uint8_t value_0,uint8_t value_1);
void ram_test(void);	
void soc_test(void);

#endif


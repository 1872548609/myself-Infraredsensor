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

#define DATA_SIZE   20


void uart0_rec_pro(void);

void uart0_send_pro(void);

void uart_init(void);

void eflash_test(void);

void eflash_page_check_8bits(uint16_t base_addr,uint8_t w_val, uint8_t page);
void eflash_page_check_bytes(uint16_t base_addr,uint8_t w_val, uint32_t length);
void eflash_page_check_byte(uint16_t base_addr, uint8_t start_page, uint8_t num_page);

#endif


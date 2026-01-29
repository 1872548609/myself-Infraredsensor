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

#define CAN_RX_INT_MODE		  

void uart0_rec_pro(void);

void uart_init(void);

void can_reg_test(void);

void soc_test(void);


#endif


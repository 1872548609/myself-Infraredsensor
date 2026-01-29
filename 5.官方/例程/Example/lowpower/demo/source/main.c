/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/     
#include "system_um800y.h"
#include "app.h"
#include "common.h"
#include "uart0.h"
#include "config.h"
void main(void)  
{  		
	system_init();
	
	uart0_init(UART0_BAUD_RATE);

  delay1ms(3000);
	
	lowpower_test();
    
	while(1) 
	{
		uart0_send_byte(0x5a);
		delay1ms(1000);
	}
}
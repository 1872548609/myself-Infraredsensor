/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#include  "system_um800y.h"           
#include  "app.h"
#include  "common.h"
#include  "uart0.h"

void main(void)  
{  		
	system_init();
	
	uart_init();
	
	beeper_test();	
    
	while(1) 
	{
	
	}
}
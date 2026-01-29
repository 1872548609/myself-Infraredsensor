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

void main(void)  
{  		
	system_init();
	
	uart_init();
	
	uart_test();	
    
	while(1) 
	{
	
	}
}
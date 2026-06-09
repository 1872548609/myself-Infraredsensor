/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-30
 ***********************************************************************/     
#include "system_um800y.h"
#include "app.h"


void main(void)  
{  		
	system_init();
	
	uart_init();
	
	soc_test();
    
	while(1) 
	{
	
	}
}
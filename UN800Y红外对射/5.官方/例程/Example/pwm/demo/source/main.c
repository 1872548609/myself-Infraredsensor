/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include "app.h" 
#include "system_um800y.h"


/***********************************************************************
 * Function   	: main
 * Description	: main
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void main(void)  
{  		
	system_init();														/* 初始化系统时钟 */
	
	uart_init();	
    
	soc_test();
		
	while(1) 
	{
		
	}
}
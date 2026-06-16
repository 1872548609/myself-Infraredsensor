/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : system_um800y.c
 * Description : system config source file (for example: clock config.....)
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
 
#include "system_um800y.h"
#include "config.h"

volatile uint32_t system_core_clock = 0;   			//core时钟 , (uint:Hz)
volatile uint32_t xdata rch24m = 0;

/************************************************************************
 * Function   	: clock_init
 * Description	: clock init, initil several clock variables
 * Input 		: uint32_t system_clk_hz
 * Output 		: none
 * Return		: none
 ************************************************************************/


void clock_init(uint32_t system_clk_hz)
{	
	uint32_t crystal_clk = 0;    
	CLKCON |= (0x1<<2);			   //RCH时钟源打开
	while((CLKCON&0x20) != 0x20);  //等待RC16M时钟稳定


	rch24m = ((uint32_t)(*(volatile uint8_t xdata*)(0x91AB))<<24) |  \
		     ((uint32_t)(*(volatile uint8_t xdata*)(0x91AA))<<16) |  \
			 ((uint32_t)(*(volatile uint8_t xdata*)(0x91A9))<<8)  |  \
			  (uint32_t)(*(volatile uint8_t xdata*)(0x91A8));   
	   		
	if ((rch24m <= 24120000) && (rch24m >= 23880000))    //实际值如果是24M的-+0.5%以内
    {
        crystal_clk = 24000000*2;      
    }
	else if(rch24m == 0xFFFFFFFF)
	{
		crystal_clk = 24000000*2;
	}
    else
	{
		crystal_clk = rch24m*2;  
	}    
    switch(system_clk_hz)
    {
        case 24000000:
            RCHDIV = (0x1<<0);                		//RCH 2分频
            system_core_clock = crystal_clk/2;
            break;
        case 16000000:
            RCHDIV = (0x2<<0);                		//RCH 3分频
            system_core_clock = crystal_clk/3;
            break;
        case 12000000:
            RCHDIV = (0x3<<0);                		//RCH 4分频
            system_core_clock = crystal_clk/4;
            break;
        default:
            RCHDIV = (0x2<<0);                		//RCH 3分频
            system_core_clock = crystal_clk/3;
            break;
           
    }
							
	CLKCON &= ~(0x1<<0);					//RCH为系统时钟
	SYSDIV &= ~(7<<0);						//系统时钟不分频			
}

/************************************************************************
 * Function   	: system_init
 * Description	: system_init
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void system_init(void)
{
    clock_init(FCLK); 
}


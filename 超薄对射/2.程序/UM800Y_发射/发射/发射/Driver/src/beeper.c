/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : beeper.c
 * Description : wdt driver source file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2020-07-28
 ***********************************************************************/
#include  "beeper.h"


/************************************************************************
 * function   : beeper_init
 * Description: beeper init
 * input : none
 * Output: none
 * return: none
 ************************************************************************/
void beeper_init(void)
{
	 BEEPCTR |= (1<<4);         //BEEPER使能,P2.5作为BEEP功能
}

/************************************************************************
 * function   : beeper_level_set
 * Description: set beeper output polarity 
 * input : level ：0 低电平
                   1 高电平
 *         sel
 * Output: none
 * return: none
 ************************************************************************/
void beeper_level_set(uint8_t level,uint8_t freq)
{
	if(LOW == level)
	{
	    BEEPCTR &= ~(1<<2);         //BEEP输出低电平
	}
	else
	{
	    BEEPCTR |= (1<<2);         //BEEP输出高电平
	}
	BEEPCTR &= ~(3<<0);
	BEEPCTR |= (freq<<0);          //输出频率
}



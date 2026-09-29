/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : common.c
 * Description : conmmon source file
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#include "common.h"
#include  "config.h"

/************************************************************************
 * Function   	: delay1ms
 * Description	: delay 1ms
 * Input 		: uint32_t n: n ms
 * Output 		: none
 * Return		: none
 ************************************************************************/
void delay1ms(uint32_t n)		//@16M Hz
{
	uint32_t x;
	uint8_t i;
	uint8_t j;
	for(x = 0; x<=n; x++)
	{
		_nop_();
		i = 21;
		j = 146;
		do
		{
			while (--j);
		} while (--i);
	}
}

/************************************************************************
 * Function   	: delay
 * Description	: delay
 * Input 		: uint32_t i
 * Output 		: none
 * Return		: none
 ************************************************************************/
void delay(uint32_t i)
{
		while(i--);
}

#ifdef DEBUG
/************************************************************************
 * Function   	: printf_buff_byte
 * Description	: printf data block by byte
 * Input 		: uint8_t* buff: buff
 *        		  uint32_t length: byte length
 * Output 		: none
 * Return		: none
 ************************************************************************/
void printf_buff_byte(uint8_t* buff, uint32_t length)
{
	uint32_t i;

	for(i=0; i<length; i++)
	{
		printf("%.2bx ",buff[i]);	
	}
	printf("\n");
}

/************************************************************************
 * Function   	: printf_buff_word
 * Description	: printf data block by word
 * Input 		: uint8_t* buff: buff
 *         		  uint32_t length: word length
 * Output 		: none
 * Return		: none
 ************************************************************************/
void printf_buff_word(uint32_t* buff, uint32_t length)
{
	uint32_t i;

	for(i=0; i<length; i++)
	{
		printf("%.8lx ",buff[i]);	
	}
	printf("\n");
}

#endif

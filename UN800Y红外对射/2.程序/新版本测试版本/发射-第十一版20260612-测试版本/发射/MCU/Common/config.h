/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : config.h
 * Description : config header file
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>							//printf .....
#include <string.h>


#define DEBUG    							//printf调试接口使能

/***********************************************************************
**	%bx :8位16进制输出
**	%bd :8位10进制输出
**	%x  :16位16进制输出
**	%d  :16位10进制输出
**	%lx :32位16进制输出
**	%ld :32位10进制输出
 ***********************************************************************/
#ifdef DEBUG
#define printfS   		  printf
#define printfB8   		  printf_buff_byte
#define printfB32   	  printf_buff_word	 
#else
#define	printfS		
#define	printfB8(buff, byte_len)	
#define	printfB32(buff, word_len)	
#endif

/*--------------- 时钟设置 ----------------------- */
#define FCLK          		24000000      	//目标时钟

/*--------------- uart设置 ----------------------- */
//通信格式采用：8位数据位，1位停止位，无校验位

#define UART0_BAUD_RATE		115200
#define UART1_BAUD_RATE		115200
//#define UART0_TX_INT_MODE   				//TX采用中断方式（程序中RX始终采用中断方式）
#endif

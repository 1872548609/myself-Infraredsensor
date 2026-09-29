/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : types.h
 * Description : types header file
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#ifndef __TYPES_H__
#define __TYPES_H__

/*
//KEILμVision4软件
//char		  占用1字节
//short int	  占用2字节
//int		  占用2字节
//long int	  占用4字节
//long		  占用4字节
//float	  	  占用4字节
//double	  占用4字节	
//其余编译器请使用 sizeof()自行查询
*/


typedef signed   char int8_t;
typedef signed   char INT8_T;
typedef unsigned char uint8_t;
typedef unsigned char UINT8_T;
typedef unsigned char byte;
typedef unsigned char BYTE;
typedef unsigned char uchar;


typedef  int   int16_t;
typedef  int   INT16_T;
typedef unsigned int uint16_t;
typedef unsigned int UINT16_T;
typedef unsigned int WORD;
typedef unsigned int word;

typedef   long   int32_t;
typedef   long   INT32_T;
typedef unsigned long uint32_t;
typedef unsigned long UINT32_T;
typedef unsigned long DWORD;
typedef unsigned long dword;

typedef    float      fp32;

#define TRUE  1
#define FALSE 0

#undef NULL
#define NULL 0  

#endif 





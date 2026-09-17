/************************************************************************************************/
/**
* @file               uart_bsp.h
* @author             MCU Ecosystem Development Team
* @brief              UART BSP头文件。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef UART_BSP_H
#define UART_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"
/*-------------------------------------------functions------------------------------------------*/

void UART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_BSP_H */

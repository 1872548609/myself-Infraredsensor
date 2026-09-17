/************************************************************************************************/
/**
* @file               common.h
* @author             MCU Ecosystem Development Team
* @brief              COMMON头文件。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"
#include <stdio.h>
    
/*--------------------------------------------define--------------------------------------------*/
/* LED IO相关定义 */
#define LED_GPIO_PORT           GPIOB
#define LED_PIN                 GPIO_PIN_1
    
/*-------------------------------------------functions------------------------------------------*/
void system_clock_config(void);
void gpio_init(void);
void crc_init(void);


#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */


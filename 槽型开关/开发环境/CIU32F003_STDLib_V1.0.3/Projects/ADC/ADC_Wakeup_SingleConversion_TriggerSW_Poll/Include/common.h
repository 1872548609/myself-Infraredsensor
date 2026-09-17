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


/*避免头文件重复引用*/
#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include <stdio.h>
#include "ciu32f003_std.h"


/*--------------------------------------------define--------------------------------------------*/
/* USER BUTTON定义 */
#define USER_BUTTON_GPIO_PORT             GPIOB
#define USER_BUTTON_PIN                   GPIO_PIN_2
    
/* USER BUTTON EXTI定义*/    
#define USER_BUTTON_EXTI_PORT             EXTI_GPIOB
#define USER_BUTTON_EXTI_LINE             EXTI_LINE_GPIO_PIN2
    
    
/*-------------------------------------------functions------------------------------------------*/
void system_clock_config(void);    
void gpio_init(void); 
void nvic_init(void);   
void adc_init(void);
void uart_init(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */


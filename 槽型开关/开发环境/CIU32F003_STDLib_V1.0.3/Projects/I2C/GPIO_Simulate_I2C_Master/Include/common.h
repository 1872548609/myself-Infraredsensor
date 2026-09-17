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

/*-------------------------------------------define---------------------------------------------*/
#define I2C_PIN_PORT            GPIOB
#define SCL_PIN                 GPIO_PIN_3
#define SDA_PIN                 GPIO_PIN_4
    
/*-------------------------------------------functions------------------------------------------*/
void system_clock_config(void);
void gpio_init(void);
void error_process(void);
void i2c_delay_time(uint32_t delay_time);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */


/************************************************************************************************/
/**
* @file               exti_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              EXTI BSP驱动函数，实现EXTI功能配置。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/

#include "exti_bsp.h"

/*------------------------------------------variables-------------------------------------------*/
extern uint32_t g_exti_gpio_flag;

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  EXTI2_3中断服务函数
* @retval 无
*/
void EXTI2_3_IRQHandler(void)
{
    /* 读取EXTI通道中断挂起状态 */
    if (std_exti_get_pending_status(USER_BUTTON_EXTI_LINE))
    {
        /* 清除EXTI通道中断挂起状态 */
        std_exti_clear_pending(USER_BUTTON_EXTI_LINE);
        g_exti_gpio_flag = 1;
    }
}





/************************************************************************************************/
/**
* @file               pmu_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              PMU BSP驱动函数，实现EXTI中断唤醒流程。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "pmu_bsp.h"

/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  GPIO_PIN_2到GPIO_PIN_3中断服务函数
* @retval 无
*/
void EXTI2_3_IRQHandler(void)
{
    /* 检测EXTI Line 中断 */
    if (std_exti_get_pending_status(USER_BUTTON_EXTI_LINE))
    {
        std_exti_clear_pending(USER_BUTTON_EXTI_LINE);
    }
    
    /* 用户根据EXTI的中断事件，结合当前的状态实现中断处理函数 */    
}


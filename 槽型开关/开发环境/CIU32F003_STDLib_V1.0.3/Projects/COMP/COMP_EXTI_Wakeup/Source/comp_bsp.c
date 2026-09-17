/************************************************************************************************/
/**
* @file               comp_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              COMP BSP驱动函数。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "comp_bsp.h"

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  COMP EXTI中断服务函数
* @retval 无
*/
void ADC_COMP_IRQHandler(void)
{
    if(std_exti_get_pending_status(EXTI_LINE_COMP1))
    {
        /* 关闭EXTI COMP1通道唤醒中断源 */
        std_exti_interrupt_disable(EXTI_LINE_COMP1);
        /* 清除EXTI上升沿挂起状态 */
        std_exti_clear_pending(EXTI_LINE_COMP1);
        /* 用户可根据自己的需求进行处理 */
    }
}

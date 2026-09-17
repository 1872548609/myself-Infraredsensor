/************************************************************************************************/
/**
* @file               tim_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              TIM BSP驱动函数，实现TIM中断方式计数。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/


/*------------------------------------------includes--------------------------------------------*/
#include "tim_bsp.h"

/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  TIM1中断服务程序
* @retval 无
*/
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    /* TIM1更新中断处理流程 */
    if (std_tim_get_interrupt_enable(TIM1, TIM_INTERRUPT_UPDATE))
    {
        std_tim_clear_flag(TIM1, TIM_FLAG_UPDATE);
        
        /* TIM1处理定时中断 */
    }
}

/**
* @brief  配置TIM1中断，并启动计数
* @retval 无
*/
void bsp_tim1_start_interrupt(void)
{
    /* 使能更新中断 */
    std_tim_interrupt_enable(TIM1, TIM_INTERRUPT_UPDATE);
    
    /* 开启定时器计数 */
    std_tim_enable(TIM1);
}


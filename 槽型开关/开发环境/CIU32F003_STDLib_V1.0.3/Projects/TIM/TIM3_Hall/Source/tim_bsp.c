/************************************************************************************************/
/**
* @file               tim_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              TIM BSP驱动函数，实现TIM霍尔接口配置功能。
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


/*-------------------------------------------define---------------------------------------------*/
/* 获取CC1的数值 */
#define CC1_VALUE_NUM                 (6U)


/*--------------------------------------------variables-----------------------------------------*/
__IO uint32_t g_loop_number = 0U;
__IO uint32_t g_ch1_cc1_value[CC1_VALUE_NUM];


/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  TIM3中断处理程序
* @retval 无
*/
void TIM3_IRQHandler(void)
{
    /* 触发中断处理流程 */
    if (std_tim_get_flag(TIM3, TIM_FLAG_TRIG))
    {
        /* 清除触发事情标志，并读取CC1的值 */
        std_tim_clear_flag(TIM3, TIM_FLAG_TRIG);
        g_ch1_cc1_value[g_loop_number ++] = std_tim_get_ccx_value(TIM3, TIM_CHANNEL_1);
        
        /* 复位循环变量 */
        if (g_loop_number >= CC1_VALUE_NUM)
        {
            g_loop_number = 0;
        }
        
        /* 用户对捕获值处理... */
        
    }    
}


/**
* @brief  TIM3 霍尔接口配置
* @retval 无
*/
void bsp_tim3_hall_start(void)
{
    /* 配置XOR功能 */
    std_tim_ti1xor_enable(TIM3);
    std_tim_trig_source_config(TIM3, TIM_TRIG_SOURCE_TI1F_ED);

    /* 配置从模式为复位模式 */
    std_tim_slave_mode_config(TIM3, TIM_SLAVE_MODE_RESET);   
    
    /* 使能TIM3触发中断 */
    std_tim_interrupt_enable(TIM3, TIM_INTERRUPT_TRIG);
        
    /* 启动TIM3计数 */
    std_tim_enable(TIM3);

}

/************************************************************************************************/
/**
* @file               tim_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              TIM BSP驱动函数，实现TIM PWM输入信号的频率及占空比计算。
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

/*------------------------------------------variables-------------------------------------------*/
uint32_t g_enter_cnt = 0U;

uint32_t g_ch1_ccx_value = 0U;
uint32_t g_ch2_ccx_value = 0U;

uint32_t g_pwm_duty = 0x0U;
uint32_t g_pwm_frequency = 0x0U;


/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  TIM3中断处理程序
* @retval 无
*/
void TIM3_IRQHandler(void)
{
    /* CC1上升沿捕获 */
    if (std_tim_get_flag(TIM3, TIM_FLAG_CC1))
    {
        /* 从模式控制器配置为复位模式，且TI1FP1信号用作触发输入信号，故首次捕获的CC值忽略 */
        if (g_enter_cnt == 0)
        {
            g_enter_cnt = 1;
            std_tim_get_ccx_value(TIM3, TIM_CHANNEL_1);
        }
        
        /* 获取第二次捕获的CC值 */
        else if (g_enter_cnt == 1)
        {
            /* 关闭CC1中断 */
            std_tim_interrupt_disable(TIM3, TIM_INTERRUPT_CC1);
            g_ch1_ccx_value = std_tim_get_ccx_value(TIM3, TIM_CHANNEL_1);
            
            /* 计算占空比 */
            g_pwm_duty = ((g_ch2_ccx_value + 1) * 100) / (g_ch1_ccx_value + 1);
            
            /* 计算输入频率 */
            g_pwm_frequency = (std_rcc_get_pclkfreq() / (g_ch1_ccx_value + 1));
        }
    }
    
    /* CC2下降沿捕获 */
    if (std_tim_get_flag(TIM3, TIM_FLAG_CC2))
    {
        /* 关闭CC2中断 */
        std_tim_interrupt_disable(TIM3, TIM_INTERRUPT_CC2);
        g_ch2_ccx_value = std_tim_get_ccx_value(TIM3, TIM_CHANNEL_2);       
    }    
}

/**
* @brief  TIM3启动捕获
* @retval 无
*/
void bsp_tim3_capture_start(void)
{
    /* 使能TIM3输入捕获通道1和通道2中断 */
    std_tim_interrupt_enable(TIM3, TIM_INTERRUPT_CC1 | TIM_INTERRUPT_CC2);
       
    /* 启动TIM3计数 */
    std_tim_enable(TIM3);
}

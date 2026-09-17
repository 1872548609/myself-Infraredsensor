/************************************************************************************************/
/**
* @file               tim_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              TIM BSP驱动函数，实现TIM 6步PWM输出功能配置。
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


/*--------------------------------------------variables-----------------------------------------*/
__IO uint32_t g_step = 1;

/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  SysTick中断服务程序，产生一次换相事件
* @retval 无
*/
void SysTick_Handler(void)
{
    /* 软件触发一次换相事件 */
    std_tim_set_sw_trig_event(TIM1, TIM_EVENT_SRC_COM);
}


/**
* @brief  TIM1换相中断服务程序
* @retval 无
*/
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    /* 清除TIM1换相中断标志 */
    std_tim_clear_flag(TIM1, TIM_FLAG_COM);
    
    if (g_step == 1)
    {
        /* 配置Step 2参数 ---------------------------- */
        /* 通道1的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_1, TIM_OUTPUT_MODE_PWM1);
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_2, TIM_OUTPUT_MODE_PWM1);
        
        std_tim_ccx_channel_enable(TIM1, TIM_CHANNEL_1);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_1);        

        /* 通道2的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_2, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_2);
        std_tim_ccxn_channel_enable(TIM1, TIM_CHANNEL_2);  

        /* 通道3的配置 */        
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_3);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_3);
          
        g_step++;
    }
    else if (g_step == 2)
    {
        /* 配置Step 3参数 ---------------------------- */
        /* 通道1的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_1, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_enable(TIM1, TIM_CHANNEL_1);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_1);
        
        /* 通道2的配置 */
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_2);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_2);    
        
        /* 通道3的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_3, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_3);
        std_tim_ccxn_channel_enable(TIM1, TIM_CHANNEL_3);    
        g_step++;
    }
    else if (g_step == 3)
    {
        /* 配置Step 4参数 ---------------------------- */
        /* 通道1的配置 */        
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_1);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_1);  
        
        /* 通道2的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_2, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_enable(TIM1, TIM_CHANNEL_2);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_2);  
        
        /* 通道3的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_3, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_3);
        std_tim_ccxn_channel_enable(TIM1, TIM_CHANNEL_3);    
        g_step++;
    }
    else if (g_step == 4)
    {
        /* 配置Step 5参数 ---------------------------- */
        /* 通道1的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_1, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_1);
        std_tim_ccxn_channel_enable(TIM1, TIM_CHANNEL_1);

        /* 通道2的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_2, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_enable(TIM1, TIM_CHANNEL_2);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_2);    
        
        /* 通道3的配置 */
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_3);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_3);
        g_step++;
    }
    else if (g_step == 5)
    {
        /* 配置Step 6参数 ---------------------------- */
        /* 通道1的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_1, TIM_OUTPUT_MODE_PWM1);        
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_1);
        std_tim_ccxn_channel_enable(TIM1, TIM_CHANNEL_1);

        /* 通道2的配置 */
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_2);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_2);  
        
        /* 通道3的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_3, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_enable(TIM1, TIM_CHANNEL_3);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_3);    
        g_step++;
    }
    else
    {
        /* 配置Step 1参数 ---------------------------- */
        /* 通道1的配置 */
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_1);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_1);

        /* 通道2的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_2, TIM_OUTPUT_MODE_PWM1);        
        std_tim_ccx_channel_disable(TIM1, TIM_CHANNEL_2);
        std_tim_ccxn_channel_enable(TIM1, TIM_CHANNEL_2);
        
        /* 通道3的配置 */
        std_tim_set_ocmode(TIM1, TIM_CHANNEL_3, TIM_OUTPUT_MODE_PWM1);
        std_tim_ccx_channel_enable(TIM1, TIM_CHANNEL_3);
        std_tim_ccxn_channel_disable(TIM1, TIM_CHANNEL_3);    
        g_step = 1;
    }    
}



/**
* @brief  使能TIM1带互补通道输出，并使能换相中断
* @retval 无
*/
void bsp_tim1_commutation_config(void)
{   
    /* 使能换相预装载功能 */
    std_tim_ccreload_enable(TIM1);
    
    /* 使能换相中断 */
    std_tim_interrupt_enable(TIM1, TIM_INTERRUPT_COM);
    
    /* 使能输出 */
    std_tim_moen_enable(TIM1);

    /* 使能计数 */
    std_tim_enable(TIM1);    
}


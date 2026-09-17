/************************************************************************************************/
/**
* @file               lptim_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              LPTIM BSP驱动函数，实现LPTIM计数功能。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "lptim_bsp.h"

/*--------------------------------------------define--------------------------------------------*/
#define PERIODVALUE     (uint32_t) (0x7D00 - 1)       /* 定时周期(ARR) */

/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  LPTIM1中断服务程序
* @retval 无
*/
void LPTIM1_IRQHandler(void)
{
    /* LPTIM1处理重载匹配中断 */
    if(std_lptim_get_flag(LPTIM_FLAG_ARRM) == LPTIM_FLAG_ARRM)
    {
        std_lptim_clear_flag(LPTIM_CLEAR_ARRM);
        
        /* 翻转LED */
        std_gpio_toggle_pin(LED_GPIO_PORT, LED_PIN);
        
    }
}

/**
* @brief  LPTIM1启动计数，并使能自动重载匹配中断
* @retval 无
*/
void bsp_lptim_start(void)
{
    /* 使能自动重载匹配中断 */
    std_lptim_interrupt_enable(LPTIM_INTERRUPT_ARRM);
    
    /* 使能LPTIM1 */
    std_lptim_enable();
    
    /* 启动LPTIM1计数 */
    std_lptim_set_auto_reload(PERIODVALUE);
    std_lptim_start_counter(LPTIM_COUNT_CONTINUOUS);
}




/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示芯片循环进入Deepstop低功耗模式，LPTIM自动重载匹配中断周期性唤醒的功能。
*                     LPTIM的时钟源为RCL，每1s产生一次自动重载匹配中断。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "main.h"

/*-------------------------------------------functions------------------------------------------*/
int main(void)
{    
    /* 配置系统时钟 */
    system_clock_config();
    
    /* Systick初始化 */
    std_delay_init();
    
    /* GPIO初始化 */
    gpio_init();
    
    /* LPTIM1初始化 */
    lptim1_init();
    
    /* NVIC初始化 */
    nvic_init();
    
    /* 等待3S，便于用户调试 */
    std_delayms(3000);
    
    /* LPTIM1启动连续计数，并使能自动重载匹配中断 */
    bsp_lptim_start();
    
    while(1)
    {
        /* 进入Deepstop模式 */
        std_pmu_enter_stop(PMU_MODE_DEEPSTOP, PMU_ENTRY_LOWPOWER_MODE_WFI);
    }
}


/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示Deepstop被唤醒后，快速响应应用程序的功能。
*                     该唤醒函数应在SRAM中执行，一旦被唤醒，无需等待可快速执行相关操作；
*                     本示例低功耗唤醒源为LPTIM计时溢出。
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


/*--------------------------------------------define--------------------------------------------*/
#define DELAY_10US               (STD_DELAY_US * 10) 

/*--------------------------------------------variables-----------------------------------------*/
uint32_t g_flash_delay_time = 0U;


/*-------------------------------------------functions------------------------------------------*/
int main(void)
{        
    /* 配置系统时钟为RCH */
    system_clock_config();   
    
    /* Systick初始化 */
    std_delay_init();
    
    /* 配置LED灯 */    
    gpio_init();     

    /* LPTIM初始化 */
    lptim_init();
    
    /* 等待3S，便于用户调试 */
    std_delayms(3000); 
    
    /* 当从SRAM切换到Flash运行程序时，需要延时10us */
    g_flash_delay_time = DELAY_10US;
    
    /* 配置Flash唤醒等待时间为0us，并进入Deepstop模式 
    注意：
       该函数在SRAM中运行，此时Deepstop唤醒过程中Flash唤醒等待时间为0us 
    */  
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_PMU);
    bsp_pmu_deepstop_flash_wakeup_time(); 
        
    /* 切换到Flash后：
       1、恢复系统时钟配置（RCH）   
       2、点亮LED灯          */
    system_clock_config();           
    std_gpio_reset_pin(LED_GPIO_PORT, LED_PIN);    
        
    while(1)
    {
       
    }
}





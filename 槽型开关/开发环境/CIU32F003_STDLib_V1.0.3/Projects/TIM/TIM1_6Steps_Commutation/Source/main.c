/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示TIM1 6步PWM输出功能，每100ms触发一次换相，并改变其输出状态。
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
    
    /* GPIO初始化 */
    gpio_init();        
    
    /* TIM1初始化 */
    tim1_init();
    
    /* 配置Systick */
    SysTick_Config((SystemCoreClock) / 10);
    
    /* NVIC初始化 */
    nvic_init();
    
    /* 配置TIM1换相功能，及互补通道的PWM模式输出 */
    bsp_tim1_commutation_config();
    
    /* 等待触发换相事件 */
    while(1)
    {
       
    }
}


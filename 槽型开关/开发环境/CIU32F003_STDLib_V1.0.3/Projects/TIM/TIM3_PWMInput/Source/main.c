/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示TIM3 PWM输入捕获功能，并计数输入频率值及占空比信息。
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

/*------------------------------------------functions-------------------------------------------*/
int main(void)
{   
    /* 配置系统时钟 */
    system_clock_config();
    
    /* GPIO初始化 */
    gpio_init();
    
    /* 设置MCO输出RCHDIV6的8分频 */
    /* 此处为了演示TIM3的PWM输入功能，将MCO引脚与TIM3_CH1相连，
       用户也可以根据实际应用将外部频率与TIM3_IN连接            */
    std_rcc_mco_config(RCC_MCO_SRC_RCHDIV6, RCC_MCO_DIV8);;
    
    /* TIM3初始化 */
    tim3_init();
    
    /* NVIC初始化 */
    nvic_init();
    
    /* TIM3启动捕获 */
    bsp_tim3_capture_start();
    
    while(1)
    {

    }  
}



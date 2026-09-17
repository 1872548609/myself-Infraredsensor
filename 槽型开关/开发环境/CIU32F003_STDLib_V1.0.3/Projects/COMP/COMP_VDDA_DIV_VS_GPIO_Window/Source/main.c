/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              本示例展示COMP窗口比较模式的用法。窗口比较器的输入是PB0，窗口阈值上限是VDDA
*                     的10/16,阈值下限由PA4决定。当输入超过阈值时LED亮，反之LED灭。 
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
    uint32_t voltage_state = 0U;

    /* 配置系统时钟为RCH（48MHz） */
    system_clock_config();
    
    /* 延时函数初始化 */
    std_delay_init();
    
    /* GPIO初始化 */
    gpio_init();
 
    /* COMP1和COMP2初始化为窗口比较器 */
    comp_init();
    
    while(1)
    {
        /* 检查输入电压 */
        voltage_state = bsp_comp_input_voltage_level_check();
        if((voltage_state == STATE_OVER_THRESHOLD) || (voltage_state == STATE_UNDER_THRESHOLD))
        {
            /* 超过阈值范围点亮LED */
            std_gpio_reset_pin(LED_GPIO_PORT,LED_PIN);
        }
        else
        {
            /* 在阈值范围内熄灭LED */
            std_gpio_set_pin(LED_GPIO_PORT,LED_PIN);
        }
    }
}


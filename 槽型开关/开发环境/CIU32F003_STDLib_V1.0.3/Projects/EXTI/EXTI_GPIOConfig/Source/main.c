/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示EXTI的用法。 按键下降沿触发EXTI中断， 每次触发LED翻转。
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

/*------------------------------------------variables-------------------------------------------*/
uint32_t g_exti_gpio_flag = 0;

/*-------------------------------------------functions------------------------------------------*/
int main(void)
{      
    /* 配置系统时钟 */
    system_clock_config(); 

    /* Systick初始化 */
    std_delay_init();

    /* GPIO 初始化 */
    gpio_init();

    /* EXTI初始化 */
    exti_init();
    
    /* NVIC 初始化*/
    nvic_init();

    while (1)
    {
        /* 每按一次按键触发EXTI中断，LED切换一次亮灭 */
        if (g_exti_gpio_flag == 1)
        {
            std_gpio_toggle_pin(LED_GPIO_PORT,LED_PIN);
            g_exti_gpio_flag = 0;
        }
    } 
}











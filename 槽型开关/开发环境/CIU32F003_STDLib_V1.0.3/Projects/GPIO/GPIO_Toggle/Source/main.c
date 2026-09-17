/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示GPIO的用法。每300毫秒LED的状态翻转一次。
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

    /* Systick初始化 */
    std_delay_init();

    /* GPIO初始化 */
    gpio_init();

    while(1)
    {
        std_gpio_toggle_pin(LED_GPIO_PORT,LED_PIN);
        std_delayms(300);
    }
}











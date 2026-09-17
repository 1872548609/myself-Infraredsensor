/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示COMP的用法。COMP的正相输入GPIO（PB0），反相输入VBGR（0.8V），如果PB0大于
*                     VBGR，LED亮，反之，LED灭。
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
    /* 配置系统时钟为RCH（48MHz）*/
    system_clock_config();
    
    /* 延时函数初始化 */
    std_delay_init();
    
    /* gpio初始化 */
    gpio_init();

    /* COMP1初始化 */
    comp1_init();
    
    /* 查询方式获取比较器比较结果 */
    while(1)
    {
        /* 正相电压高于反相 */
        if(std_comp_get_output_result(COMP1))
        {
            /* 点亮LED */
            std_gpio_reset_pin(LED_GPIO_PORT,LED_PIN);
        }
        /* 正相电压低于反相 */
        else
        {
            /* 熄灭LED */
            std_gpio_set_pin(LED_GPIO_PORT,LED_PIN);
        }
    }
}


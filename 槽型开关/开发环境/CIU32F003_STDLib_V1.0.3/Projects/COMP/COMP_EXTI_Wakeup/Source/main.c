/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示COMP通过唤醒DEEPSTOP的功能。正相输入PB0默认接GND，当PB0大于反相
*                     输入VBGR（0.8V）时，产生上升沿唤醒DeepStop，LED闪烁。
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
    /* 配置系统时钟为RCH（48MHz）*/
    system_clock_config();

    /* 延时函数初始化 */
    std_delay_init();
    
    /* GPIO初始化 */
    gpio_init();

    /* COMP1初始化 */
    comp1_init();

    /* NVIC初始化 */
    nvic_init(); 

    /* 进入DeepStop前延时3s便于debug，正常使用不用延时 */
    std_delayms(3000);

    /* 进入DeepStop模式 */
    std_pmu_enter_stop(PMU_MODE_DEEPSTOP,PMU_ENTRY_LOWPOWER_MODE_WFI);
      
    /* 唤醒后，恢复系统时钟配置 */
    system_clock_config();      

    while(1)
    {
        /* 唤醒后LED闪烁 */
        std_gpio_toggle_pin(LED_GPIO_PORT,LED_PIN);
        std_delayms(300); 
    }
}



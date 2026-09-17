/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示WFI方式进入stop模式后，通过EXTI（按键）方式唤醒芯片的功能；
*                     芯片唤醒后，系统时钟恢复为RCH时钟：48MHz。
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
    
    /* 配置按键，作为stop唤醒源 */    
    button_gpio_init();    
       
    /* NVIC初始化 */
    nvic_init(); 
    
    /* 等待3S，便于用户调试 */
    std_delayms(3000); 
    
    /* 进入stop模式 */  
    std_pmu_enter_stop(PMU_MODE_STOP, PMU_ENTRY_LOWPOWER_MODE_WFI);
    

    /* 唤醒后，恢复系统时钟配置 */
    system_clock_config();

    led_gpio_init();
    
    /* 点亮LED */
    std_gpio_reset_pin(LED_GPIO_PORT, LED_PIN);
        
    while(1)
    {
       
    }
}





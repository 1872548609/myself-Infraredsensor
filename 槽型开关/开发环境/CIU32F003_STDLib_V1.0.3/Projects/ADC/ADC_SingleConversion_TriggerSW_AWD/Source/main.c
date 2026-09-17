/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              本示例展示ADC模拟看门狗的使用，软件触发ADC单次扫描转换，模拟看门狗监测ADC通道7（PB0）；
*                     间隔1s触发一次ADC转换，将转换结果转为电压值进行保存，当转换结果超出看门狗阈值范围内，
*                     触发看门狗报警中断，翻转一次LED（PB1）。
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
/* ADC参考电压为VDDA：3300mV。在实际应用中需根据真实的VDD电压值设置该值 */
#define VREF_ADC_VDDA_VOLTAGE        (3300U)

/*------------------------------------------variables-------------------------------------------*/
__IO uint32_t g_voltage;

/*------------------------------------------functions-------------------------------------------*/
int main(void)
{
    /* 配置系统时钟（RCH48MHz）*/
    system_clock_config();
    
    /* 延时函数初始化 */
    std_delay_init();
    
    /* GPIO初始化 */
    gpio_init();
            
    /* ADC初始化 */
    adc_init();

    /* NVIC初始化 */
    nvic_init();
    
    /* 提升ADC校准系数精度，系统复位后只需执行一次 */ 
    bsp_adc_software_calibrate();

    while(1)
    {
        /* 启动转换 */
        std_adc_start_conversion();
        
        /* 等待ADC通道转换完成 */
        while(std_adc_get_flag(ADC_FLAG_EOC) == 0);
        
        /* 清除EOC标志 */
        std_adc_clear_flag(ADC_FLAG_EOC);
        
        /* 获取采样值并转换为电压值，单位mV*/
        g_voltage = std_adc_get_conversion_value() * VREF_ADC_VDDA_VOLTAGE / ADC_CONVER_SCALE;
        
        if(g_interrupt_status == 0x01U)
        {
            g_interrupt_status = 0;
            
            /* LED反转 */
            std_gpio_toggle_pin(LED_GPIO_PORT, LED_PIN);
        }
        
        /* 1s转换一次*/
        std_delayms(1000); 
    }
}



/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              本示例展示硬件触发ADC单次扫描转换，仅配置1个通道（PA7），硬件EXTI通道7（PB7）
*                     上升沿触发；查询方式获取ADC转换结果，间隔0.5S等待一次触发。
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
    /* 配置系统时钟 */
    system_clock_config(); 
    
    /* 延时函数初始化 */
    std_delay_init();

    /* GPIO初始化 */
    gpio_init();
    
    /* ADC初始化 */
    adc_init();

    /* 提升ADC校准系数精度，系统复位后只需执行一次 */ 
    bsp_adc_software_calibrate();
    
    /* 启动转换 */
    std_adc_start_conversion();  
    
    while(1)
    {
        /* 等待ADC通道转换完成 */
        while(std_adc_get_flag(ADC_FLAG_EOC) == 0U);

        /* 清除EOC标志 */
        std_adc_clear_flag(ADC_FLAG_EOC);
        
        /* 获取采样值并转换为电压值，单位mV */
        g_voltage = std_adc_get_conversion_value() * VREF_ADC_VDDA_VOLTAGE / ADC_CONVER_SCALE;  
        
        std_delayms(500);
    }
}

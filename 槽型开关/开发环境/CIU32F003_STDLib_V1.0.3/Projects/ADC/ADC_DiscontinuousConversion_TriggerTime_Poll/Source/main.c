/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              本示例展示硬件触发ADC循环间断转换，配置为1个ADC通道，使用TIM3的更新事件触发ADC转换；
*                     TIM3间隔1s触发一次ADC，ADC采样当前通道电压值，将转换结果转为电压值进行保存，TIM3触
*                     发完成ADC所有通道的转换，清除EOS标志。
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*-------------------------------------------includes-------------------------------------------*/
#include "main.h"

/*--------------------------------------------define--------------------------------------------*/
/* ADC参考电压为VDDA：3300mV。在实际应用中需根据真实的VDD电压值设置该值 */
#define VREF_ADC_VDDA_VOLTAGE        (3300U)

/* ADC转换通道数 */
#define ADC_CHANNEL_NUM              (1U)

/*-------------------------------------------variables------------------------------------------*/
__IO uint32_t g_voltage_buf[ADC_CHANNEL_NUM];

/*-------------------------------------------functions------------------------------------------*/
int main(void)
{
    uint32_t i;
    
    /* 配置系统时(RCH 48MHz) */
    system_clock_config(); 
    
    /* 延时函数初始化 */
    std_delay_init();

    /* GPIO初始化 */
    gpio_init();

    /* TIM3初始化 */
    tim3_init();

    /* ADC初始化 */
    adc_init();
    
    /* 提升ADC校准系数精度，系统复位后只需执行一次 */ 
    bsp_adc_software_calibrate();
    
    /* 使能TIM3 */
    std_tim_enable(TIM3);
    
    /* 启动转换 */
    std_adc_start_conversion();

    while(1)
    {
        /* 当前示例仅1个通道，多通道需修改ADC_CHANNEL_NUM定义同时使能对应多通道以及相关配置 */
        for(i=0; i<ADC_CHANNEL_NUM; i++)
        {
            /* TIM3间隔1s触发一次ADC，ADC采样当前通道电压值 */
            while(std_adc_get_flag(ADC_FLAG_EOC) == 0U);

            /* 清除EOC标志 */
            std_adc_clear_flag(ADC_FLAG_EOC);
            
            /* 获取采样值并转换为电压值，单位mV */
            g_voltage_buf[i] = std_adc_get_conversion_value() * VREF_ADC_VDDA_VOLTAGE / ADC_CONVER_SCALE;  
        }
        
        /* 等待一个序列转换完成 */
        while(std_adc_get_flag(ADC_FLAG_EOS) == 0U);
        
        /* 清除EOS标志 */
        std_adc_clear_flag(ADC_FLAG_EOS);
        
    } 
}


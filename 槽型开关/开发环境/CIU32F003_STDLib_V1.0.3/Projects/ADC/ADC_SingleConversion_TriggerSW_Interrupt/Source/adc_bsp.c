/************************************************************************************************/
/**
* @file               adc_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              ADC BSP驱动函数。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*-------------------------------------------includes-------------------------------------------*/
#include "adc_bsp.h"

/*--------------------------------------------define--------------------------------------------*/
/* ADC参考电压为VDDA：3300mV。在实际应用中需根据真实的VDD电压值设置该值 */
#define VREF_ADC_VDDA_VOLTAGE               (3300U)

/* ADC采样次数，根据实际需求可调整此值 */
#define ADC_SAMPLE_NUM                      (10U)

/* 校准系数的最值 */
#define CALFACT_MAX                         (31)
#define CALFACT_MIN                         (-31)

/* ADC校准系数的符号位 */
#define ADC_CALFACT_SYMBOL                  (ADC_CALFACT_CALFACT_5)

/* ADC补偿值 */
#define ADC_COMPENSATION_VALUE              (*(int32_t *)(0x1FFF03CC))  

/*-------------------------------------------variables------------------------------------------*/
__IO uint8_t g_adc_complete = 0U;
__IO uint32_t g_ex_channel_sample = 0U;

/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  ADC中断服务函数
* @retval 无
*/
void ADC_COMP_IRQHandler(void)
{  
    if(std_adc_get_flag(ADC_FLAG_EOC))
    {
        /* 清除EOC中断标志 */                       
        std_adc_clear_flag(ADC_FLAG_EOC); 

        /* 获取外部通道采样值 */ 
        g_ex_channel_sample = std_adc_get_conversion_value();
        
        g_adc_complete = 1U;
    }
}

/**
* @brief  提升ADC校准系数的精度
* @retval 无
*/
void bsp_adc_software_calibrate(void)
{
    int32_t get_calfact = 0;
    
    /* 使能校准 */
    std_adc_calibration_enable();
    
    /* 等待校准完成 */
    while(std_adc_get_flag(ADC_FLAG_EOCAL) == 0U);
    
    /* 清除ADC转换状态，确保之前状态不影响转换 */
    std_adc_clear_flag(ADC_FLAG_ALL);
    
    get_calfact = std_adc_get_calibration_factor();
    
    /* 判断校准系数符号位 */
    if(get_calfact & ADC_CALFACT_SYMBOL)
    {
        /* 校准系数是负值，转换成32位有符号负数，方便计算 */
        get_calfact = get_calfact | 0xFFFFFFE0;
    }
    
    /* 校准系数减去ADC补偿值获取新的校准系数 */
    get_calfact = get_calfact - ADC_COMPENSATION_VALUE;
    
    /* 判断校准系数是否超限 */
    if(get_calfact > CALFACT_MAX)
    {
        get_calfact = CALFACT_MAX;
    }
    else if(get_calfact < CALFACT_MIN)
    {
        get_calfact = CALFACT_MIN;
    }
    std_adc_calibration_factor_config(get_calfact);
}

/**
* @brief  转换ADC通道采样值为实际电压值
* @retval uint32_t 转换结果
*/
uint32_t bsp_adc_get_channel_voltage(void)
{
    uint32_t get_sample_sum = 0;
    uint32_t get_channel_voltage = 0;
    uint32_t get_single_sample = 0;
    
    /* ADC多次采样取平均，提升采样精度 */
    for(uint8_t i = 0; i < ADC_SAMPLE_NUM; i++)
    {
        /* 启动转换 */
        std_adc_start_conversion();
        
        /* 等待ADC通道转换完成 */
        while(g_adc_complete == 0x00U);
         
        g_adc_complete = 0U;
        
        get_sample_sum += g_ex_channel_sample;
    }
    
    /* 计算平均值 */
    get_single_sample = get_sample_sum / ADC_SAMPLE_NUM;
    
    /* 计算采样通道电压 */
    get_channel_voltage = get_single_sample * VREF_ADC_VDDA_VOLTAGE / ADC_CONVER_SCALE;

    return get_channel_voltage;
}

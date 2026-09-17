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

/*------------------------------------------includes--------------------------------------------*/
#include "adc_bsp.h"

/*--------------------------------------------define--------------------------------------------*/
/* ADC采样次数，根据实际需求可调整此值 */
#define ADC_SAMPLE_NUM                       (10U)

/* 校准系数的最值 */
#define CALFACT_MAX                          (31)
#define CALFACT_MIN                          (-31)

/* ADC校准系数的符号位 */
#define ADC_CALFACT_SYMBOL                   (ADC_CALFACT_CALFACT_5)

/* ADC补偿值 */
#define ADC_COMPENSATION_VALUE               (*(int32_t *)(0x1FFF03CC))      

/*------------------------------------------functions-------------------------------------------*/
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
* @brief  获取BGR转换通道采样值
* @param  conversion_result 通道采样值
* @retval 无
*/
void bsp_adc_get_single_channel_sample_value(uint32_t *conversion_result)
{
    /* 启动转换 */
    std_adc_start_conversion();
    
    /* 等待BGR通道转换完成，采样BGR时采样时间需保证大于12us，可根据ADC工作时钟频率（ADC_CK）配置合理的采样周期数 */
    while(std_adc_get_flag(ADC_FLAG_EOC) == 0U);

    /* 清除EOC标志 */
    std_adc_clear_flag(ADC_FLAG_EOC);
        
    /* 获取通道采样值 */
    *conversion_result = std_adc_get_conversion_value();
}

/**
* @brief  ADC采集BGR校准参考电压值
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
        bsp_adc_get_single_channel_sample_value(&get_single_sample);
        get_sample_sum += get_single_sample;
    }
    
    /* 计算平均值 */
    get_single_sample = get_sample_sum / ADC_SAMPLE_NUM;
    
    /* 校准参考电压 */
    get_channel_voltage = std_adc_calc_vref_voltage(get_single_sample);

    return get_channel_voltage;
}


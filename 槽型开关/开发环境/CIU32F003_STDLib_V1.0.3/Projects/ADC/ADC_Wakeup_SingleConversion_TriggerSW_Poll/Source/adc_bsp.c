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
#include "common.h"

/*--------------------------------------------define--------------------------------------------*/
/* ADC参考电压为VDDA：3300mV。在实际应用中需根据真实的VDD电压值设置该值 */
#define VREF_ADC_VDDA_VOLTAGE               (3300U)

/* 校准系数的最值 */
#define CALFACT_MAX                         (31)
#define CALFACT_MIN                         (-31)

/* ADC校准系数的符号位 */
#define ADC_CALFACT_SYMBOL                  (ADC_CALFACT_CALFACT_5)

/* ADC补偿值 */
#define ADC_COMPENSATION_VALUE              (*(int32_t *)(0x1FFF03CC))   

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  GPIO_PIN_2到GPIO_PIN_3中断服务函数
* @retval 无
*/
void EXTI2_3_IRQHandler(void)
{
    /* 检测EXTI Line 中断 */
    if (std_exti_get_pending_status(USER_BUTTON_EXTI_LINE))
    {
        std_exti_clear_pending(USER_BUTTON_EXTI_LINE);
    }
    
    /* 用户根据EXTI的中断事件，结合当前的状态实现中断处理函数 */    
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
* @param  conversion_result 转换的电压值(mV)
* @retval 无
*/
void bsp_adc_get_channel_voltage(uint32_t *conversion_result)
{
    uint32_t get_sample_sum[ADC_CHANNEL_NUM + 1] = {0};
    uint32_t get_cal_voltage = 0;
    uint8_t cur_sample_cnt = 0;
    uint8_t cur_channel_num = 0;
    
    /* ADC多次采样取平均，提升采样精度 */
    for(cur_sample_cnt = 0; cur_sample_cnt < ADC_SAMPLE_NUM; cur_sample_cnt++)
    {
        /* 启动转换 */
        std_adc_start_conversion();
        
        /* 多外部通道需修改ADC_CHANNEL_NUM定义同时使能对应多通道以及相关配置 */
        for(cur_channel_num = 0; cur_channel_num < (ADC_CHANNEL_NUM + 1); cur_channel_num++)
        {
            /* 等待ADC通道转换完成 */
            while(std_adc_get_flag(ADC_FLAG_EOC) == 0U);

            /* 清除EOC标志 */
            std_adc_clear_flag(ADC_FLAG_EOC);
            
            get_sample_sum[cur_channel_num] += std_adc_get_conversion_value();
        }
    
        /* 等待一个序列转换完成 */
        while(std_adc_get_flag(ADC_FLAG_EOS) == 0U);
        
        /* 清除EOS标志 */
        std_adc_clear_flag(ADC_FLAG_EOS);
    }
    
    /* VBGR通道采样平均值 */
    get_sample_sum[ADC_CHANNEL_NUM] = get_sample_sum[ADC_CHANNEL_NUM] / ADC_SAMPLE_NUM;
    
    /* 校准参考电压 */
    get_cal_voltage = std_adc_calc_vref_voltage(get_sample_sum[ADC_CHANNEL_NUM]);
    
    /* 通道采样求平均，再将平均值转换成电压 */
    for(cur_channel_num = 0; cur_channel_num < ADC_CHANNEL_NUM; cur_channel_num++)
    {
        conversion_result[cur_channel_num] = get_sample_sum[cur_channel_num] / ADC_SAMPLE_NUM * get_cal_voltage / ADC_CONVER_SCALE;  
    }
}


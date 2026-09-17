/************************************************************************************************/
/**
* @file               common.c
* @author             MCU Ecosystem Development Team
* @brief              通用函数或本外设相关的配置实现函数。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "common.h"
 
/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  系统时钟配置函数
* @retval 无
*/
void system_clock_config(void)
{
    /* 设置Flash读访问等待时间 */
    std_flash_set_latency(FLASH_LATENCY_1CLK);

    /* 使能RCH */
    std_rcc_rch_enable();
    while(std_rcc_get_rch_ready() != RCC_CSR1_RCHRDY);
    
    /* 设置系统时钟源为RCH */
    std_rcc_set_sysclk_source(RCC_SYSCLK_SRC_RCH);
    while(std_rcc_get_sysclk_source() != RCC_SYSCLK_SRC_STATUS_RCH);

    /* 设置AHB分频因子 */
    std_rcc_set_ahbdiv(RCC_HCLK_DIV1);
    /* 设置APB分频因子 */
    std_rcc_set_apbdiv(RCC_PCLK_DIV1);
    
    /* 配置系统时钟全局变量 */
    SystemCoreClock = RCH_VALUE;
}

/**
* @brief  ADC初始化配置函数
* @retval 无
*/
void adc_init(void)
{
    /* 使能ADC时钟 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_ADC);
       
    /* ADC_CK时钟为PCLK的3分频 */
    std_adc_clock_config(ADC_CK_DIV3);
    
    /* 软件触发ADC */
    std_adc_trig_sw();
    
    /* 单次转换模式 */
    std_adc_conversion_mode_config(ADC_SINGLE_CONVER_MODE);
    
    /* 采样时间配置，239个周期
       采样BGR时采样时间需保证大于12us，可根据ADC工作时钟频率（ADC_CK）配置合理的采样周期数
    */
    std_adc_sampt_time_config(ADC_SAMPTIME_239CYCLES);
     
    /* 选择内部通道VBGR */
    std_adc_fix_sequence_channel_enable(ADC_CHANNEL_VBGR);
    
    /* 配置wait模式，避免数据未及时读取，转换溢出 */
    std_adc_wait_mode_enable();
    
    /* 使能通道VBGR */
    std_adc_internal_channel_vbgr_enable();
    
    /* 等待内部BGR通道启动稳定，ADC_VBGR_CHANNEL_DELAY值可参考CIU32F003的数据手册(tADC_BUF参数) */
    std_delayus(ADC_VBGR_CHANNEL_DELAY);

    /* 使能ADC */
    std_adc_enable();
    
    /* 等待ADC使能状态稳定 */
    std_delayus(ADC_EN_DELAY);
}

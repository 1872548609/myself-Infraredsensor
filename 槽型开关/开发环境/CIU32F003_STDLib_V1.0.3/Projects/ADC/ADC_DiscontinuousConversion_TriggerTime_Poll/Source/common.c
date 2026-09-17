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

/*-------------------------------------------define---------------------------------------------*/
#define TIM_ARR_VALUE             (11718)   /* 计数时钟源为11.7K时，计时1s */  
#define TIM_PSC_VALUE             (12)      /* 系统时钟是48M，分频后时钟为11.7K */ 

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
    
    /* TIM3 TRGO触发，上升沿有效 */
    std_adc_trig_config(ADC_TRIG_HW_EDGE_RISING,ADC_EXTRIG_TIM3_TRGO);
    
    /* 循环间断转换模式 */
    std_adc_conversion_mode_config(ADC_DISCONTINUOUS_CONVER_MODE);
    
    /* 采样时间配置，119个周期 */
    std_adc_sampt_time_config(ADC_SAMPTIME_119CYCLES);
    
    /* 选择通道4 */
    std_adc_fix_sequence_channel_enable(ADC_CHANNEL_4);
       
    /* 配置wait模式，避免数据未及时读取，转换溢出 */
    std_adc_wait_mode_enable();

    /* 使能ADC */
    std_adc_enable();

    /* 等待ADC使能状态稳定 */
    std_delayus(ADC_EN_DELAY);
}


/**
* @brief  tim3初始化
* @retval 无
*/
void tim3_init(void)
{
    std_tim_basic_init_t basic_init_struct = {0};
    
    /* TIM3时钟使能 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_TIM3);
            
    /* 配置TIM3计数器参数 */
    basic_init_struct.prescaler = TIM_PSC_VALUE;
    basic_init_struct.period = TIM_ARR_VALUE;
    basic_init_struct.clock_div = TIM_CLOCK_DTS_DIV1;
    std_tim_init(TIM3, &basic_init_struct);

    /* 清除更新事件标志 */
    std_tim_clear_flag(TIM3, TIM_FLAG_UPDATE);
    
    /* 配置tim3的TRGO信号源 */
    std_tim_trigout_mode_config(TIM3, TIM_TRIG_OUT_UPDATE);
}


/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{     
    std_gpio_init_t adc_gpio_config = {0};

    /* 使能GPIOA时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);

    /* ADC_IN4输入通道配置：PA7 */
    adc_gpio_config.pin = GPIO_PIN_7;
    adc_gpio_config.mode = GPIO_MODE_ANALOG;
    adc_gpio_config.pull = GPIO_NOPULL;
    std_gpio_init(GPIOA, &adc_gpio_config);
}



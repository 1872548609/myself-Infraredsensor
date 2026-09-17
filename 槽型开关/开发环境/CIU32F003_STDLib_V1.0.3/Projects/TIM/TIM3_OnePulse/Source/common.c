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


/*--------------------------------------------define--------------------------------------------*/
#define TIM_ARR_VALUE           (0xFFFFU)
#define TIM_PSC_VALUE           (0x03U)
#define TIM_PULSE_VALUE         (0x3FFFU)


/*-------------------------------------------functions------------------------------------------*/
/**
* @brief  系统时钟配置
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
* @brief  TIM3初始化
* @retval 无
*/
void tim3_init(void)
{
    std_tim_basic_init_t basic_init = {0};    
    std_tim_output_compare_init_t output_init = {0};
    std_tim_input_capture_init_t input_capture_init = {0};    
    
    /* TIM3时钟使能 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_TIM3);
        
    /* 配置TIM3计数器参数 */
    basic_init.prescaler = TIM_PSC_VALUE;
    basic_init.period = TIM_ARR_VALUE;
    std_tim_init(TIM3, &basic_init);
    
    /* 清除更新事件标志 */
    std_tim_clear_flag(TIM3, TIM_FLAG_UPDATE);
    
    /* 配置单脉冲触发模式为TRIG，且触发源为TI2 */
    std_tim_slave_mode_config(TIM3, TIM_SLAVE_MODE_TRIG);
    std_tim_trig_source_config(TIM3, TIM_TRIG_SOURCE_TI2FP2);
        
    /* 配置TI2FP2映射到IC2上，且上升沿有效 */
    input_capture_init.input_capture_pol = TIM_INPUT_POL_RISING;
    input_capture_init.input_capture_sel = TIM_INPUT_CAPTURE_SEL_DIRECTTI;
    input_capture_init.input_capture_prescaler = TIM_INPUT_CAPTURE_PSC_DIV1;
    input_capture_init.input_capture_filter = 0x00;
    std_tim_input_capture_init(TIM3, &input_capture_init, TIM_CHANNEL_2);
    
    /* 配置通道1输出模式为PWM2模式 */
    output_init.output_compare_mode = TIM_OUTPUT_MODE_PWM2;
    output_init.pulse = TIM_PULSE_VALUE;
    output_init.output_pol = TIM_OUTPUT_POL_HIGH; 
    output_init.output_state = TIM_OUTPUT_ENABLE;
    std_tim_output_compare_init(TIM3, &output_init, TIM_CHANNEL_1);
}

/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};
    
    /* GPIOA时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);
    
    /* TIM3 GPIO 配置
    PA4     ------> TIM3_CH2
    PA5     ------> TIM3_CH1  */
    gpio_config.pin = GPIO_PIN_4 | GPIO_PIN_5;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_PULLDOWN;
    gpio_config.alternate = GPIO_AF3_TIM3;
    std_gpio_init(GPIOA, &gpio_config);
}


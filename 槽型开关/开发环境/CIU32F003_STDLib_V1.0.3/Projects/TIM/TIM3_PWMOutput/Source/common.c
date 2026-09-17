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
#define TIM_PERIOD_VALUE        (0xFFFFU)
#define TIM_PRESCALER_VALUE     (0x03U)
#define TIM_PULSE1_VALUE        (uint32_t)(TIM_PERIOD_VALUE/2)
#define TIM_PULSE2_VALUE        (uint32_t)(TIM_PERIOD_VALUE*37.5/100)

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
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t tim3_gpio_init = {0};
    
    /* GPIO时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);
    
    /* TIM3 GPIO 配置
    PA5     ------> TIM3_CH1
    PA4     ------> TIM3_CH2  */
    tim3_gpio_init.pin = GPIO_PIN_4 | GPIO_PIN_5;
    tim3_gpio_init.mode = GPIO_MODE_ALTERNATE;
    tim3_gpio_init.pull = GPIO_NOPULL;
    tim3_gpio_init.alternate = GPIO_AF3_TIM3;
    std_gpio_init(GPIOA, &tim3_gpio_init);    
}


/**
* @brief  TIM3初始化
* @retval 无
*/
void tim3_init(void)
{
    std_tim_basic_init_t basic_init_struct = {0};
    std_tim_output_compare_init_t oc_config_struct = {0};
    
    /* TIM3时钟使能 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_TIM3);
    
    /* 配置TIM3计数器参数 */
    basic_init_struct.prescaler = TIM_PRESCALER_VALUE;
    basic_init_struct.period = TIM_PERIOD_VALUE;
    basic_init_struct.clock_div = TIM_CLOCK_DTS_DIV1;
    std_tim_init(TIM3, &basic_init_struct);
        
    /* 配置通道1输出模式为PWM1模式 */
    oc_config_struct.output_compare_mode = TIM_OUTPUT_MODE_PWM1;
    oc_config_struct.output_pol = TIM_OUTPUT_POL_HIGH;
    oc_config_struct.pulse = TIM_PULSE1_VALUE;
    oc_config_struct.output_state = TIM_OUTPUT_ENABLE;
    std_tim_output_compare_init(TIM3, &oc_config_struct, TIM_CHANNEL_1);
    
    /* 配置通道2输出模式为PWM1模式 */
    oc_config_struct.pulse = TIM_PULSE2_VALUE;
    std_tim_output_compare_init(TIM3, &oc_config_struct, TIM_CHANNEL_2);
}


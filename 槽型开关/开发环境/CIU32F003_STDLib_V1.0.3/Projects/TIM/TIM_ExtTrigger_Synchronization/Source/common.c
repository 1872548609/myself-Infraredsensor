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
#define TIM_ARR_VALUE                 (0x3FFFU)
#define TIM_PULSE_VALUE               (TIM_ARR_VALUE >> 1)

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
    std_tim_output_compare_init_t oc_config = {0};
    std_tim_basic_init_t base_config = {0};
    
    /* TIM3时钟使能 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_TIM3);
            
    /* 配置TIM3计数参数 */
    base_config.period = TIM_ARR_VALUE;
    std_tim_init(TIM3, &base_config);
        
    /* 配置通道2为输入模式 */
    std_tim_set_icmode(TIM3, TIM_CHANNEL_2, TIM_INPUT_CAPTURE_SEL_DIRECTTI);
    
    /* 配置从模式为门控模式，且TI2FP2为触发源，上升沿有效 */
    std_tim_slave_mode_config(TIM3, TIM_SLAVE_MODE_GATED);
    std_tim_trig_source_config(TIM3, TIM_TRIG_SOURCE_TI2FP2);
    std_tim_set_input_pol(TIM3, TIM_CHANNEL_2, TIM_INPUT_POL_RISING);

    /* 配置主模式有效 */
    std_tim_trigout_mode_config(TIM3,TIM_TRIG_OUT_ENABLE);
    std_tim_master_mode_enable(TIM3);
    
    /* 配置通道1为比较输出模式 */
    oc_config.output_compare_mode = TIM_OUTPUT_MODE_PWM1;
    oc_config.pulse = TIM_PULSE_VALUE;
    oc_config.output_pol = TIM_OUTPUT_POL_HIGH;
    oc_config.output_state = TIM_OUTPUT_ENABLE;
    std_tim_output_compare_init(TIM3, &oc_config, TIM_CHANNEL_1);
}

/**
* @brief  TIM1初始化
* @retval 无
*/
void tim1_init(void)
{
    std_tim_output_compare_init_t oc_config = {0};
    std_tim_basic_init_t base_config = {0};
        
    /* TIM1时钟使能 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_TIM1);

    /* 配置TIM1计数器参数 */
    base_config.counter_mode = TIM_COUNTER_MODE_UP;
    base_config.period = TIM_ARR_VALUE;
    base_config.clock_div = TIM_CLOCK_DTS_DIV1;
    std_tim_init(TIM1, &base_config);
        
    /* 配置从模式为门控模式，且ITR0为触发源 */
    std_tim_slave_mode_config(TIM1, TIM_SLAVE_MODE_GATED);
    std_tim_trig_source_config(TIM1, TIM_TRIG_SOURCE_ITR0);
    
    /* 配置比较输出模式 */
    oc_config.output_compare_mode = TIM_OUTPUT_MODE_PWM1;
    oc_config.pulse = TIM_PULSE_VALUE;
    oc_config.output_pol = TIM_OUTPUT_POL_HIGH;
    oc_config.output_state = TIM_OUTPUT_ENABLE;    
    std_tim_output_compare_init(TIM1, &oc_config, TIM_CHANNEL_1);         
}


/**
* @brief  TIM1、TIM3 GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};

    /* GPIO外设时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);   
    
    /* TIM1 GPIO 配置   
    PA0     ------> TIM1_CH1  */  
    gpio_config.pin = GPIO_PIN_0;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.alternate = GPIO_AF2_TIM1;
    std_gpio_init(GPIOA, &gpio_config); 

    /* TIM3 GPIO 配置      
    PA5     ------> TIM3_CH1   */ 
    gpio_config.pin = GPIO_PIN_5;
    gpio_config.alternate = GPIO_AF3_TIM3;
    std_gpio_init(GPIOA, &gpio_config);    
    
    /* TIM3 GPIO 配置   
    PA4     ------> TIM3_CH2  */
    gpio_config.pin = GPIO_PIN_4;
    gpio_config.pull = GPIO_PULLDOWN;
    std_gpio_init(GPIOA, &gpio_config);
}



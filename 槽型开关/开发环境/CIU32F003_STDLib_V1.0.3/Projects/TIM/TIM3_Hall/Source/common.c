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
#define TIM_ARR_VALUE        (0xFFFFU)


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
    
    /* GPIOA时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);
    
    /* TIM3 GPIO 配置
    PA5     ------> TIM3_CH1 
    PA4     ------> TIM3_CH2 
    PA3     ------> TIM3_CH3    */
    tim3_gpio_init.pin = GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3;
    tim3_gpio_init.mode = GPIO_MODE_ALTERNATE;
    tim3_gpio_init.pull = GPIO_PULLDOWN;
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
    std_tim_input_capture_init_t input_capture_struct = {0};
    
    /* TIM3时钟使能 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_TIM3);
    
    /* TIM3基本定时器配置 */
    basic_init_struct.period = TIM_ARR_VALUE;
    std_tim_init(TIM3, &basic_init_struct);
    
    /* 配置TRC映射到IC1上，且双沿有效 */
    input_capture_struct.input_capture_pol = TIM_INPUT_POL_BOTH;
    input_capture_struct.input_capture_sel = TIM_INPUT_CAPTURE_SEL_TRC;
    input_capture_struct.input_capture_prescaler = TIM_INPUT_CAPTURE_PSC_DIV1;
    std_tim_input_capture_init(TIM3, &input_capture_struct, TIM_CHANNEL_1);
    
    /* 使能输入捕获 */
    std_tim_ccx_channel_enable(TIM3, TIM_CHANNEL_1);        
}


/**
* @brief  NVIC初始化
* @retval 无
*/
void nvic_init(void)
{
    /* NVIC初始化 */
    NVIC_SetPriority(TIM3_IRQn, NVIC_PRIO_0);
    NVIC_EnableIRQ(TIM3_IRQn);
}



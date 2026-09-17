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
    std_gpio_init_t gpio_config = {0};
    
    /* GPIO时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);
    
    /* TIM3 GPIO 配置
    PA5     ------> TIM3_CH1 */
    gpio_config.pin = GPIO_PIN_5;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_PULLDOWN;
    gpio_config.alternate = GPIO_AF3_TIM3;
    std_gpio_init(GPIOA, &gpio_config); 
    
    /* 配置PA7作为MCO输出引脚 */      
    gpio_config.pin = GPIO_PIN_7;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.alternate = GPIO_AF6_MCO;    
    std_gpio_init(GPIOA, &gpio_config);
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
    basic_init_struct.clock_div = TIM_CLOCK_DTS_DIV1;
    std_tim_init(TIM3, &basic_init_struct);
    
    /* 配置为复位模式，且触发源为TI1FP1，触发极性为上升沿触发 */
    std_tim_slave_mode_config(TIM3, TIM_SLAVE_MODE_RESET);
    std_tim_trig_source_config(TIM3, TIM_TRIG_SOURCE_TI1FP1);
    std_tim_set_input_pol(TIM3, TIM_CHANNEL_1, TIM_INPUT_POL_RISING);
    
    /* 配置TI1FP1映射到IC1上，且上升沿有效 */
    input_capture_struct.input_capture_pol = TIM_INPUT_POL_RISING;
    input_capture_struct.input_capture_sel = TIM_INPUT_CAPTURE_SEL_DIRECTTI;
    input_capture_struct.input_capture_prescaler = TIM_INPUT_CAPTURE_PSC_DIV1;
    input_capture_struct.input_capture_filter = 0x00;
    std_tim_input_capture_init(TIM3, &input_capture_struct, TIM_CHANNEL_1);
    
    /* 配置TI1FP1映射到IC2上，且下降沿有效 */
    input_capture_struct.input_capture_pol = TIM_INPUT_POL_FALLING;
    input_capture_struct.input_capture_sel = TIM_INPUT_CAPTURE_SEL_INDIRECTTI;
    std_tim_input_capture_init(TIM3, &input_capture_struct, TIM_CHANNEL_2);
    
    /* 使能输入捕获 */
    std_tim_ccx_channel_enable(TIM3, TIM_CHANNEL_1);
    std_tim_ccx_channel_enable(TIM3, TIM_CHANNEL_2);    
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





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
#define  TIM_PERIOD_VALUE       0xFFF
#define  TIM_PULSE1_VALUE       (uint32_t)(TIM_PERIOD_VALUE/2)        /* Capture Compare 1 Value  */
#define  TIM_PULSE2_VALUE       (uint32_t)(TIM_PERIOD_VALUE*37.5/100) /* Capture Compare 2 Value  */
#define  TIM_PULSE3_VALUE       (uint32_t)(TIM_PERIOD_VALUE/4)        /* Capture Compare 3 Value  */


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
* @brief  TIM1初始化
* @retval 无
*/
void tim1_init(void)
{
    std_tim_basic_init_t basic_init = {0};    
    std_tim_output_compare_init_t output_init = {0};
    std_tim_break_init_t break_config = {0};
    
    /* TIM1时钟使能 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_TIM1);
        
    /* 配置TIM1计数器参数 */
    basic_init.counter_mode = TIM_COUNTER_MODE_UP;
    basic_init.period = TIM_PERIOD_VALUE;
    basic_init.clock_div = TIM_CLOCK_DTS_DIV1;
    std_tim_init(TIM1, &basic_init);
       
    /* 配置通道1输出模式为PWM1模式 */
    output_init.output_compare_mode = TIM_OUTPUT_MODE_FROZEN;
    output_init.pulse = TIM_PULSE1_VALUE;
    output_init.output_pol = TIM_OUTPUT_POL_HIGH; 
    output_init.output_negtive_pol = TIM_OUTPUT_NEGTIVE_POL_HIGH; 
    output_init.output_idle_state = TIM_OUTPUT_IDLE_SET; 
    output_init.output_negtive_idle_state = TIM_OUTPUT_NEGTIVE_IDLE_RESET;
    output_init.output_state = TIM_OUTPUT_ENABLE;
    output_init.output_negtive_state = TIM_OUTPUT_NEGTIVE_ENABLE;
    std_tim_output_compare_init(TIM1, &output_init, TIM_CHANNEL_1);
   
    /* 配置通道2输出模式为PWM1模式 */    
    output_init.pulse = TIM_PULSE2_VALUE;
    std_tim_output_compare_init(TIM1, &output_init, TIM_CHANNEL_2);    

    /* 配置通道3输出模式为PWM1模式 */    
    output_init.pulse = TIM_PULSE3_VALUE;
    std_tim_output_compare_init(TIM1, &output_init, TIM_CHANNEL_3);    
    
    /* 配置断路输入源为GPIO */
    std_tim_brk_source_enable(TIM1, TIM_BREAK_INPUT_SRC_GPIO);
    std_tim_set_brk_pol(TIM1, TIM_BREAK_INPUT_SRC_GPIO, TIM_BREAK_INPUT_POL_HIGH);
    
    /* 配置断路输入参数 */
    break_config.off_state_run_mode = TIM_OSSR_ENABLE;
    break_config.off_state_idle_mode = TIM_OSSI_ENABLE;
    break_config.lock_level = TIM_LOCK_LEVEL_OFF;
    break_config.dead_time = 0x3F;
    break_config.break_state = TIM_BREAK_ENABLE;    
    std_tim_bdt_init(TIM1, &break_config);    
}

/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};
    
    /* GPIOA、GPIOB时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA | RCC_PERIPH_CLK_GPIOB);
        
    /* TIM1 GPIO 配置
    PB4     ------> TIM1_BK
    PA0     ------> TIM1_CH1
    PB0     ------> TIM1_CH2
    PA1     ------> TIM1_CH3

    PB3     ------> TIM1_CH1N
    PA4     ------> TIM1_CH2N
    PB5     ------> TIM1_CH3N  */
    gpio_config.pin = GPIO_PIN_0 | GPIO_PIN_4;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.alternate = GPIO_AF2_TIM1;
    std_gpio_init(GPIOA, &gpio_config);
    
    gpio_config.pin = GPIO_PIN_1;
    gpio_config.alternate = GPIO_AF4_TIM1;
    std_gpio_init(GPIOA, &gpio_config);   

    gpio_config.pin = GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_5;
    gpio_config.alternate = GPIO_AF2_TIM1;
    std_gpio_init(GPIOB, &gpio_config);   
    
    gpio_config.pin = GPIO_PIN_4;
    gpio_config.pull = GPIO_PULLDOWN;
    std_gpio_init(GPIOB, &gpio_config);       
}


/**
* @brief  NVIC初始化，使能TIM1、SysTick中断
* @retval 无
*/
void nvic_init(void)
{
    /* 配置SysTick中断优先级 */
    NVIC_SetPriority(SysTick_IRQn, NVIC_PRIO_0);
    
    /* 配置TIM1中断使能 */
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, NVIC_PRIO_1);
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
}


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
#define TIM_PERIOD_VALUE        (0x04EFU)
#define TIM_PRESCALER_VALUE     (0x00U)
#define TIM_REPEAT_COUNT        (0x00U)
#define TIM_PULSE1_VALUE        (TIM_PERIOD_VALUE >> 1)

/*------------------------------------------functions-------------------------------------------*/
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
    std_gpio_init_t gpio_config = {0} ;

    /* 使IR_OUT对应的GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);

    /* IRTIM  IR_OUT 输出配置 PA7 */
    gpio_config.pin = GPIO_PIN_7;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    gpio_config.alternate = GPIO_AF7_IR_OUT;
    std_gpio_init(GPIOA, &gpio_config);
    
    /* UART1 配置RX = PA4 */
    gpio_config.pin = GPIO_PIN_4;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.alternate = GPIO_AF1_UART1;   
    std_gpio_init(GPIOA,&gpio_config);     
}

/**
* @brief  TIM1初始化
* @retval 无
*/
void tim1_init(void)
{
    std_tim_basic_init_t basic_init_config = {0};
    std_tim_output_compare_init_t oc_config = {0};
    
    /* TIM1时钟使能 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_TIM1);
    
    /* 配置TIM1计数器参数 */
    basic_init_config.prescaler = TIM_PRESCALER_VALUE;
    basic_init_config.counter_mode = TIM_COUNTER_MODE_UP;
    basic_init_config.period = TIM_PERIOD_VALUE;
    basic_init_config.clock_div = TIM_CLOCK_DTS_DIV1;
    basic_init_config.repeat_counter = TIM_REPEAT_COUNT;
    std_tim_init(TIM1, &basic_init_config);
    
    /* 配置通道1输出模式为PWM1模式 */
    oc_config.output_compare_mode = TIM_OUTPUT_MODE_PWM1;
    oc_config.output_pol = TIM_OUTPUT_POL_HIGH;
    oc_config.pulse = TIM_PULSE1_VALUE;
    oc_config.output_state = TIM_OUTPUT_ENABLE;
    
    std_tim_output_compare_init(TIM1, &oc_config, TIM_CHANNEL_1);
}


/**
* @brief  UART1初始化
* @retval 无
*/
void uart1_init(void)
{
    std_uart_init_t uart_config = {0} ;    
    
    /* UART1时钟使能 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_UART1);
    
    /* UART1 初始化*/
    uart_config.direction = UART_DIRECTION_SEND_RECEIVE;
    uart_config.baudrate = 2400;
    uart_config.wordlength = UART_WORDLENGTH_8BITS;
    uart_config.stopbits = UART_STOPBITS_1;
    uart_config.parity = UART_PARITY_NONE;
    std_uart_init(UART1, &uart_config);
    
    /* UART1使能 */
    std_uart_enable(UART1);
}


/**
* @brief  IRTIM初始化
* @retval 无
*/
void irtim_init(void)
{    
    /* 设置载波信号在调制信号高电平输出，低电平不输出 */
    std_irtim_set_polarity(IRTIM_POLARITY_INVERSE);

    /* 选择载波信号输入源为UART1 */
    std_irtim_set_signal_source(ITRIM_SIGNAL_SOURCE_UART1_TX);  
}



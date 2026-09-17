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

    SystemCoreClock = RCH_VALUE;
}

/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0} ;

    /* 使能LED对应的GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);

    /* 配置LED的IO */
    gpio_config.pin = LED_PIN;
    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    
    /* 初始化 LED GPIO */
    std_gpio_init(LED_GPIO_PORT, &gpio_config);
   
    /* 配置USER BUTTON 的IO*/
    gpio_config.pin = USER_BUTTON_PIN;
    gpio_config.mode = GPIO_MODE_INPUT;
    gpio_config.pull = GPIO_NOPULL;
    
    /* 初始化 USER Button 的GPIO */
    std_gpio_init(USER_BUTTON_GPIO_PORT, &gpio_config);
    
}

/**
* @brief  EXTI初始化
* @retval 无
*/
void exti_init(void)
{
    std_exti_init_t exti_config = {0} ;
      
    /* 配置USER Button的EXTI */
    exti_config.line_id = USER_BUTTON_EXTI_LINE;
    exti_config.mode = EXTI_MODE_INTERRUPT;
    exti_config.trigger = EXTI_TRIGGER_FALLING;
    exti_config.gpio_id = USER_BUTTON_EXTI_PORT;
    /* 初始化EXTI*/
    std_exti_init(&exti_config);

}

/**
* @brief  NVIC初始化
* @retval 无
*/
void nvic_init(void)
{
    /* 配置中断优先级 */
    NVIC_SetPriority(EXTI2_3_IRQn, NVIC_PRIO_3); 
    /* 使能中断 */
    NVIC_EnableIRQ(EXTI2_3_IRQn);
}



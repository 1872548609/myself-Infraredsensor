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
    
    /* 配置系统时钟全局变量 */
    SystemCoreClock = RCH_VALUE;
}

/**
* @brief  按键GPIO初始化
* @retval 无
*/
void button_gpio_init(void)
{
    std_gpio_init_t  gpio_config = {0};

    /* GPIO时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);
    
    /* USER BUTTON初始化 */
    gpio_config.pin = USER_BUTTON_PIN;
    gpio_config.mode = GPIO_MODE_INPUT;
    gpio_config.pull = GPIO_NOPULL;
    std_gpio_init(USER_BUTTON_GPIO_PORT, &gpio_config);    
    
    /* 配置EXTI，中断方式 */
    std_exti_set_gpio(USER_BUTTON_EXTI_PORT, USER_BUTTON_EXTI_LINE);    
    std_exti_falling_trigger_enable(USER_BUTTON_EXTI_LINE);  
    std_exti_interrupt_enable(USER_BUTTON_EXTI_LINE);  
}

/**
* @brief  NVIC初始化
* @retval 无
*/
void nvic_init(void)
{
    /* 使能EXTI中断 */
    NVIC_SetPriority(EXTI2_3_IRQn, NVIC_PRIO_0);
    NVIC_EnableIRQ(EXTI2_3_IRQn);     
}

/**
* @brief  LED灯GPIO初始化
* @retval 无
*/
void led_gpio_init(void)
{
    std_gpio_init_t  gpio_config = {0};

    /* GPIO时钟使能 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);
        
    /* 初始化LED GPIO */
    gpio_config.pin = LED_PIN;
    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    std_gpio_init(LED_GPIO_PORT, &gpio_config); 
    
    /* 初始化关闭LED */
    std_gpio_set_pin(LED_GPIO_PORT, LED_PIN);  
}


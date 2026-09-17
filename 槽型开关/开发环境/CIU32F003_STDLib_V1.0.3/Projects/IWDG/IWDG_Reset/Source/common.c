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
#define IWDG_COUNTER        (0x0200)

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
* @brief  IWDG初始化
* @retval 无
*/
void iwdg_init(void)
{
    /* 使能RCL时钟 */
    std_rcc_rcl_enable();
    while(std_rcc_get_rcl_ready() != RCC_CSR2_RCLRDY);
    
    /* 使能IWDG的写权限 */
    std_iwdg_write_access_enable();

    /* 配置IWDG参数 */
    std_iwdg_set_overflow_period(IWDG_OVERFLOW_PERIOD_2048);
    
    /* 写入重载值,使配置参数生效 */
    std_iwdg_refresh();

    /* IWDG使能 */
    std_iwdg_start();
}

/**
* @brief  LED初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t led_gpio_init = {0};
    
    /* 使能LED对应的GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);

    /* 配置LED的GPIO */
    led_gpio_init.pin   = LED_PIN;
    led_gpio_init.mode  = GPIO_MODE_OUTPUT;
    led_gpio_init.pull  = GPIO_PULLUP;
    led_gpio_init.output_type = GPIO_OUTPUT_PUSHPULL;
    std_gpio_init(LED_GPIO_PORT, &led_gpio_init);
}

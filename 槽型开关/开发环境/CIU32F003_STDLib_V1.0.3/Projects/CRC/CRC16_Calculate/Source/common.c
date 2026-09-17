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
* @brief  CRC初始化
* @retval 无
*/
void crc_init(void)
{
    /* 使能CRC时钟 */
    std_rcc_ahb_clk_enable(RCC_PERIPH_CLK_CRC);

    /* 选择多项式CRC-16 */
    std_crc_set_poly_size(CRC_POLY_16);
    /* 配置默认初始值 */
    std_crc_set_init_value(CRC_DEFAULT_INIT_VALUE);

}


/**
* @brief  LED初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t led_io = {0};
    
    /* 使能LED对应的GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);

    /* 配置LED的IO */
    led_io.pin   = LED_PIN;
    led_io.mode  = GPIO_MODE_OUTPUT;
    led_io.pull  = GPIO_PULLUP;
    led_io.output_type = GPIO_OUTPUT_PUSHPULL;
    std_gpio_init(LED_GPIO_PORT, &led_io);
    
    /* 初始化熄灭LED */
    std_gpio_set_pin(LED_GPIO_PORT, LED_PIN);
}

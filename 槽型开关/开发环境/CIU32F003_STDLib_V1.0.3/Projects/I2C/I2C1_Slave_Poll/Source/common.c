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
#define SLAVE_ADDRESS           (0x5C)

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
    std_gpio_init_t gpio_config = {0};
    
    /* 使能I2C LED GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);
    
    /* 初始化LED GPIO */
    gpio_config.pin = LED_PIN;
    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    std_gpio_init(LED_GPIO_PORT, &gpio_config);   
    
    /* 初始化关闭LED */
    std_gpio_set_pin(LED_GPIO_PORT, LED_PIN);
    
    /* 
    I2C GPIO初始化
    I2C_SCL: PB3
    I2C_SDA: PB4
    */
    gpio_config.pin = GPIO_PIN_3 | GPIO_PIN_4;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.output_type = GPIO_OUTPUT_OPENDRAIN;
    gpio_config.alternate = GPIO_AF6_I2C1;
    std_gpio_init(GPIOB, &gpio_config);
}

/**
* @brief  I2C1初始化为从机模式
* @retval 无
*/
void i2c1_slave_init(void)
{
    /* 使能I2C1外设时钟 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_I2C1);

    /* 禁止I2C1 */
    std_i2c_disable();
    
    /* 
    I2C从模式初始化:
    配置I2C设备地址
    配置I2C时钟延长
    配置I2C数字滤波器
    */
    std_i2c_device_address1_config(SLAVE_ADDRESS);
    std_i2c_clock_stretch_enable();
    std_i2c_digital_filter_config(I2C_DIGITALFILTER_1CLK);
    
    /* 使能I2C1 */
    std_i2c_enable();
}

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
    std_gpio_init_t gpio_config = {0};
    
    /* 使能I2C GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);
    
    /* 设置SCL SDA默认输出高电平 */
    std_gpio_set_pin(I2C_PIN_PORT, (SCL_PIN | SDA_PIN));
    
    /* 初始化I2C GPIO */
    gpio_config.pin = (SCL_PIN | SDA_PIN);
    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.output_type = GPIO_OUTPUT_OPENDRAIN;
    gpio_config.pull = GPIO_NOPULL;
    std_gpio_init(I2C_PIN_PORT, &gpio_config);
}


/**
* @brief  异常处理
* @retval 无
*/
void error_process(void)
{
    /* 用户添加异常处理流程 */
    
    while(1)
    {
    }
}

/**
* @brief  I2C延时函数
* @param  delay_time 延时计数值
* @note   用户可更改延时计数值来调整I2C的通信速率
* @retval 无
*/
void i2c_delay_time(uint32_t delay_time)
{
    uint32_t delay_cnt;
    
    for(delay_cnt=delay_time; delay_cnt>0; delay_cnt--);
}

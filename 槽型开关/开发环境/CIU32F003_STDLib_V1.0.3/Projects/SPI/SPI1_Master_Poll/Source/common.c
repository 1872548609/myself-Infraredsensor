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
* @brief  GPIO 初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};
    
    /* GPIO外设时钟使能 */  
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA | RCC_PERIPH_CLK_GPIOB);
    /* SPI1 GPIO 配置    
        PB1     ------> SPI1_NSS
        PB0     ------> SPI1_SCK
        PA1     ------> SPI1_MISO
        PA0     ------> SPI1_MOSI
    */
    gpio_config.pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    gpio_config.alternate = GPIO_AF0_SPI1;
    std_gpio_init(GPIOA, &gpio_config);

    gpio_config.pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_config.pull = GPIO_PULLUP;
    std_gpio_init(GPIOB, &gpio_config);
}


/**
* @brief  SPI1初始化
* @retval 无
*/
void spi1_init(void)
{
    std_spi_init_t spi_config = {0};
    
    /* SPI1时钟使能 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_SPI1);
    
    /* 禁止SPI */
    std_spi_disable();
    
    /* SPI结构体初始化 */
    spi_config.mode = SPI_MODE_MASTER;
    spi_config.baud_rate_prescaler = SPI_BAUD_PCLKDIV_32;
    spi_config.clk_polarity = SPI_POLARITY_HIGH;
    spi_config.clk_phase = SPI_PHASE_1EDGE;
    spi_config.bitorder = SPI_FIRSTBIT_MSB;
    std_spi_init(&spi_config);
    
    /* SPI主机模式，配置NSS输出使能 */
    std_spi_nss_output_enable();
    
    /* SPI使能 */
    std_spi_enable();
}




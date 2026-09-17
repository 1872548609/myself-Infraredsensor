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

    /* 使能UART对应的GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);

    /* GPIO引脚配置  
        PA3    ------> TX  
        PA4    ------> RX  
    */
    gpio_config.pin = GPIO_PIN_3|GPIO_PIN_4;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.pull = GPIO_PULLUP;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    gpio_config.alternate = GPIO_AF1_UART1;
    std_gpio_init(GPIOA,&gpio_config);
  
}

/**
* @brief  UART 初始化
* @retval 无
*/
void uart_init(void)
{
    std_uart_init_t uart_confg = {0};
    /* UART 时钟使能 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_UART1);
        
    /* UART 初始化 */
    uart_confg.baudrate = 115200;
    uart_confg.direction = UART_DIRECTION_SEND_RECEIVE;
    uart_confg.wordlength = UART_WORDLENGTH_8BITS;
    uart_confg.stopbits = UART_STOPBITS_1;
    uart_confg.parity = UART_PARITY_NONE;
    std_uart_init(UART1,&uart_confg);

    /* 使能UART */
    std_uart_enable(UART1);
}

/**
* @brief  NVIC 初始化
* @retval 无
*/
void nvic_init(void)
{
    /* 配置UART1中断优先级以及使能UART1的NVIC中断 */   
    NVIC_SetPriority(UART1_IRQn,NVIC_PRIO_1);    
    NVIC_EnableIRQ(UART1_IRQn);
}



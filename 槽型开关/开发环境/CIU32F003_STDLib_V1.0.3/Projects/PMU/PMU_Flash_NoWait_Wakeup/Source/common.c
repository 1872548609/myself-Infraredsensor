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
#define PERIODVALUE          (0xA000)             /* 定时周期(ARR) */


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
* @brief  LPTIM初始化
* @retval 无
*/
void lptim_init(void)
{
    /* 使能RCL时钟并等待时钟稳定 */
    std_rcc_rcl_enable();
    while(std_rcc_get_rcl_ready() != RCC_CSR2_RCLRDY);
    
    /* 选择LPTIM1的时钟源为RCL */
    std_rcc_set_lptim1clk_source(RCC_LPTIM1_ASYNC_CLK_SRC_RCL);
    
    /* 使能LPTIM1时钟 */
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_LPTIM1);
    
    /* 使能自动重载匹配中断 */
    std_lptim_interrupt_enable(LPTIM_INTERRUPT_ARRM);  
    
    /* 配置LPTIM计数预分频为2分频 */
    std_lptim_set_prescaler(LPTIM_PRESCALER_DIV2);
    
    /* 使能LPTIM1 */
    std_lptim_enable();
        
    /* 配置LPTIM1计数周期 */
    std_lptim_set_auto_reload(PERIODVALUE); 
    
    /* 配置LPTIM中断挂起唤醒 */    
    std_exti_interrupt_enable(EXTI_LINE_LPTIM1);
    std_exti_event_disable(EXTI_LINE_LPTIM1);    
}


/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
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



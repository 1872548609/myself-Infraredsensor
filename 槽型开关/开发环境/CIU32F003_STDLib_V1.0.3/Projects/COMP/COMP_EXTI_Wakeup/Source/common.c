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
* @brief  COMP1初始化配置函数
* @retval 无
*/
void comp1_init(void)
{
    /* 使能COMP的时钟 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_COMP);
       
    /* 配置COMP1的正相输入端PB0, 反相输入端是内部参考电压 */
    std_comp_input_config(COMP1,COMP_INPSEL_IO1,COMP_INMSEL_INVREF);
    
    /* 配置COMP1 输出不反相 */
    std_comp_set_output_polarity(COMP1,COMP_OUTPOL_NON_INVERTED);
    
    /* 禁止输入迟滞 */
    std_comp_input_hysteresis_disable();
    
    /* 使用VBGR作为内部参考电压 */
    std_comp_set_reference_source(COMP_REFERENCE_VBGR);
    
    /* 使能COMP1 */
    std_comp_enable(COMP1);

    /* 等待COMP1启动稳定 */
    std_delayus(COMP_EN_DELAY);
    
    /* 上升沿触发 */ 
    std_exti_rising_trigger_enable(EXTI_LINE_COMP1);
     
    /* 使能EXTI 的中断使能 */    
    std_exti_interrupt_enable(EXTI_LINE_COMP1);
}


/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};
     
    /* 使能GPIO时钟 */
    std_rcc_gpio_clk_enable( RCC_PERIPH_CLK_GPIOB);
    
    /* LED初始化 */
    gpio_config.pin = LED_PIN;
    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    gpio_config.pull = GPIO_PULLUP;
    std_gpio_init(LED_GPIO_PORT, &gpio_config);
    
    /* 配置COMP1正相输入 PB0 模拟模式 */
    gpio_config.pin = GPIO_PIN_0;
    gpio_config.mode = GPIO_MODE_ANALOG;
    gpio_config.pull = GPIO_NOPULL;
    std_gpio_init(GPIOB, &gpio_config);
    
    /* 熄灭LED */
    std_gpio_set_pin(LED_GPIO_PORT,LED_PIN);
}


/**
* @brief  NVIC初始化配置
* @retval 无
*/
void nvic_init(void)
{
    NVIC_SetPriority(ADC_COMP_IRQn, NVIC_PRIO_0);
    NVIC_EnableIRQ(ADC_COMP_IRQn);
}

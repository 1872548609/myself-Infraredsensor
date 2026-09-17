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
* @brief  COMP初始化
* @note   初始化COMP1和COMP2作为窗口比较器
* @retval 无
*/
void comp_init(void)
{
    /* 使能COMP时钟 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_COMP);
    
    /* 配置COMP 使用VDDA DIV 作为内部参电压源  */
    std_comp_set_reference_source(COMP_REFERENCE_VDDA_DIV);
    
    /* 配置VCDIV 是10/16 */   
    std_comp_set_ref_vdda_div(COMP_VDDA_DIV_10DIV16);
    
    /* 使能输入迟滞 */
    std_comp_input_hysteresis_enable();
    
    /* COMP1 独立比较器，正相PB0 ,反相是内部参考电压 */
    std_comp_input_config(COMP1,COMP_INPSEL_IO1,COMP_INMSEL_INVREF);
    
    /* COMP1 输出结果和COMP2异或*/
    std_comp_set_output_mode(COMP1,COMP_OUTMODE_COMMON_XOR_OUT);
    
    /* COMP2 配置为窗口比较模式 */
    std_comp_set_input_plus_mode(COMP2,COMP_INPMODE_COMMON_INPUT);
    
    /* COMP2 反相设置,PA4   */
    std_comp_set_input_minus(COMP2,COMP_INMSEL_IO);
    
    /* 使能COMP1 */
    std_comp_enable(COMP1);

    /* 使能COMP2 */
    std_comp_enable(COMP2);

    /* 等待COMP启动稳定*/
    std_delayus(COMP_EN_DELAY);
}


/**
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};
    
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA | RCC_PERIPH_CLK_GPIOB);
    
     /* LED初始化 */
    gpio_config.pin = LED_PIN;
    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    gpio_config.pull = GPIO_PULLUP;
    std_gpio_init(LED_GPIO_PORT, &gpio_config);
    
    /*COMP1 正相输入端 PB0 */
    gpio_config.pin = GPIO_PIN_0;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.mode = GPIO_MODE_ANALOG;
    std_gpio_init(GPIOB, &gpio_config);
    
    /*COMP2 反相输入端 PA4 */
    gpio_config.pin = GPIO_PIN_4;
    std_gpio_init(GPIOA, &gpio_config);
    
    /* COMP1 输出 PB3 */
    gpio_config.pin = GPIO_PIN_3;
    gpio_config.mode = GPIO_MODE_ALTERNATE;
    gpio_config.alternate = GPIO_AF4_COMP1;
    std_gpio_init(GPIOB,&gpio_config);
     
    /* 熄灭LED */
    std_gpio_set_pin(LED_GPIO_PORT,LED_PIN);
}

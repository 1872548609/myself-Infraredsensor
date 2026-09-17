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
* @brief  GPIO初始化
* @retval 无
*/
void gpio_init(void)
{
    std_gpio_init_t  gpio_config = {0};

    /* 使能GPIO时钟 */
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA | RCC_PERIPH_CLK_GPIOB);
    
    /* USER BUTTON初始化 */
    gpio_config.pin = USER_BUTTON_PIN;
    gpio_config.mode = GPIO_MODE_INPUT;
    gpio_config.pull = GPIO_NOPULL;
    std_gpio_init(USER_BUTTON_GPIO_PORT, &gpio_config);    
    
    /* 配置EXTI，中断方式 */
    std_exti_set_gpio(USER_BUTTON_EXTI_PORT, USER_BUTTON_EXTI_LINE);    
    std_exti_falling_trigger_enable(USER_BUTTON_EXTI_LINE);  
    std_exti_interrupt_enable(USER_BUTTON_EXTI_LINE); 
    
    /* ADC_IN4输入通道配置：PA7 */
    gpio_config.pin = GPIO_PIN_7;
    gpio_config.mode = GPIO_MODE_ANALOG;
    gpio_config.pull = GPIO_NOPULL;
    std_gpio_init(GPIOA, &gpio_config);

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
* @brief  ADC初始化配置函数
* @retval 无
*/
void adc_init(void)
{
    /* 使能ADC时钟 */
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_ADC);
       
    /* ADC_CK时钟为PCLK的3分频 */
    std_adc_clock_config(ADC_CK_DIV3);
    
    /* 软件触发ADC */
    std_adc_trig_sw();

    /* 单次转换模式 */
    std_adc_conversion_mode_config(ADC_SINGLE_CONVER_MODE);
    
    /* 采样时间配置，239个周期*/
    std_adc_sampt_time_config(ADC_SAMPTIME_239CYCLES);
     
    /* 选择通道4 */
    std_adc_fix_sequence_channel_enable(ADC_CHANNEL_VBGR | ADC_CHANNEL_4);
    
    /* 配置wait模式，避免数据未及时读取，转换溢出 */
    std_adc_wait_mode_enable();

    /* 使能ADC */
    std_adc_enable();
    
    /* 等待ADC使能状态稳定 */
    std_delayus(ADC_EN_DELAY);
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
* @brief  uart初始化
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

#if defined ( __GNUC__ )

/**
* @brief  重定向c库函数printf到串口，重定向后可使用printf函数
* @param  fd 文件描述符
* @param  ptr 待发送字符串
* @param  len 待发送字符串长度
* @retval len 待发送字符串长度
*/
int _write(int fd, char* ptr, int len)
{
    uint32_t i = 0;

    /* 发送一个字节数据到串口 */
    for (; i<len; i++) 
    {
        std_uart_tx_write_data(UART1, (uint32_t)ptr[i]);

        /* 等待发送完成 */
        while(!std_uart_get_flag(UART1, UART_FLAG_TC));
    }

    return len;
}

#else

/**
* @brief  重定向c库函数printf到串口，重定向后可使用printf函数
* @param  ch 待发送字符
* @retval ch 发送的字符
*/
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口 */
    std_uart_tx_write_data(UART1, (uint32_t)ch);
    
    /* 等待发送完毕 */
    while(!std_uart_get_flag(UART1, UART_FLAG_TC));
    return ch;
}

#endif

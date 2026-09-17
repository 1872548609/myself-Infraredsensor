/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              本示例展示了UART中断方式收发数据的功能。
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "main.h"


/*--------------------------------------------variables-----------------------------------------*/
/* UART收发完成标志 */
__IO uint8_t g_uart_ready = 0;    

/*------------------------------------------functions-------------------------------------------*/
int main(void)
{      
    /* 配置系统时钟 */
    system_clock_config(); 

    /* GPIO初始化 */
    gpio_init();
    
    /* UART初始化 */
    uart_init();

    /* NVIC初始化 */
    nvic_init(); 
    
    while(1)
    {  
        /* UART 使能中断接收 */
        std_uart_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_RXNE);
 
        while(!g_uart_ready);
        g_uart_ready = 0;
        
        /* UART 使能中断发送 */
        std_uart_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_TXE);

        while(!g_uart_ready);
        g_uart_ready = 0;        
    }
}











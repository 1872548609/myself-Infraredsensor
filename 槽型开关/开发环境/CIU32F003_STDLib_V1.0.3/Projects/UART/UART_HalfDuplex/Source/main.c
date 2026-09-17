/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示UART的单线半双工功能。
*                     UART1先向UART2发送8个字节，之后UART2向UART1返回8个字节。                                                                                
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "main.h"

/*------------------------------------------functions-------------------------------------------*/
int main(void)
{      
    /* 配置系统时钟 */
    system_clock_config(); 
 
    /* GPIO初始化 */
    gpio_init();
    
    /* UART初始化 */
    uart_init();
 
    /* UART1、UART2使能单线半双工 */
    bsp_uart_set_half_duplex(UART1);
    bsp_uart_set_half_duplex(UART2);
    
    /* UART1 发送，UART2接收 */
    bsp_uart_tx_rx(UART1,UART2);
        
    /* UART2 发送，UART1 接收 */
    bsp_uart_tx_rx(UART2,UART1);
    
    while(1);
}

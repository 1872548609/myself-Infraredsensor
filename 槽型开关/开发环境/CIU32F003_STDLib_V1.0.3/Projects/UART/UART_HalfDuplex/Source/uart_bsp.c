/************************************************************************************************/
/**
* @file               uart_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              UART BSP函数，实现了UART在单线半双工模式下的通信。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "uart_bsp.h"

/*-------------------------------------------define---------------------------------------------*/
#define BUF_LEN   8

/*--------------------------------------------variables-----------------------------------------*/
uint8_t g_tx_buffer[BUF_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint8_t g_rx_buffer[BUF_LEN] = {0};

/*-------------------------------------------functions------------------------------------------*/

/**
* @brief  配置UART为单线半双工模式
* @param  uartx 发送UART外设
*             @arg UART1
*             @arg UART2
* @retval 无
*/
void bsp_uart_set_half_duplex(UART_t *uartx)
{
    /* 使能单线半双工 */
    std_uart_half_duplex_enable(uartx);
    std_uart_enable(uartx);
}    

/**
* @brief  在单线半双工模式下，UART1和UART2之间发送和接收数据
* @param  tx_uartx 发送UART外设
*             @arg UART1
*             @arg UART2
* @param  rx_uartx 接收UART外设
*             @arg UART1
*             @arg UART2
* @note   当tx_uartx是UART1时,rx_uartx应该是UART2,反之亦然。
* @retval 无
*/
void bsp_uart_tx_rx(UART_t *tx_uartx,UART_t *rx_uartx)
{    
    for(int i = 0; i < BUF_LEN; i++)
    {
        /* 查询发送 */
        while(!std_uart_get_flag(tx_uartx,UART_FLAG_TXE));

        std_uart_tx_write_data(tx_uartx, g_tx_buffer[i]);
        
        /* 查询接收 */
        while(!std_uart_get_flag(rx_uartx,UART_FLAG_RXNE));
         
        g_rx_buffer[i]= std_uart_rx_read_data(rx_uartx);
    }
}

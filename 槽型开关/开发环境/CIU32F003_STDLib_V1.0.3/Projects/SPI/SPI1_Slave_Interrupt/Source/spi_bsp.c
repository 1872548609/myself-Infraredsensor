/************************************************************************************************/
/**
* @file               spi_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              SPI BSP驱动函数，实现SPI中断服务，中断收发等功能。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "spi_bsp.h"

/*--------------------------------------------variables-----------------------------------------*/
uint8_t g_send_buffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t g_recv_buffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__IO uint32_t g_send_count = 0;
__IO uint32_t g_recv_count = 0;

/*-------------------------------------------functions------------------------------------------*/

/**
* @brief  SPI1中断服务函数
* @retval 无
*/
void SPI1_IRQHandler(void)
{
    /* 接收中断处理流程 */
    if(std_spi_get_flag(SPI_FLAG_RXFNE) == SPI_FLAG_RXFNE)
    {
        if(g_recv_count < 8)
        {
            g_recv_buffer[g_recv_count++] = std_spi_read_data();
        }
        if(g_recv_count >= 8)
        {
            std_spi_interrupt_disable(SPI_INTERRUPT_RXFNE);
        }
    }
    
    /* 发送中断处理流程 */
    if(std_spi_get_flag(SPI_FLAG_TXFE) == SPI_FLAG_TXFE)
    {
        if(g_send_count < 8)
        {
            std_spi_write_data(g_send_buffer[g_send_count++]);
        }
        if(g_send_count >= 8)
        {
            std_spi_interrupt_disable(SPI_INTERRUPT_TXFE);
        }
    }
}


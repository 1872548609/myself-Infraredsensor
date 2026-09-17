/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示SPI主模式下，使用GPIO作为NSS片选信号，进行全双工数据收发的功能。
*                     SPI采用查询方式进行通信，通信速率为PCLK的32分频，即1.5MHz。
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
uint8_t g_send_buffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t g_recv_buffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*-------------------------------------------functions------------------------------------------*/
int main(void)
{
    uint32_t index = 0;
    
    /* 配置系统时钟 */
    system_clock_config();
    
    /* GPIO初始化 */
    gpio_init();

    /* SPI初始化 */
    spi1_init();
    
    /* SPI主机模式,拉低片选信号 */
    std_gpio_reset_pin(NSS_PORT, NSS_PIN);
    std_spi_set_nss_output(SPI_NSS_OUTPUT_LOW);

    /* SPI数据传输 */
    for(index=0; index<8; index++)
    {
        /* 等待发送数据寄存器为空 */
        while(std_spi_get_flag(SPI_FLAG_TXFE) != SPI_FLAG_TXFE);
        /* 发送数据 */ 
        std_spi_write_data(g_send_buffer[index]);
        
        /* 等待接收数据寄存器非空 */
        while(std_spi_get_flag(SPI_FLAG_RXFNE) != SPI_FLAG_RXFNE);
        /* 接收数据 */ 
        g_recv_buffer[index] = std_spi_read_data();
    }

    /* SPI主机拉高片选信号，结束通信 */
    while(!std_spi_get_flag(SPI_FLAG_TXFE));
    while(std_spi_get_flag(SPI_FLAG_BUSY));
    std_spi_set_nss_output(SPI_NSS_OUTPUT_HIGH);
    std_gpio_set_pin(NSS_PORT, NSS_PIN);
    
    while(1)
    {
    }
}













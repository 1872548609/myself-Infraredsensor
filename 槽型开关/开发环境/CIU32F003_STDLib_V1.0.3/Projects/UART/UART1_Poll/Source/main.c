/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示UART查询方式数据收发功能。
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

/*--------------------------------------------define-----------------------------------------*/
/* buffer长度 */
#define BUF_LEN  8
/*--------------------------------------------variables-----------------------------------------*/
/* 存放数据 */
uint8_t buffer[BUF_LEN] = {0};
/*------------------------------------------functions-------------------------------------------*/
int main(void)
{      
    /* 配置系统时钟 */
    system_clock_config(); 

    /* GPIO初始化 */
    gpio_init();
    
    /* UART初始化*/
    uart_init();

    while(1)
    {
        /* UART 查询接收 BUF_LEN个数据 */
        for(int i = 0; i<BUF_LEN; i++)
        {
            while(!std_uart_get_flag(UART1,UART_FLAG_RXNE));
            buffer[i] = std_uart_rx_read_data(UART1);
        }
        
        /* UART 查询发送 BUF_LEN个数据*/
        for(int i = 0; i<BUF_LEN; i++)
        {
            while(!std_uart_get_flag(UART1,UART_FLAG_TXE));
            std_uart_tx_write_data(UART1,buffer[i]);     
        }
        /* 等待数据发送完成*/
        while(!std_uart_get_flag(UART1,UART_FLAG_TC)); 
    }
}


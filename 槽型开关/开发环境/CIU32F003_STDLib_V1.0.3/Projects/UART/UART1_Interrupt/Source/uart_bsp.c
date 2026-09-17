/************************************************************************************************/
/**
* @file               uart_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              UART BSP函数，实现UART中断服务函数。
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


/*--------------------------------------------define-------------------------------------------*/
/* 定义BUF的长度*/
#define BUF_LEN  8

/*--------------------------------------------variables-----------------------------------------*/

/* 数据缓存BUF*/
static uint8_t g_buffer[BUF_LEN];

/* UART 当前收发数据的个数 */
static __IO uint8_t g_cur_count ;

extern __IO uint8_t g_uart_ready ; 
/*-------------------------------------------functions------------------------------------------*/

/**
* @brief  UART1 中断服务函数
* @retval 无
*/
void UART1_IRQHandler(void)
{
     /* 检查上溢错误 */
    if((std_uart_get_cr3_interrupt_err_enable(UART1)                          \
        || std_uart_get_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_RXNE))  \
        && std_uart_get_flag(UART1,UART_FLAG_ORE))
    {
        /* 清除上溢错误标志 */
        std_uart_clear_flag(UART1,UART_CLEAR_ORE);
        /* 用户可根据实际使用场景，自定义错误处理流程 */
    }
    
    /* 检查到接收寄存器非空标志后， 读取接收数据 */    
    if(std_uart_get_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_RXNE)     \
         && std_uart_get_flag(UART1,UART_FLAG_RXNE))
    {
        g_buffer[g_cur_count++] = std_uart_rx_read_data(UART1);
        
        if(g_cur_count == BUF_LEN)
        {
            std_uart_cr1_interrupt_disable(UART1,UART_CR1_INTERRUPT_RXNE);
            g_cur_count = 0;
            g_uart_ready = 1;
        }
    }
    /* 检查发送寄存器为空标志置1时，写入发送数据 */    
    if( std_uart_get_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_TXE)        \
         && std_uart_get_flag(UART1,UART_FLAG_TXE))
    {
        std_uart_tx_write_data(UART1, g_buffer[g_cur_count++]);
  
        if(g_cur_count == BUF_LEN)
        {
            /* 完成发送后，关闭TXE中断，开启发送完成中断 */    
            std_uart_cr1_interrupt_disable(UART1,UART_CR1_INTERRUPT_TXE);
            std_uart_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_TC);
        }
    }
    /* 检测到发送完成标志置1，将全局标志置1 */    
    if(std_uart_get_cr1_interrupt_enable(UART1,UART_CR1_INTERRUPT_TC)         \
        && std_uart_get_flag(UART1,UART_FLAG_TC))
    {
        std_uart_clear_flag(UART1,UART_FLAG_TC);
        std_uart_cr1_interrupt_disable(UART1,UART_CR1_INTERRUPT_TC);
        g_cur_count = 0;
        g_uart_ready = 1;  
    }
}






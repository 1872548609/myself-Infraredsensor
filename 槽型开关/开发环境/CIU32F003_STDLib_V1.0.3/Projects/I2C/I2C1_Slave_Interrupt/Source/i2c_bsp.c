/************************************************************************************************/
/**
* @file               i2c_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              I2C BSP驱动函数，实现I2C从模式中断方式通信。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "i2c_bsp.h"

/*------------------------------------------define----------------------------------------------*/
#define BUFF_SIZE     (8U)

/*------------------------------------------variables-------------------------------------------*/
uint8_t g_tx_buffer[BUFF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t g_rx_buffer[BUFF_SIZE] = {0};
uint32_t g_tx_count;
uint32_t g_rx_count;

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  I2C中断服务程序
* @retval 无
*/
void I2C1_IRQHandler(void)
{
    uint32_t current_status;
    
    /* 获取当前I2C1的状态 */
    current_status = I2C1->ISR;
    
    /* 地址匹配事件处理 */
    if ((current_status & I2C_FLAG_ADDR) == I2C_FLAG_ADDR)
    {
        /* 清除地址匹配中断标志位 */
        std_i2c_clear_flag(I2C_CLEAR_ADDR);
        /* 使能发送接收缓冲器中断 */
        std_i2c_interrupt_enable(I2C_INTERRUPT_BUF); 
        
        /* 通信计数值清0 */
        g_tx_count = 0;
        g_rx_count = 0;
    }
    
    /* I2C发送数据流程 */
    if ((current_status & I2C_FLAG_TXIS) == I2C_FLAG_TXIS)
    {
        if (g_tx_count < BUFF_SIZE)
        {
            std_i2c_transmit_byte(g_tx_buffer[g_tx_count]);
            g_tx_count++;
            
            /* 发送完成后，关闭发送接收缓冲器中断 */
            if (BUFF_SIZE == g_tx_count)
            {
                std_i2c_interrupt_disable(I2C_INTERRUPT_BUF);
            }
        }
    }
    
    /* I2C接收数据流程 */
    if ((current_status & I2C_FLAG_RXNE) == I2C_FLAG_RXNE)
    {
        if (g_rx_count < BUFF_SIZE)
        {
            g_rx_buffer[g_rx_count] = std_i2c_receive_byte();
            g_rx_count++;
            
            /* 接收完成后，关闭发送接收缓冲器中断 */
            if (BUFF_SIZE == g_rx_count)
            {
                std_i2c_interrupt_disable(I2C_INTERRUPT_BUF);
            }
        }
    }
    
    /* 接收到NACK事件处理 */
    if((current_status & I2C_FLAG_NACK) == I2C_FLAG_NACK)
    {
        /* 清除NACKF */
        std_i2c_clear_flag(I2C_CLEAR_NACK);
    }
    
    /* 接收到停止位事件处理 */
    if ((current_status & I2C_FLAG_STOP) == I2C_FLAG_STOP)
    {
        /* 清除STOPF */
        std_i2c_clear_flag(I2C_CLEAR_STOP);
    }
}


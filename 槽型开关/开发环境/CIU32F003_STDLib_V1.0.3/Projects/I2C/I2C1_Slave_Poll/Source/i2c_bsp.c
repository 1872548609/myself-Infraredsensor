/************************************************************************************************/
/**
* @file               i2c_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              I2C BSP驱动函数，实现I2C从模式查询方式通信。
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

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  I2C1查询方式发送数据流程
* @retval 无
*/
void bsp_i2c1_slave_send(void)
{
    uint32_t index;
    
    /* 等待地址匹配 */
    while(std_i2c_get_flag(I2C_FLAG_ADDR) != I2C_FLAG_ADDR);
    /* 清除地址匹配中断标志位 */
    std_i2c_clear_flag(I2C_CLEAR_ADDR);
    
    /* 确认传输方向 */
    while(std_i2c_get_flag(I2C_FLAG_DIR) != I2C_DIR_TX);
    
    for (index=0; index<BUFF_SIZE; index++)
    {
        /* 等待TXIS位置位 */
        while(std_i2c_get_flag(I2C_FLAG_TXIS) != I2C_FLAG_TXIS);
        std_i2c_transmit_byte(g_tx_buffer[index]);
    }
    
    /* 清除NACKF标志 */
    while(std_i2c_get_flag(I2C_FLAG_NACK) != I2C_FLAG_NACK);
    std_i2c_clear_flag(I2C_CLEAR_NACK);
    
    /* 清除STOPF */
    while(std_i2c_get_flag(I2C_FLAG_STOP) != I2C_FLAG_STOP);
    std_i2c_clear_flag(I2C_CLEAR_STOP);
}

/**
* @brief  I2C1查询方式接收数据流程
* @retval 无
*/
void bsp_i2c1_slave_receive(void)
{
    uint32_t index;
    
    /* 等待地址匹配 */
    while(std_i2c_get_flag(I2C_FLAG_ADDR) != I2C_FLAG_ADDR);
    /* 清除地址匹配中断标志位 */
    std_i2c_clear_flag(I2C_CLEAR_ADDR);
    
    /* 确认传输方向 */
    while(std_i2c_get_flag(I2C_FLAG_DIR) != I2C_DIR_RX);
    
    for (index=0; index<BUFF_SIZE; index++)
    {
        /* 等待RXNE位置位 */
        while(std_i2c_get_flag(I2C_FLAG_RXNE) != I2C_FLAG_RXNE);
        g_rx_buffer[index] = std_i2c_receive_byte();
    }
    
    /* 清除STOPF */
    while(std_i2c_get_flag(I2C_FLAG_STOP) != I2C_FLAG_STOP);
    std_i2c_clear_flag(I2C_CLEAR_STOP);
}

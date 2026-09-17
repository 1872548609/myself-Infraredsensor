/************************************************************************************************/
/**
* @file               i2c_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              I2C BSP驱动函数，使用GPIO模拟I2C主模式，实现I2C主模式通信协议。
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
#define BUFF_SIZE               (8U)
#define SLAVE_ADDRESS           (0x5C)
/*------------------------------------------variables-------------------------------------------*/
uint8_t g_tx_buffer[BUFF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t g_rx_buffer[BUFF_SIZE];

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  I2C生成起始位
* @retval 无
*/
void bsp_i2c_generate_start(void)
{
    /* 切换输出模式前固定电平状态以避免毛刺 */
    std_gpio_set_pin(I2C_PIN_PORT, (SCL_PIN | SDA_PIN));
    /* 设置SDA为输出模式 */
    std_gpio_set_pin_mode(I2C_PIN_PORT, SDA_PIN, GPIO_MODE_OUTPUT);
    i2c_delay_time(CLK_DELAY);
    std_gpio_reset_pin(I2C_PIN_PORT, SDA_PIN);
    i2c_delay_time(CLK_DELAY);
    std_gpio_reset_pin(I2C_PIN_PORT, SCL_PIN);
}


/**
* @brief  I2C生成停止位
* @retval 无
*/
void bsp_i2c_generate_stop(void)
{
    /* 切换输出模式前固定电平状态以避免毛刺 */
    std_gpio_reset_pin(I2C_PIN_PORT, (SCL_PIN | SDA_PIN));
    /* 设置SDA为输出模式 */
    std_gpio_set_pin_mode(I2C_PIN_PORT, SDA_PIN, GPIO_MODE_OUTPUT);
    i2c_delay_time(CLK_DELAY);
    std_gpio_set_pin(I2C_PIN_PORT, SCL_PIN);
    i2c_delay_time(CLK_DELAY);
    std_gpio_set_pin(I2C_PIN_PORT, SDA_PIN);
}


/**
* @brief  I2C生成ACK/NACK
* @param  ack_type ACK类型
*             @arg RET_ACK:  主机发送ACK
*             @arg RET_NACK: 主机发送NACK
* @retval 无
*/
void bsp_i2c_generate_ack(uint32_t ack_type)
{
    /* 切换输出模式前根据返回ACK/NACK固定电平状态以避免毛刺 */
    if(ack_type == RET_ACK)
    {
        std_gpio_reset_pin(I2C_PIN_PORT, SDA_PIN);
    }
    else
    {
        std_gpio_set_pin(I2C_PIN_PORT, SDA_PIN);
    }
    
    /* 设置SDA为输出模式 */
    std_gpio_set_pin_mode(I2C_PIN_PORT, SDA_PIN, GPIO_MODE_OUTPUT);
    
    i2c_delay_time(CLK_DELAY);
    std_gpio_set_pin(I2C_PIN_PORT, SCL_PIN);
    i2c_delay_time(CLK_DELAY);
    std_gpio_reset_pin(I2C_PIN_PORT, SCL_PIN);
}


/**
* @brief  I2C主模式发送一个字节数据
* @param  data 发送数据
* @retval uint32_t 返回从机ACK状态
*             @arg RET_ACK:  从机返回ACK
*             @arg RET_NACK: 从机返回NACK
*/
uint32_t bsp_i2c_master_send_byte(uint8_t send_data)
{
    uint32_t bit_cnt = 0;
    uint32_t ack_type = 0;
    
    /* 切换输出模式前固定电平状态以避免毛刺 */
    if (send_data & 0x80)
    {
        std_gpio_set_pin(I2C_PIN_PORT, SDA_PIN);
    }
    else
    {
        std_gpio_reset_pin(I2C_PIN_PORT, SDA_PIN);
    }
    
    /* 设置SDA为输出模式 */
    std_gpio_set_pin_mode(I2C_PIN_PORT, SDA_PIN, GPIO_MODE_OUTPUT);
    
    /* 发送8bit数据 */
    for (bit_cnt=0; bit_cnt < 8; bit_cnt++)
    {
        if ((send_data << bit_cnt) & 0x80)
        {
            std_gpio_set_pin(I2C_PIN_PORT, SDA_PIN);
        }
        else
        {
            std_gpio_reset_pin(I2C_PIN_PORT, SDA_PIN);
        }
        i2c_delay_time(CLK_DELAY);
        std_gpio_set_pin(I2C_PIN_PORT, SCL_PIN);
        i2c_delay_time(CLK_DELAY);
        std_gpio_reset_pin(I2C_PIN_PORT, SCL_PIN);
    }
    
    /* 判断从机返回ACK */
    std_gpio_set_pin_mode(I2C_PIN_PORT, SDA_PIN, GPIO_MODE_INPUT);
    i2c_delay_time(CLK_DELAY);
    std_gpio_set_pin(I2C_PIN_PORT, SCL_PIN);
    i2c_delay_time(ACK_DELAY);
    
    for (bit_cnt=0; bit_cnt < 5; bit_cnt++)
    {
        if (std_gpio_get_input_pin(I2C_PIN_PORT, SDA_PIN) == false)
        {
            ack_type = RET_ACK;
        }
        else
        {
            ack_type = RET_NACK;
            break;
        }
    }
    std_gpio_reset_pin(I2C_PIN_PORT, SCL_PIN);
    i2c_delay_time(CLK_DELAY);
    
    return ack_type;
}


/**
* @brief  I2C主模式接收一个字节数据
* @retval uint8_t 接收数据
*/
uint8_t bsp_i2c_master_receive_byte(void)
{
    uint32_t bit_cnt = 0;
    uint8_t receive_data = 0;
    
    /* 设置SDA为输入状态 */
    std_gpio_set_pin_mode(I2C_PIN_PORT, SDA_PIN, GPIO_MODE_INPUT);
    
    /* 接收8bit数据 */
    for (bit_cnt=0; bit_cnt<8; bit_cnt++)
    {
        std_gpio_reset_pin(I2C_PIN_PORT, SCL_PIN);
        i2c_delay_time(CLK_DELAY);
        std_gpio_set_pin(I2C_PIN_PORT, SCL_PIN);
        i2c_delay_time(CLK_DELAY);
        
        receive_data = receive_data << 1;
        if (std_gpio_get_input_pin(I2C_PIN_PORT, SDA_PIN) == true)
        {
            receive_data += 1;
        }
    }
    std_gpio_reset_pin(I2C_PIN_PORT, SCL_PIN);
    i2c_delay_time(CLK_DELAY);
    
    return receive_data;
}


/**
* @brief  I2C主模式发送数据流程
* @retval 无
*/
void bsp_i2c_master_send(void)
{
    uint32_t ack_type;
    uint32_t index;
    
    /* 发送start位 */
    bsp_i2c_generate_start();
    
    /* 发送从机地址及写请求 */
    ack_type = bsp_i2c_master_send_byte(SLAVE_ADDRESS | REQ_WRITE);
    if (ack_type != RET_ACK)
    {
        error_process();
    }
    
    /* 地址匹配转数据发送延时 */
    /* 用户可根据实际从机地址匹配后的时序进行调整 */
    i2c_delay_time(USER_DELAY);
    
    /* 发送数据 */
    for (index=0; index<BUFF_SIZE; index++)
    {
        ack_type = bsp_i2c_master_send_byte(g_tx_buffer[index]);
        if (ack_type != RET_ACK)
        {
            error_process();
        }
    }
    
    /* 发送stop位 */
    bsp_i2c_generate_stop();
}


/**
* @brief  I2C主模式接收数据
* @retval 无
*/
void bsp_i2c_master_receive(void)
{
    uint32_t ack_type;
    uint32_t index;
    
    /* 发送start位 */
    bsp_i2c_generate_start();
    
    /* 发送从机地址及读请求 */
    ack_type = bsp_i2c_master_send_byte(SLAVE_ADDRESS | REQ_READ);
    if (ack_type != RET_ACK)
    {
        error_process();
    }
    
    /* 地址匹配转数据接收延时 */
    /* 用户可根据实际从机地址匹配后的时序进行调整 */
    i2c_delay_time(USER_DELAY);
    
    /* 接收数据 */
    for (index=0; index<BUFF_SIZE-1; index++)
    {
        g_rx_buffer[index] = bsp_i2c_master_receive_byte();
        bsp_i2c_generate_ack(RET_ACK);
    }
    g_rx_buffer[index] = bsp_i2c_master_receive_byte();
    bsp_i2c_generate_ack(RET_NACK);
    
    /* 发送stop位 */
    bsp_i2c_generate_stop();
}

/************************************************************************************************/
/**
* @file               crc_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              CRC BSP驱动函数实现,实现CRC—16计算功能。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "crc_bsp.h"

/*-------------------------------------------functions------------------------------------------*/

/**
* @brief  按字节计算CRC值
* @param  buffer 指向待测数据缓冲区的指针
* @param  len  输入数据长度
* @retval uint16_t CRC计算结果
*/
uint16_t bsp_crc16_calulate(uint8_t *buffer, uint32_t len)
{
    uint32_t i;
    
    for (i=0; i<len; i++)
    {
        std_crc_set_byte(buffer[i]);
    }
    
    return (std_crc_get_value());
}


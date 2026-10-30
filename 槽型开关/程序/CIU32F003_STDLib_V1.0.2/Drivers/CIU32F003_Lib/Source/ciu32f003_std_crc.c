/************************************************************************************************/
/**
* @file               ciu32f003_std_crc.c
* @author             MCU Ecosystem Development Team
* @brief              CRC STD库驱动。
*                     实现CRC初始化配置等功能API。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/************************************************************************************************/
/**
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @addtogroup CRC
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"

#ifdef STD_CRC_PERIPHERAL_USED

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @addtogroup CRC_External_Functions 
* @{
*
*/
/************************************************************************************************/
/**
* @brief  CRC去初始化
* @retval 无
*/
void std_crc_deinit(void)
{
    /* 复位CRC */
    std_rcc_ahb_reset(RCC_PERIPH_RESET_CRC); 
}


/**
* @brief  将CRC初始值反转后写入寄存器
* @param  poly_sel   多项式选择
* @param  init_value 自定义初始值
* @retval 无
*/
void std_crc_set_init_value_invert(uint32_t poly_sel, uint32_t init_value)
{
    uint32_t index, temp; 
    uint32_t result = 0;
    
    /* 初始值按位翻转 */
    for (index = 0U; index < 4; index++)
    {
        temp = (init_value >> (8U * index));
        temp = (((temp & 0x55) << 1) | ((temp & 0xaa) >> 1));
        temp = (((temp & 0x33) << 2) | ((temp & 0xcc) >> 2));
        temp = (((temp & 0x0f) << 4) | ((temp & 0xf0) >> 4));
        result |= (temp << (8U * (3 - index)));
    }

    if (poly_sel == CRC_POLY_16) 
    {
        result >>= 16;
    }
    CRC->RDR = result;
}

/**
* @}
*/

#endif /* STD_CRC_PERIPHERAL_USED */

/** 
* @} 
*/

/** 
* @} 
*/

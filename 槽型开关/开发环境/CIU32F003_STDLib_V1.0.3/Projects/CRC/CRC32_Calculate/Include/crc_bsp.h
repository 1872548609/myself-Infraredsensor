/************************************************************************************************/
/**
* @file               crc_bsp.h
* @author             MCU Ecosystem Development Team
* @brief              CRC BSP头文件。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef CRC_BSP_H
#define CRC_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"
        
/*-------------------------------------------functions------------------------------------------*/
uint32_t bsp_crc32_calulate(uint8_t *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* CRC_BSP_H */


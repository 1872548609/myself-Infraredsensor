/************************************************************************************************/
/**
* @file               irtim_bsp.h
* @author             MCU Ecosystem Development Team
* @brief              IRTIM BSP头文件
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef IRTIM_BSP_H
#define IRTIM_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"
    
    
/*-------------------------------------------functions------------------------------------------*/
void bsp_irtim_modulation(void);
    

#ifdef __cplusplus
}
#endif

#endif /* IRTIM_BSP_H */

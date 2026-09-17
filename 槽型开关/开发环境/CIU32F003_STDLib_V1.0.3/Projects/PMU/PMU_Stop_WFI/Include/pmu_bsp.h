/************************************************************************************************/
/**
* @file               pmu_bsp.h
* @author             MCU Ecosystem Development Team
* @brief              PMU BSP头文件。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/


/* 避免头文件重复引用 */
#ifndef PMU_BSP_H
#define PMU_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "common.h"
    
    
/*-------------------------------------------functions------------------------------------------*/
void EXTI2_3_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif /* PMU_BSP_H */


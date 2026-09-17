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


/*避免头文件重复引用*/
#ifndef PMU_BSP_H
#define PMU_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"
  

/*--------------------------------------------variables-----------------------------------------*/
extern uint32_t g_flash_delay_time;
        
    
/*-------------------------------------------functions------------------------------------------*/
void bsp_pmu_deepstop_flash_wakeup_time(void);
    
    

#ifdef __cplusplus
}
#endif

#endif /* PMU_BSP_H */


/************************************************************************************************/
/**
* @file               adc_bsp.h
* @author             MCU Ecosystem Development Team
* @brief              ADC BSP头文件。
*                           
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/


/* 避免头文件重复引用 */
#ifndef ADC_BSP_H
#define ADC_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"
    
/*------------------------------------------variables-------------------------------------------*/
extern __IO uint8_t g_interrupt_status;
    
/*------------------------------------------functions-------------------------------------------*/
void ADC_COMP_IRQHandler(void);
void bsp_adc_software_calibrate(void);
    
#ifdef __cplusplus
}
#endif

#endif /* ADC_BSP_H */


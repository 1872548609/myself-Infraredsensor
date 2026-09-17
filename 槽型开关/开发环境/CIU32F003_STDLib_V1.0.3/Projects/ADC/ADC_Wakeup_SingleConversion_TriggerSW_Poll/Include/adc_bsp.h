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

/*-------------------------------------------includes-------------------------------------------*/
#include "ciu32F003_std.h"
    
/*--------------------------------------------define--------------------------------------------*/
/* ADC采样次数，根据实际需求可调整此值 */
#define ADC_SAMPLE_NUM               (10U)
/* ADC外部通道数 */
#define ADC_CHANNEL_NUM              (1U)
    
/*-------------------------------------------functions------------------------------------------*/
void EXTI2_3_IRQHandler(void);
void bsp_adc_software_calibrate(void);
void bsp_adc_get_channel_voltage(uint32_t *conversion_result);

#ifdef __cplusplus
}
#endif

#endif /* ADC_BSP_H */


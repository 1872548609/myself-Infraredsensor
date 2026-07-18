/**
 * @file    adc_anychannel_singleconversion_interrupt.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _ADC_ANYCHANNEL_SINGLECONVERSION_INTERRUPT_H_
#define _ADC_ANYCHANNEL_SINGLECONVERSION_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32f003_device.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _ADC_ANYCHANNEL_SINGLECONVERSION_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint32_t ADC_InterruptFlag;

/* Exported functions *************************************************************************************************/
void ADC_AnyChannel_SingleConversion_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _ADC_ANYCHANNEL_SINGLECONVERSION_INTERRUPT_H_ */


/**
 * @file    adc_analogwatchdog.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _ADC_ANALOGWATCHDOG_H_
#define _ADC_ANALOGWATCHDOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32_conf.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _ADC_ANALOGWATCHDOG_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint32_t ADC_InterruptFlag;

/* Exported functions *************************************************************************************************/
void ADC_AnalogWatchdog_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _ADC_ANALOGWATCHDOG_H_ */


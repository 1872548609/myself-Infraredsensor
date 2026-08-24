/**
 * @file    platform.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    29-Jan-2024
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32_conf.h"

/* Exported types *****************************************************************************************************/
typedef enum
{
    LED1,
    LED2,
    LED3,
    LED4
} LEDn_TypeDef;

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _PLATFORM_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint32_t PLATFORM_DelayTick;

/* Exported functions *************************************************************************************************/
void PLATFORM_DelayMS(uint32_t Millisecond);
void PLATFORM_LED_Enable(LEDn_TypeDef LEDn, FunctionalState State);
void PLATFORM_LED_Toggle(LEDn_TypeDef LEDn);
void PLATFORM_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* _PLATFORM_H_ */


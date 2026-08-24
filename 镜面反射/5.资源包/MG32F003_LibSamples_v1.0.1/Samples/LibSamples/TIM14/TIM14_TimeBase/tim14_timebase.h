/**
 * @file    tim14_timebase.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _TIM14_TIMEBASE_H_
#define _TIM14_TIMEBASE_H_

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

#ifdef _TIM14_TIMEBASE_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint32_t TIM14_UpdateFlag;

/* Exported functions *************************************************************************************************/
void TIM14_TimeBase_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _TIM14_TIMEBASE_H_ */


/**
 * @file    pwr_pvd_interrupt.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _PWR_PVD_INTERRUPT_H_
#define _PWR_PVD_INTERRUPT_H_

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

#ifdef _PWR_PVD_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint32_t PWR_PVD_InterruptFlag;

/* Exported functions *************************************************************************************************/
void PWR_PVD_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _PWR_PVD_INTERRUPT_H_ */


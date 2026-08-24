/**
 * @file    mg32f003_it.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _MG32F003_IT_H_
#define _MG32F003_IT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/

/* Exported functions *************************************************************************************************/
void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* _MG32F003_IT_H_ */


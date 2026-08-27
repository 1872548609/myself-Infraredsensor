/**
 * @file    mg32_dbg.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the functions prototypes for the DBG firmware
 *          library.
 */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __MG32_DBG_H
#define __MG32_DBG_H

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32f003_device.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @defgroup DBG
  * @{
  */

/** @defgroup DBG_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup DBG_Exported_Constants
  * @{
  */
#define DBG_SLEEP                 (0x01U << DBG_CR_DBG_SLEEP_Pos)        /*!< Debug Sleep mode */
#define DBG_STOP                  (0x01U << DBG_CR_DBG_STOP_Pos)         /*!< Debug stop mode */
#define DBG_STOP_FOR_LDO          (0x01U << DBG_CR_DBG_STOP_FOR_LDO_Pos) /*!< DBG_STOP_FOR_LDO mode */
#define DBG_IWDG_STOP             (0x01U << DBG_CR_DBG_IWDG_STOP_Pos)    /*!< DBG_IWDG_STOP mode */
#define DBG_TIM1_STOP             (0x01U << DBG_CR_DBG_TIM1_STOP_Pos)    /*!< DBG_TIM1_STOP */
#define DBG_TIM3_STOP             (0x01U << DBG_CR_DBG_TIM3_STOP_Pos)    /*!< DBG_TIM3_STOP */
#define DBG_TIM1_PWM_OFF          (0x01U << DBG_CR_DBG_TIM1_PWM_OFF_Pos) /*!< TIM1 outputs all 0 at debug halt */
#define DBG_TIM3_PWM_OFF          (0x01U << DBG_CR_DBG_TIM3_PWM_OFF_Pos) /*!< TIM2 outputs all 0 at debug halt */
#define DBG_TIM14_STOP            (0x01U << DBG_CR_DBG_TIM14_STOP_Pos)   /*!< TIM14 stops work when the core enters the debug mode */

/**
  * @}
  */

/** @defgroup DBG_Exported_Functions
  * @{
  */
void DBG_Configure(uint32_t periph, FunctionalState state);
uint32_t DBG_GetDeviceIDCode(void);

#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


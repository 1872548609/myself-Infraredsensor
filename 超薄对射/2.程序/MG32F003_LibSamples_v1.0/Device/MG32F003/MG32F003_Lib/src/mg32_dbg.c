/**
 * @file    mg32_dbg.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the DBG firmware functions
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_dbg.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup DBG
  * @{
  */

/** @defgroup DBG_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup DBG_Private_Functions
  * @{
  */

/**
  * @brief  Enables or disables the specified DBG peripheral.
  * @param  periph: DBG peripheral.
  *     @arg DBG_SLEEP
  *     @arg DBG_STOP
  *     @arg DBG_STOP_FOR_LDO
  *     @arg DBG_IWDG_STOP
  *     @arg DBG_TIM1_STOP
  *     @arg DBG_TIM3_STOP
  *     @arg DBG_TIM1_PWM_OFF
  *     @arg DBG_TIM3_PWM_OFF
  *     @arg DBG_TIM14_STOP
  * @param  state: new state of the specified DBG peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void DBG_Configure(uint32_t periph, FunctionalState state)
{
    (state) ?             \
    (DBG->CR |= periph) : \
    (DBG->CR &= ~periph);
}

/**
  * @brief  Returns the device identifier..
  * @param  None.
  * @retval return the device Device identifier.
  */
uint32_t DBG_GetDeviceIDCode(void)
{
    return ((uint32_t)DBG->IDCODE);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


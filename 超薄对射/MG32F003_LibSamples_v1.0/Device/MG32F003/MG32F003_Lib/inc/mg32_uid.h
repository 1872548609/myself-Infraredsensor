/**
 * @file    mg32_uid.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the functions prototypes for the UID firmware
 *          library.
 */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __MG32_UID_H
#define __MG32_UID_H

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32f003_device.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @defgroup UID
  * @{
  */

/** @defgroup UID_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup UID_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @defgroup UID_Exported_Functions
  * @{
  */
uint32_t Get_ChipsetUIDw0(void);
uint32_t Get_ChipsetUIDw1(void);
uint32_t Get_ChipsetUIDw2(void);

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


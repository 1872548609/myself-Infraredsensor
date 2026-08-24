/**
 * @file    mg32_crc.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the functions prototypes for the CRC firmware
 *          library.
 */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __MG32_CRC_H
#define __MG32_CRC_H

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32f003_device.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @defgroup CRC
  * @{
  */

/** @defgroup CRC_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup CRC_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @defgroup CRC_Exported_Functions
  * @{
  */
void CRC_ResetDR(void);
uint32_t CRC_CalcCRC(uint32_t data);
uint32_t CRC_CalcBlockCRC(uint32_t *buffer, uint32_t length);
uint32_t CRC_GetCRC(void);
void CRC_SetIndependentDataRegister(uint8_t id_value);
uint8_t CRC_GetIndependentDataRegister(void);

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


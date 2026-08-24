/**
 * @file    mg32_crc.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the CRC firmware functions
 */


/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_crc.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup CRC
  * @{
  */

/** @defgroup CRC_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup CRC_Private_Functions
  * @{
  */
/**
  * @brief  Resets the CRC Data register (DR).
  * @param  None.
  * @retval None.
  */
void CRC_ResetDR(void)
{
    CRC->CR |= CRC_CR_RST_Msk;
}

/**
  * @brief  Computes the 32-bit CRC of a given data word(32-bit).
  * @param  Data: data word(32-bit) to compute its CRC
  * @retval 32-bit CRC
  */
uint32_t CRC_CalcCRC(uint32_t data)
{
    CRC->DR = data;
    return (CRC->DR);
}

/**
  * @brief  Computes the 32-bit CRC of a given buffer of data word(32-bit).
  * @param  buffer: pointer to the buffer containing the data to be computed
  * @param  length: length of the buffer to be computed
  * @retval 32-bit CRC
  */
uint32_t CRC_CalcBlockCRC(uint32_t *buffer, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        CRC->DR = buffer[i];
    }

    return (CRC->DR);
}

/**
  * @brief  Returns the current CRC value.
  * @param  None.
  * @retval 32-bit CRC
  */
uint32_t CRC_GetCRC(void)
{
    return (CRC->DR);
}

/**
  * @brief  Stores a 8-bit data in the Independent Data(ID) register.
  * @param  id_value: 8-bit value to be stored in the ID register
  * @retval None.
  */
void CRC_SetIndependentDataRegister(uint8_t id_value)
{
    CRC->IDR = id_value;
}

/**
  * @brief  Returns the 8-bit data stored in the Independent Data(ID) register
  * @param  None.
  * @retval 8-bit value of the ID register
  */
uint8_t CRC_GetIndependentDataRegister(void)
{
    return (CRC->IDR);
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

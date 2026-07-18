/**
 * @file    mg32_uid.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the UID firmware functions
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_uid.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup UID
  * @{
  */

/** @defgroup UID_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup UID_Private_Functions
  * @{
  */

/**
  * @brief  Returns first word of the unique device identifier (UID based on 96 bits)
  * @param  None.
  * @retval Device identifier
  */
uint32_t Get_ChipsetUIDw0(void)
{
    return (*((__IO uint32_t *)UID_BASE));
}

/**
  * @brief  Returns second word of the unique device identifier (UID based on 96 bits)
  * @param  None.
  * @retval Device identifier
  */
uint32_t Get_ChipsetUIDw1(void)
{
    return (*((__IO uint32_t *)(UID_BASE + 4U)));
}

/**
  * @brief  Returns third word of the unique device identifier (UID based on 96 bits)
  * @param  None.
  * @retval Device identifier
  */
uint32_t Get_ChipsetUIDw2(void)
{
    return (*((__IO uint32_t *)(UID_BASE + 8U)));
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


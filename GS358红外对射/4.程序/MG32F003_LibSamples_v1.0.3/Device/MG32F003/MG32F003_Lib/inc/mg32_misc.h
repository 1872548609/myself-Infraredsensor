/**
 * @file    mg32_misc.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the functions prototypes for the MISC firmware
 *          library.
 */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __MG32_MISC_H
#define __MG32_MISC_H

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32f003_device.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @defgroup NVIC
  * @{
  */

/** @defgroup NVIC_Exported_Types
  * @{
  */

/**
  * @brief  NVIC Init Structure definition
  */
typedef struct
{
    uint8_t         NVIC_IRQChannel;
    uint8_t         NVIC_IRQChannelPriority;
    FunctionalState NVIC_IRQChannelCmd;
} NVIC_InitTypeDef;
/**
  * @}
  */

/** @defgroup NVIC_Exported_Constants
  * @{
  */
#define NVIC_VectTab_RAM                (0x20000000U)
#define NVIC_VectTab_FLASH              (0x08000000U)

#define NVIC_LP_SEVONPEND               (0x10U)
#define NVIC_LP_SLEEPDEEP               (0x04U)
#define NVIC_LP_SLEEPONEXIT             (0x02U)

#define AIRCR_VECTKEY_MASK              (0x05FA0000U)

#define SysTick_CLKSource_EXTCLK        (0xFFFFFFFBU)
#define SysTick_CLKSource_HCLK          (0x00000004U)
/**
  * @}
  */

/** @defgroup NVIC_Exported_Functions
  * @{
  */
void NVIC_Init(NVIC_InitTypeDef *init_struct);
void SysTick_CLKSourceConfig(uint32_t systick_clk_source);
void NVIC_SystemLPConfig(uint8_t low_power_mode, FunctionalState state);

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


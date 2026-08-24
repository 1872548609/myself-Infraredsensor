/**
 * @file    mg32_iwdg.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the functions prototypes for the IWDG firmware
 *          library.
 */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef __MG32_IWDG_H
#define __MG32_IWDG_H

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32f003_device.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @defgroup IWDG
  * @{
  */

/** @defgroup IWDG_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup IWDG_Exported_Constants
  * @{
  */

/**
  * @brief  Write access to IWDG_PR and IWDG_RLR registers
  */
#define IWDG_WriteAccess_Enable         0x5555 /* Enable write */
#define IWDG_WriteAccess_Disable        0x0000 /* Disable write */

/**
  * @brief  IWDG prescaler
  */
#define IWDG_Prescaler_4                (0x00U << IWDG_PR_PR_Pos)
#define IWDG_Prescaler_8                (0x01U << IWDG_PR_PR_Pos)
#define IWDG_Prescaler_16               (0x02U << IWDG_PR_PR_Pos)
#define IWDG_Prescaler_32               (0x03U << IWDG_PR_PR_Pos)
#define IWDG_Prescaler_64               (0x04U << IWDG_PR_PR_Pos)
#define IWDG_Prescaler_128              (0x05U << IWDG_PR_PR_Pos)
#define IWDG_Prescaler_256              (0x06U << IWDG_PR_PR_Pos)

/**
  * @brief  IWDG flag
  */
#define IWDG_FLAG_PVU                   (0x01U << IWDG_SR_PVU_Pos) /* IWDG prescaler value update flag */
#define IWDG_FLAG_RVU                   (0x01U << IWDG_SR_RVU_Pos) /* IWDG counter reload value update flag */
#define IWDG_FLAG_IVU                   (0x01U << IWDG_SR_IVU_Pos) /* IWDG Interrupt Generate value update flag */

/**
  * @brief  IWDG Key Reload
  */
#define KR_KEY_Reload                   0xAAAA                     /* Reload value */
#define KR_KEY_Enable                   0xCCCC                     /* Start IWDG */

/**
  * @brief  IWDG Overflow Configration
  */
#define IWDG_Overflow_Reset             (0x00U << IWDG_CR_IRQ_SEL_Pos)
#define IWDG_Overflow_Interrupt         (0x01U << IWDG_CR_IRQ_SEL_Pos)
/**
  * @}
  */

/** @defgroup IWDG_Exported_Functions
  * @{
  */
void IWDG_WriteAccessCmd(uint16_t write_access);
FlagStatus IWDG_GetFlagStatus(uint32_t flag);
void PVU_CheckStatus(void);
void RVU_CheckStatus(void);
void IVU_CheckStatus(void);
void IWDG_SetPrescaler(uint32_t prescaler);
void IWDG_SetReload(uint16_t reload);
uint32_t IWDG_GetReload(void);
void IWDG_ReloadCounter(void);
void IWDG_Enable(void);
void IWDG_OverflowConfig(uint32_t overflow_action);
void IWDG_ClearITPendingBit(void);

void IWDG_SetIGen(uint16_t GenValue);
uint16_t IWDG_GetCnt(void);

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


/**
 * @file    mg32_iwdg.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the IWDG firmware functions
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_iwdg.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup IWDG
  * @{
  */

/** @defgroup IWDG_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup IWDG_Private_Functions
  * @{
  */

/**
  * @brief  Enables or disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  write_access: new state of write access to IWDG_PR and IWDG_RLR registers.
  *   This parameter can be one of the following values:
  * @arg IWDG_WriteAccess_Enable: Enable write access to IWDG_PR and IWDG_RLR registers
  * @arg IWDG_WriteAccess_Disable: Disable write access to IWDG_PR and IWDG_RLR registers
  * @retval None.
  */
void IWDG_WriteAccessCmd(uint16_t write_access)
{
    IWDG->KR = write_access;
}

/**
  * @brief  Checks whether the specified IWDG flag is set or not.
  * @param  flag: specifies the flag to check.
  *   This parameter can be one of the following values:
  * @arg    IWDG_FLAG_PVU: Prescaler Value Update on going
  * @arg    IWDG_FLAG_RVU: reload Value Update on going
  * @arg    IWDG_FLAG_IVU: Interrupt Value Update on going
  * @retval The new state of flag (SET or RESET).
  */
FlagStatus IWDG_GetFlagStatus(uint32_t flag)
{
    return (((IWDG->SR & flag) != (uint32_t)RESET) ? SET : RESET);
}

/**
  * @brief  Checks prescaler value has been updated.
  * @param  None.
  * @retval None.
  */
void PVU_CheckStatus(void)
{
    while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) == SET)
    {
    }
}

/**
  * @brief  Checks count relead value has been updated.
  * @param  None.
  * @retval None.
  */
void RVU_CheckStatus(void)
{
    while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) == SET)
    {
    }
}

/**
  * @brief  Checks count Interrupt values value has been updated.
  * @param  None.
  * @retval None.
  */
void IVU_CheckStatus(void)
{
    while (IWDG_GetFlagStatus(IWDG_FLAG_IVU) == SET)
    {
    }
}

/**
  * @brief  Sets IWDG Prescaler value.
  * @param  prescaler: specifies the IWDG Prescaler value.
  *         This parameter can be one of the following values:
  * @arg    IWDG_Prescaler_4:   IWDG prescaler set to 4
  * @arg    IWDG_Prescaler_8:   IWDG prescaler set to 8
  * @arg    IWDG_Prescaler_16:  IWDG prescaler set to 16
  * @arg    IWDG_Prescaler_32:  IWDG prescaler set to 32
  * @arg    IWDG_Prescaler_64:  IWDG prescaler set to 64
  * @arg    IWDG_Prescaler_128: IWDG prescaler set to 128
  * @arg    IWDG_Prescaler_256: IWDG prescaler set to 256
  * @retval None.
  */
void IWDG_SetPrescaler(uint32_t prescaler)
{
    IWDG->PR = prescaler;
    PVU_CheckStatus();
}

/**
  * @brief  Set IWDG reload value.
  * @param  reload: specifies the IWDG reload value.
  *         This parameter must be a number between 0 and 0x0FFF.
  * @retval None.
  */
void IWDG_SetReload(uint16_t reload)
{
    IWDG->RLR = reload;
    RVU_CheckStatus();
}

/**
  * @brief  Get IWDG reload value.
  * @param  None.
  * @retval reload: specifies the IWDG reload value.
  */
uint32_t IWDG_GetReload(void)
{
    return (IWDG->RLR);
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  * @param  None.
  * @retval None.
  */
void IWDG_ReloadCounter(void)
{
    IWDG->KR = KR_KEY_Reload;
}

/**
  * @brief  Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @param  None.
  * @retval None.
  */
void IWDG_Enable(void)
{
    IWDG->KR = KR_KEY_Enable;
}

/**
  * @brief  IWDG overflow configuration.
  * @param  overflow_action
  * @arg    IWDG_Overflow_Interrupt: Interrupt after overflow.
  * @arg    IWDG_Overflow_Reset:     Reset after overflow.
  * @retval None.
  */
void IWDG_OverflowConfig(uint32_t overflow_action)
{
    MODIFY_REG(IWDG->CR, IWDG_CR_IRQ_SEL_Msk, overflow_action);
}

/**
  * @brief  Clear interrupt flag
  * @param  None.
  * @retval None.
  */
void IWDG_ClearITPendingBit(void)
{
    IWDG->CR |= (0x01U << IWDG_CR_IRQ_CLR_Pos);
}

/**
  * @brief  Set IWDG IGen value.
  * @param  GenValue: specifies the IWDG IGEN value.
  *         This parameter must be a number between 0 and 0x0FFF.
  * @retval None.
  */
void IWDG_SetIGen(uint16_t GenValue)
{
    IWDG->IGEN = GenValue;
    IVU_CheckStatus();
}

/**
  * @brief  Get IWDG Cnt Value. This Value is only read.
  * @param  None
  * @retval IWDG CNT value
  */
uint16_t IWDG_GetCnt(void)
{
    return (IWDG->CNT & IWDG_CNT_IWDG_CNT_Msk);
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


/**
 * @file    mg32_misc.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the MISC firmware functions
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_misc.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup NVIC
  * @{
  */

/** @defgroup NVIC_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup NVIC_Private_Functions
  * @{
  */

/**
  * @brief  NVIC initialization.
  * @param  init_struct
  * @retval None.
  */
void NVIC_Init(NVIC_InitTypeDef *init_struct)
{
    if (init_struct->NVIC_IRQChannelCmd != DISABLE)
    {
        NVIC->IP[init_struct->NVIC_IRQChannel >> 0x02] =
            (NVIC->IP[init_struct->NVIC_IRQChannel >> 0x02] &
             (~(((uint32_t)0xFF) << ((init_struct->NVIC_IRQChannel & 0x03) * 8)))) |
            ((((uint32_t)init_struct->NVIC_IRQChannelPriority << 6) & 0xFF) << ((init_struct->NVIC_IRQChannel & 0x03) * 8));

        NVIC->ISER[0] = 0x01 << (init_struct->NVIC_IRQChannel & 0x1F);
    }
    else
    {
        NVIC->ICER[0] = 0x01 << (init_struct->NVIC_IRQChannel & 0x1F);
    }
}

/**
  * @brief  SysTick clock source configuration.
  * @param  systick_clk_source
  *         This parameter can be any combination of the following values:
  *    @arg SYSTICK_CLKSOURCE_EXTCLK
  *    @arg SYSTICK_CLKSOURCE_HCLK
  * @retval None.
  */
void SysTick_CLKSourceConfig(uint32_t systick_clk_source)
{
    (systick_clk_source == SysTick_CLKSource_HCLK) ? \
    (SysTick->CTRL |= SysTick_CLKSource_HCLK) :      \
    (SysTick->CTRL &= ~SysTick_CLKSource_HCLK);
}

/**
  * @brief  System low power mode configuration.
  * @param  low_power_mode
  *         This parameter can be any combination of the following values:
  *    @arg NVIC_LP_SEVONPEND
  *    @arg NVIC_LP_SLEEPDEEP
  *    @arg NVIC_LP_SLEEPONEXIT
  * @param  state: new state of the low power mode.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void NVIC_SystemLPConfig(uint8_t low_power_mode, FunctionalState state)
{
    (state) ?                      \
    (SCB->SCR |= low_power_mode) : \
    (SCB->SCR &= ~(uint32_t)low_power_mode);
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


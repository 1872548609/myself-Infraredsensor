/**
 * @file    mg32_syscfg.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the SYSCFG firmware functions
 */

/* Includes ------------------------------------------------------------------*/
#include "mg32_syscfg.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @defgroup SYSCFG
  * @{
  */

/** @defgroup SYSCFG_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup SYSCFG_Private_Functions
  * @{
  */

/**
  * @brief  Deinitializes the SYSCFG registers to their default reset values.
  * @param  None
  * @retval None
  */
void SYSCFG_DeInit(void)
{
     /* Set SYSCFG_CFGR1 register to reset value without affecting MEM_MODE bits */
    SYSCFG->CFGR &= SYSCFG_CFGR_MEM_MODE_Msk;
     /* Set EXTICRx registers to reset value */
    SYSCFG->EXTICR1 = 0;
    SYSCFG->EXTICR2 = 0;
    SYSCFG->EXTICR3 = 0;
    SYSCFG->EXTICR4 = 0;
}

/**
  * @brief  Configures the memory mapping at address 0x00000000.
  * @param  SYSCFG_MemoryRemap: selects the memory remapping.
  *          This parameter can be one of the following values:
  *            @arg SYSCFG_MEMORYREMAP_FLASH : Main Flash memory mapped at 0x00000000
  *            @arg SYSCFG_MEMORYREMAP_SYSTEMMEMORY : System Flash memory mapped at 0x00000000
  *            @arg SYSCFG_MEMORYREMAP_SRAM : Embedded SRAM mapped at 0x00000000
  * @retval None
  */
void SYSCFG_MemoryRemapConfig(uint32_t SYSCFG_MemoryRemap)
{
    MODIFY_REG(SYSCFG->CFGR, SYSCFG_CFGR_MEM_MODE_Msk, SYSCFG_MemoryRemap);
}

/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  port_source_gpio: EXTI_PORTSOURCEGPIOx .
  * @param  pin_source: EXTI_PINSOURCEx.
  * @retval None.
  */
void SYSCFG_EXTILineConfig(uint8_t port_source_gpio, uint8_t pin_source)
{
    uint32_t tmp = 0x00;

    tmp = ((uint32_t)0x0F) << (0x04 * (pin_source & (uint8_t)0x03));

    if ((pin_source >> 0x02) == 0)
    {
        SYSCFG->EXTICR1 &= ~tmp;
        SYSCFG->EXTICR1 |= (((uint32_t)port_source_gpio) << (0x04 * (pin_source & (uint8_t)0x03)));
    }
    else if ((pin_source >> 0x02) == 1)
    {
        SYSCFG->EXTICR2 &= ~tmp;
        SYSCFG->EXTICR2 |= (((uint32_t)port_source_gpio) << (0x04 * (pin_source & (uint8_t)0x03)));
    }
    else if ((pin_source >> 0x02) == 2)
    {
        SYSCFG->EXTICR3 &= ~tmp;
        SYSCFG->EXTICR3 |= (((uint32_t)port_source_gpio) << (0x04 * (pin_source & (uint8_t)0x03)));
    }
    else if ((pin_source >> 0x02) == 3)
    {
        SYSCFG->EXTICR4 &= ~tmp;
        SYSCFG->EXTICR4 |= (((uint32_t)port_source_gpio) << (0x04 * (pin_source & (uint8_t)0x03)));
    }
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


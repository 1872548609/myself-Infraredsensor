/**
 * @file    mg32_exti.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   This file contains all the EXTI firmware functions
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32_exti.h"

/** @addtogroup MG32_StdPeriph_Driver
  * @{
  */

/** @addtogroup EXTI
  * @{
  */

/** @defgroup EXTI_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup EXTI_Private_Functions
  * @{
  */

/**
  * @brief  Deinitializes the EXTI peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void EXTI_DeInit(void)
{
    EXTI->IMR  = 0x00000000;
    EXTI->EMR  = 0x00000000;
    EXTI->RTSR = 0x00000000;
    EXTI->FTSR = 0x00000000;
    EXTI->PR   = 0x000FFFFF;
}

/**
  * @brief  Initializes the EXTI peripheral according to the specified
  *         parameters in the init_struct.
  * @param  init_struct: pointer to a EXTI_InitTypeDef structure that
  *         contains the configuration information for the EXTI peripheral.
  * @retval None.
  */
void EXTI_Init(EXTI_InitTypeDef *init_struct)
{
    if (init_struct->EXTI_LineCmd != DISABLE)
    {
        EXTI->IMR  &= ~init_struct->EXTI_Line;
        EXTI->EMR  &= ~init_struct->EXTI_Line;

        if (init_struct->EXTI_Mode == EXTI_Mode_Interrupt)
        {
            EXTI->IMR |= init_struct->EXTI_Line;
        }
        else
        {
            EXTI->EMR |= init_struct->EXTI_Line;
        }

        EXTI->RTSR &= ~init_struct->EXTI_Line;
        EXTI->FTSR &= ~init_struct->EXTI_Line;

        if (init_struct->EXTI_Trigger == EXTI_Trigger_Rising_Falling)
        {
            EXTI->RTSR |= init_struct->EXTI_Line;
            EXTI->FTSR |= init_struct->EXTI_Line; /* Rising and Faling    afio */
        }
        else if (init_struct->EXTI_Trigger == EXTI_Trigger_Rising)
        {
            EXTI->RTSR |= init_struct->EXTI_Line;
        }
        else
        {
            EXTI->FTSR |= init_struct->EXTI_Line;
        }
    }
    else
    {
        if (init_struct->EXTI_Mode == EXTI_Mode_Interrupt)
        {
            EXTI->IMR &= ~init_struct->EXTI_Line;
        }
        else
        {
            EXTI->EMR &= ~init_struct->EXTI_Line;
        }
    }
}

/**
  * @brief  Fills each init_struct member with its reset value.
  * @param  init_struct: pointer to a EXTI_InitTypeDef structure which will
  *         be initialized.
  * @retval None.
  */
void EXTI_StructInit(EXTI_InitTypeDef *init_struct)
{
    init_struct->EXTI_Line    = EXTI_LineNone;
    init_struct->EXTI_Mode    = EXTI_Mode_Interrupt;
    init_struct->EXTI_Trigger = EXTI_Trigger_Falling;
    init_struct->EXTI_LineCmd = DISABLE;
}

/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param  line: specifies the EXTI line on which the software interrupt
  *         will be generated.
  * @retval None.
  */
void EXTI_GenerateSWInterrupt(uint32_t line)
{
    EXTI->SWIER |= line;
}

/**
  * @brief  Checks whether the specified EXTI line flag is set or not.
  * @param  line: specifies the EXTI line flag to check.
  * @retval The new state of line (SET or RESET).
  */
FlagStatus EXTI_GetFlagStatus(uint32_t line)
{
    return ((EXTI->PR & line) ? SET : RESET);
}

/**
  * @brief  Clears the EXTI's line pending flags.
  * @param  line: specifies the EXTI lines flags to clear.
  * @retval None.
  */
void EXTI_ClearFlag(uint32_t line)
{
    EXTI->PR = line;
}

/**
  * @brief  Checks whether the specified EXTI line is asserted or not.
  * @param  line: specifies the EXTI line to check.
  * @retval The new state of line (SET or RESET).
  */
ITStatus EXTI_GetITStatus(uint32_t line)
{
    return (((EXTI->PR & line) && (EXTI->IMR & line)) ? SET : RESET);
}

/**
  * @brief  Clears the EXTI's line pending bits.
  * @param  line: specifies the EXTI lines to clear.
  * @retval None.
  */
void EXTI_ClearITPendingBit(uint32_t line)
{
    EXTI->PR = line;
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


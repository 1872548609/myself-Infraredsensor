/**
 * @file    iwdg_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _IWDG_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "iwdg_interrupt.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup IWDG
  * @{
  */

/**
  * @addtogroup IWDG_Interrupt
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void IWDG_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SYSCFG, ENABLE);

    RCC_LSICmd(ENABLE);

    while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSIRDY))
    {
    }

    PVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    RVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(0xFFF);

    IVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetIGen(0x500);

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_OverflowConfig(IWDG_Overflow_Interrupt);

    IWDG_ReloadCounter();

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line17;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = IWDG_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority  = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd  = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    IWDG_Enable();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void IWDG_Interrupt_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    IWDG_Configure();

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(100);
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


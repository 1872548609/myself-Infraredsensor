/**
 * @file    pwr_deepstop_iwdg_wakeup.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_DEEPSTOP_IWDG_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_deepstop_iwdg_wakeup.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_DeepStop_IWDG_Wakeup
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
void PWR_GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_All;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_IWDG_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

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
    IWDG_SetIGen(0x100);

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_OverflowConfig(IWDG_Overflow_Interrupt);

    IWDG_ReloadCounter();

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line17;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
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
void PWR_Pretreatment(void)
{
    printf("\r\n-->Entry Deep Stop Mode");

    PLATFORM_DelayMS(10);

    PWR_GPIO_Configure();

    PWR_IWDG_Configure();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Reinitialize(void)
{
    SystemInit();

    PLATFORM_Init();

    PLATFORM_DelayMS(100);

    printf("\r\n-->Wakeup By IWDG");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_DeepStop_IWDG_Wakeup_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s", __FUNCTION__);

    while (1)
    {
        printf("\r\n-->Running...");

        for (i = 0; i < 20; i++)
        {
            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(99);

            IWDG_ReloadCounter();
        }

        PWR_Pretreatment();

        /* DeepStop Mode */
        RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);
        PWR_EnterSTOPMode(PWR_Mode_DeepStop, PWR_StopEntry_WFI);

        PWR_Reinitialize();
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


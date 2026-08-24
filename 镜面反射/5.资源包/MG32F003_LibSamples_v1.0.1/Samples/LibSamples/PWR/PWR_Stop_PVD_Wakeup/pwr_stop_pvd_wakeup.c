/**
 * @file    pwr_stop_pvd_wakeup.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_STOP_PVD_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_stop_pvd_wakeup.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_Stop_PVD_Wakeup
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
void PWR_PVD_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);

    PWR_PVDLevelConfig(PWR_PVDLevel_3V3);   /* 3.3V */

    /* PVD Interrupt->EXTI_Line_16 */
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SYSCFG, ENABLE);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line16;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = PVD_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    PWR_PVDCmd(ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Pretreatment(void)
{
    printf("\r\n-->Entry Stop Mode");

    PLATFORM_DelayMS(10);

    PWR_GPIO_Configure();

    PWR_PVD_Configure();
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

    printf("\r\n-->Wakeup By PVD");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Stop_PVD_Wakeup_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s", __FUNCTION__);

    while (1)
    {
        printf("\r\n-->Running...");

        for (i = 0; i < 20; i++)
        {
            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(100);
        }

        PWR_Pretreatment();

        /* Stop Mode */
        RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);
        PWR_EnterSTOPMode(PWR_Mode_Stop, PWR_StopEntry_WFI);

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


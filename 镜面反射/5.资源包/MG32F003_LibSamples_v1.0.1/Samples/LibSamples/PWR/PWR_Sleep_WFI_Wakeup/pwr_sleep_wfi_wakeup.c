/**
 * @file    pwr_sleep_wfi_wakeup.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_SLEEP_WFI_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_sleep_wfi_wakeup.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_Sleep_WFI_Wakeup
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
void PWR_TIM_Configure(void)
{
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM1) / 10000 - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period            = (5000 - 1);
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Sleep_WFI_Wakeup_Sample(void)
{
    uint32_t Count = 0;

    printf("\r\nTest %s", __FUNCTION__);

    /* Disable SysTick and interrupt */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    PWR_TIM_Configure();

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);

        printf("\r\n%d", Count++);

        RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);

        PWR_EnterSLEEPMode(PWR_SLEEPNOW_WFI);
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


/**
 * @file    tim14_timebase.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM14_TIMEBASE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim14_timebase.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM14
  * @{
  */

/**
  * @addtogroup TIM14_TimeBase
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
void TIM14_Configure(void)
{
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM14, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM14) / 10000 - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period            = (5000 - 1);
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM14, TIM_FLAG_Update);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM14, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM14_TimeBase_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM14_UpdateFlag = 0;

    TIM14_Configure();

    while (1)
    {
        if (0 != TIM14_UpdateFlag)
        {
            TIM14_UpdateFlag = 0;

            PLATFORM_LED_Toggle(LED1);
        }
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


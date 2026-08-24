/**
 * @file    tim1_onepulseoutput.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM1_ONEPULSEOUTPUT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim1_onepulseoutput.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_OnePulseOutput
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
void TIM1_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2); /* TIM1_CH1 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM1) / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (1000 - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = (10 - 1);
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_SelectOnePulseMode(TIM1, TIM_OPMode_Single);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = (1000 - 1) / 2;

    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_Low;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_OnePulseOutput_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM1_Configure();

    while (1)
    {
        TIM_Cmd(TIM1, ENABLE);

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(200);
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


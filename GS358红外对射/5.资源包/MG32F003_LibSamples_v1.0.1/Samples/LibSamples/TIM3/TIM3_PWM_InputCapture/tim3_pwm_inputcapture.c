/**
 * @file    tim3_pwm_inputcapture.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM3_PWM_INPUTCAPTURE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim3_pwm_inputcapture.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_PWM_InputCapture
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
void TIM3_Configure(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_ICInitTypeDef       TIM_ICInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM1) / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = 0xFFFF;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);

    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_Div1;
    TIM_ICInitStruct.TIM_ICFilter    = 0x00;
    TIM_PWMIConfig(TIM3, &TIM_ICInitStruct);

    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);

    TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);   /* TIM3_CH1 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM3, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_PWM_InputCapture_Sample(void)
{
    float Frequency, Duty;

    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nInjection frequency range : [20Hz, 100kHz]");

    TIM3_Configure();

    while (1)
    {
        if (1 == TIM3_CC_InterruptFlag)
        {
            TIM3_CC_InterruptFlag = 0;

            Frequency = (float)1000000 / (float)TIM3_CC_Value1;

            Duty = (float)TIM3_CC_Value2 / (float)TIM3_CC_Value1 * (float)100;

            printf("\r\nTIM3:");

            if (Frequency >= 1000000.0)
            {
                printf("\r\nFrequency = %0.1fMHz, Duty = %0.1f%%", Frequency / (float)1000000, Duty);
            }
            else if (Frequency >= 1000.0)
            {
                printf("\r\nFrequency = %0.1fkHz, Duty = %0.1f%%", Frequency / (float)1000, Duty);
            }
            else
            {
                printf("\r\nFrequency = %0.1fHz,  Duty = %0.1f%%", Frequency, Duty);
            }
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


/**
 * @file    tim1_pwm_6stepoutput_deadtime_break.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM1_PWM_6STEPOUTPUT_DEADTIME_BREAK_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim1_pwm_6stepoutput_deadtime_break.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_PWM_6StepOutput_DeadTime_Break
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
    TIM_BDTRInitTypeDef     TIM_BDTRInitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = 0;
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = 10000 - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStruct.TIM_Pulse        = 0;
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Set;

    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC2Init(TIM1, &TIM_OCInitStruct);

    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC3Init(TIM1, &TIM_OCInitStruct);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_BDTRStructInit(&TIM_BDTRInitStruct);
    TIM_BDTRInitStruct.TIM_OSSRState       = TIM_OSSRState_Enable;
    TIM_BDTRInitStruct.TIM_OSSIState       = TIM_OSSIState_Enable;
    TIM_BDTRInitStruct.TIM_LOCKLevel       = TIM_LockLevel_OFF;
    TIM_BDTRInitStruct.TIM_DeadTime        = 1;
    TIM_BDTRInitStruct.TIM_Break           = TIM_Break_Enable;
    TIM_BDTRInitStruct.TIM_BreakPolarity   = TIM_BreakPolarity_High;
    TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_2); /* TIM1_BKIN */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_2);  /* TIM1_CH1  */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_1);  /* TIM1_CH2  */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_4);  /* TIM1_CH3  */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_1); /* TIM1_CH1N */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1); /* TIM1_CH2N */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_2); /* TIM1_CH3N */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_Cmd(TIM1, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_PWM_6StepOutput_DeadTime_Break_Sample(void)
{
    uint8_t Index   = 0;
    uint8_t Step[6] =
    {
        6, 2, 3, 1, 5, 4
    };

    printf("\r\nTest %s", __FUNCTION__);

    TIM1_Configure();

    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

    while (1)
    {
        switch (Step[Index])
        {
            case 6:
                TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);

                TIM_SetCompare2(TIM1, 5000 - 1);
                TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);

                TIM_SetCompare3(TIM1, 5000 - 1);
                TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
                break;

            case 2:
                TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

                TIM_SetCompare1(TIM1, 5000 - 1);
                TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);

                TIM_SetCompare2(TIM1, 5000 - 1);
                TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
                break;

            case 3:
                TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

                TIM_SetCompare1(TIM1, 5000 - 1);
                TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);

                TIM_SetCompare3(TIM1, 5000 - 1);
                TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
                break;

            case 1:
                TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

                TIM_SetCompare2(TIM1, 5000 - 1);
                TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);

                TIM_SetCompare3(TIM1, 5000 - 1);
                TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
                break;

            case 5:
                TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

                TIM_SetCompare1(TIM1, 5000 - 1);
                TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);

                TIM_SetCompare2(TIM1, 5000 - 1);
                TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
                break;

            case 4:
                TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

                TIM_SetCompare1(TIM1, 5000 - 1);
                TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);

                TIM_SetCompare3(TIM1, 5000 - 1);
                TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
                break;

            default:
                TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

                TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

                TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
                TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
                break;
        }

        Index = (Index + 1) % 6;

        PLATFORM_DelayMS(50);
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


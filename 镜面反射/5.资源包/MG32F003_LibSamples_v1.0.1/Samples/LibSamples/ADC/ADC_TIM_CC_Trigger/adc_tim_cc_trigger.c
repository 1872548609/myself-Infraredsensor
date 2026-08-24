/**
 * @file    adc_tim_cc_trigger.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_TIM_CC_TRIGGER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_tim_cc_trigger.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_TIM_CC_Trigger
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
void ADC_Configure(void)
{
    ADC_InitTypeDef         ADC_InitStruct;
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution       = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler        = ADC_Prescaler_16;
    ADC_InitStruct.ADC_Mode             = ADC_Mode_Scan;
    ADC_InitStruct.ADC_DataAlign        = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExtTrig_T1_CC1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

    ADC_AnyChannelNumCfg(ADC1, 2);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_0, ADC_Channel_3);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_1, ADC_Channel_1);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_2, ADC_Channel_7);
    ADC_AnyChannelCmd(ADC1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOB, ENABLE);

    /* PA12(RV1) PB0(RV2) PA7(RV3) */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM1, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM1) / 2000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (2000 - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStruct);

    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode          = TIM_OCMode_Timing;
    TIM_OCInitStruct.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse           = (2000 / 2 - 1);
    TIM_OCInitStruct.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStruct);

    TIM_SetCompare2(TIM1, 2000);
    TIM_SetCompare3(TIM1, 2000);
    TIM_SetCompare4(TIM1, 2000);
    TIM_SetCompare5(TIM1, 2000);

    TIM_Cmd(TIM1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_TIM_CC_Trigger_Sample(void)
{
    float RVxVoltage[3];

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    while (1)
    {
        if (SET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        {
            ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

            RVxVoltage[0] = (float)ADC_GetChannelConvertedValue(ADC1, ADC_Channel_3) * (float)3.3 / (float)4096.0;
            RVxVoltage[1] = (float)ADC_GetChannelConvertedValue(ADC1, ADC_Channel_1) * (float)3.3 / (float)4096.0;
            RVxVoltage[2] = (float)ADC_GetChannelConvertedValue(ADC1, ADC_Channel_7) * (float)3.3 / (float)4096.0;

            printf("\r\nRV1 Voltage = %0.2f  \tRV2 Voltage = %0.2f  \tRV3 Voltage = %0.2f\t", RVxVoltage[0], RVxVoltage[1], RVxVoltage[2]);
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


/**
 * @file    adc_analogwatchdog.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_ANALOGWATCHDOG_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_analogwatchdog.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_AnalogWatchdog
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
    ADC_InitTypeDef  ADC_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution       = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler        = ADC_Prescaler_16;
    ADC_InitStruct.ADC_Mode             = ADC_Mode_Continue;
    ADC_InitStruct.ADC_DataAlign        = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_0, ADC_Channel_3);
    ADC_AnyChannelCmd(ADC1, ENABLE);

    ADC_AnalogWatchdogCmd(ADC1, ENABLE);
    ADC_AnalogWatchdogThresholdsConfig(ADC1, 3000, 1000);
    ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_3);

    ADC_ClearFlag(ADC1, ADC_FLAG_AWD);
    ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = ADC1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    ADC_Cmd(ADC1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_AnalogWatchdog_Sample(void)
{
    uint16_t Value = 0;

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    printf("\r\nTurn around RV1...");

    while (1)
    {
        if (0 != ADC_InterruptFlag)
        {
            ADC_InterruptFlag = 0;

            while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
            {
            }

            ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

            Value = ADC_GetConversionValue(ADC1);

            printf("\r\nADC : %d, Voltage : %0.1f", Value, (float)Value / (float)4095 * (float)3.3);
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


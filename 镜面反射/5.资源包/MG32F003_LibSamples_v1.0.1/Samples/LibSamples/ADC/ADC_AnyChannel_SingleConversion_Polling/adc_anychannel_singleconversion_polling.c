/**
 * @file    adc_anychannel_singleconversion_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_ANYCHANNEL_SINGLECONVERSION_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_anychannel_singleconversion_polling.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_AnyChannel_SingleConversion_Polling
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

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution       = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler        = ADC_Prescaler_16;
    ADC_InitStruct.ADC_Mode             = ADC_Mode_Imm;
    ADC_InitStruct.ADC_DataAlign        = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

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

    ADC_Cmd(ADC1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
float ADC_GetChannelVoltage(uint8_t Channel)
{
    uint16_t Value = 0;
    float Voltage;

    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_0, Channel);
    ADC_AnyChannelCmd(ADC1, ENABLE);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
    {
    }

    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

    Value = ADC_GetConversionValue(ADC1);

    ADC_AnyChannelCmd(ADC1, DISABLE);

    Voltage = (float)Value / (float)4096 * (float)3.3;

    return (Voltage);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_AnyChannel_SingleConversion_Polling_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    while (1)
    {
        printf("\r\nRV1 Voltage = %0.2f  \tRV2 Voltage = %0.2f  \tRV3 Voltage = %0.2f",
               ADC_GetChannelVoltage(ADC_Channel_3),
               ADC_GetChannelVoltage(ADC_Channel_1),
               ADC_GetChannelVoltage(ADC_Channel_7));

        PLATFORM_DelayMS(500);
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


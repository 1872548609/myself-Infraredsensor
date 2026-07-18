/**
 * @file    adc_internalvoltagesensor.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_INTERNALVOLTAGESENSOR_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_internalvoltagesensor.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_InternalVoltageSensor
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
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution       = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler        = ADC_Prescaler_16;
    ADC_InitStruct.ADC_Mode             = ADC_Mode_Imm;
    ADC_InitStruct.ADC_DataAlign        = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_0, ADC_Channel_VoltSensor);
    ADC_AnyChannelCmd(ADC1, ENABLE);

    ADC_VrefSensorCmd(ENABLE);

    ADC_Cmd(ADC1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_InternalVoltageSensor_Sample(void)
{
    uint16_t ConversionValue = 0;
    uint16_t CalibrationData = *(uint16_t *)(0x1FFFF7E0);
    float    VrefCalculation = (float)CalibrationData * (float)3.3 / (float)4096.0;

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    while (1)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        {
        }

        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

        ConversionValue = ADC_GetConversionValue(ADC1);

        printf("\r\nVDDA = %0.2fV", (float)4096.0 * (float)VrefCalculation / (float)ConversionValue);

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


/**
 * @file    pwr_pvd_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_PVD_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_pvd_polling.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_PVD_Polling
  * @{
  */

/* Private typedef ****************************************************************************************************/
typedef struct
{
    float    Voltage;
    uint32_t Level;
} PVD_MAP_TypeDef;

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/
PVD_MAP_TypeDef PVD_MapTable[11] =
{
    { 1.8, PWR_PVDLevel_1V8 },      /* 0:  1.8V */
    { 2.1, PWR_PVDLevel_2V1 },      /* 1:  2.1V */
    { 2.4, PWR_PVDLevel_2V4 },      /* 2:  2.4V */
    { 2.7, PWR_PVDLevel_2V7 },      /* 3:  2.7V */
    { 3.0, PWR_PVDLevel_3V0 },      /* 4:  3.0V */
    { 3.3, PWR_PVDLevel_3V3 },      /* 5:  3.3V */
    { 3.6, PWR_PVDLevel_3V6 },      /* 6:  3.6V */
    { 3.9, PWR_PVDLevel_3V9 },      /* 7:  3.9V */
    { 4.2, PWR_PVDLevel_4V2 },      /* 8:  4.2V */
    { 4.5, PWR_PVDLevel_4V5 },      /* 9:  4.5V */
    { 4.8, PWR_PVDLevel_4V8 },      /* 10: 4.8V */
};

uint8_t PVD_LevelSelection = 0;

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Configure(void)
{
    ADC_InitTypeDef ADC_InitStruct;

    /* For measure VDD_M voltage */
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_ADC1, ENABLE);

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_Prescaler   = ADC_Prescaler_16;
    ADC_InitStruct.ADC_Mode       = ADC_Mode_Continue;
    ADC_InitStruct.ADC_DataAlign  = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

    ADC_AnyChannelNumCfg(ADC1, 0);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_0, ADC_Channel_VoltSensor);
    ADC_AnyChannelCmd(ADC1, ENABLE);

    ADC_VrefSensorCmd(ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    /* PVD */
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWR, ENABLE);

    PWR_PVDLevelConfig(PVD_MapTable[PVD_LevelSelection].Level);

    PWR_PVDCmd(ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_PVD_Polling_Sample(void)
{
    static uint8_t PVD_Count  = 0;
    static uint8_t PVD_State  = 0;

    uint16_t ConversionResult = 0;
    uint16_t CalibrationValue = *((uint16_t *)(0x1FFFF7E0));

    float VDD_M_Voltage;

    printf("\r\nTest %s", __FUNCTION__);

    PVD_LevelSelection = 5;

    printf("\r\nPVD Level(%d) Selection : %0.1fV", PVD_LevelSelection, PVD_MapTable[PVD_LevelSelection].Voltage);

    PWR_Configure();

    while (1)
    {
        /* Read VDD_M voltage */
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
        {
        }

        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

        ConversionResult = ADC_GetConversionValue(ADC1);

        VDD_M_Voltage = (float)3.3 * (float)CalibrationValue / (float)ConversionResult;

        /* PVD detection */
        if (RESET != PWR_GetPVDOFlagStatus(PWR_FLAG_PVDO))
        {
            if (1 != PVD_State)
            {
                PVD_State = 1;
                printf("\r\n%03d : VDD/VDDA(%0.1fV) <= PVD(%0.1fV)", PVD_Count++, VDD_M_Voltage, PVD_MapTable[PVD_LevelSelection].Voltage);
            }
        }
        else
        {
            if (2 != PVD_State)
            {
                PVD_State = 2;
                printf("\r\n%03d : VDD/VDDA(%0.1fV) >= PVD(%0.1fV)", PVD_Count++, VDD_M_Voltage, PVD_MapTable[PVD_LevelSelection].Voltage);
            }
        }

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


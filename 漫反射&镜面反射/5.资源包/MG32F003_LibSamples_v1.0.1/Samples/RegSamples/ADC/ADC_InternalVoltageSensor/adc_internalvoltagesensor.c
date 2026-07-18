/**
 * @file    adc_internalvoltagesensor.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_INTERNALVOLTAGESENSOR_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_internalvoltagesensor.h"

/**
  * @addtogroup MG32F003_RegSamples
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
    /* Enable ADC Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_ADC1);

    /* ADC Prescaler : ADC Clock Division = (ADCPRE + 2) = 16 */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_ADCPREH, 0x07U << ADC_ADCFG_ADCPREH_Pos);
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_ADCPREL, 0x00U << ADC_ADCFG_ADCPREL_Pos);

    /* ADC Conversion Data Resolution Selection : 12-bit Effective */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_RSLTCTL, 0x00U << ADC_ADCFG_RSLTCTL_Pos);

    /* Data Alignment : Right */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_ALIGN, 0x00U << ADC_ADCR_ALIGN_Pos);

    /* ADC Conversion Mode : Single Conversion */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_ADMD, 0x00U << ADC_ADCR_ADMD_Pos);

    /* Channel Sample Time Selection : 240.5 Cycles */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_SAMCTL, 0x07U << ADC_ADCFG_SAMCTL_Pos);

    /* Number Of Any Channel Mode */
    MODIFY_REG(ADC1->ANY_CFG, ADC_ANY_CFG_CHANY_NUM, 0x00U << ADC_ANY_CFG_CHANY_NUM_Pos);

    /* Arbitrary Channel Selection */
    MODIFY_REG(ADC1->CHANY0, ADC_CHANY0_CHANY_SEL0, 0x08U << ADC_CHANY0_CHANY_SEL0_Pos);

    /* Arbitrary Channel Configuration Mode Enable */
    SET_BIT(ADC1->ANY_CR, ADC_ANY_CR_CHANY_MDEN);

    /* Voltage Sensor Enable */
    SET_BIT(ADC1->ADCFG, ADC_ADCFG_VSEN);

    /* ADC Conversion Enable */
    SET_BIT(ADC1->ADCFG, ADC_ADCFG_ADEN);
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
        /* ADC Conversion Start */
        SET_BIT(ADC1->ADCR, ADC_ADCR_ADST);

        while (READ_BIT(ADC1->ADSTA, ADC_ADSTA_ADIF) == 0)
        {
        }

        /* Clear ADC Conversion Completed Flag */
        SET_BIT(ADC1->ADSTA, ADC_ADSTA_ADIF);

        /* Read ADC Conversion Result */
        ConversionValue = (READ_REG(ADC1->ADDATA) & ADC_ADDATA_DATA_Msk) >> ADC_ADDATA_DATA_Pos;

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


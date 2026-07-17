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
    /* Enable ADC Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_ADC1);

    /* ADC Prescaler : ADC Clock Division = (ADCPRE + 2) = 16 */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_ADCPREH, 0x07U << ADC_ADCFG_ADCPREH_Pos);
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_ADCPREL, 0x00U << ADC_ADCFG_ADCPREL_Pos);

    /* ADC Conversion Data Resolution Selection : 12-bit Effective */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_RSLTCTL, 0x00U << ADC_ADCFG_RSLTCTL_Pos);

    /* Data Alignment : Right */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_ALIGN, 0x00U << ADC_ADCR_ALIGN_Pos);

    /* ADC Conversion Mode : Continuous Conversion */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_ADMD, 0x02U << ADC_ADCR_ADMD_Pos);

    /* Channel Sample Time Selection : 240.5 Cycles */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_SAMCTL, 0x07U << ADC_ADCFG_SAMCTL_Pos);

    /* Number Of Any Channel Mode */
    MODIFY_REG(ADC1->ANY_CFG, ADC_ANY_CFG_CHANY_NUM, 0x00U << ADC_ANY_CFG_CHANY_NUM_Pos);

    /* Arbitrary Channel Selection */
    MODIFY_REG(ADC1->CHANY0, ADC_CHANY0_CHANY_SEL0, 0x03U << ADC_CHANY0_CHANY_SEL0_Pos);

    /* Arbitrary Channel Configuration Mode Enable */
    SET_BIT(ADC1->ANY_CR, ADC_ANY_CR_CHANY_MDEN);

    /* Enables analog watchdog */
    SET_BIT(ADC1->ADCFG, ADC_ADCFG_ADWEN);

    /* ADC analog watchdog High threshold value */
    MODIFY_REG(ADC1->ADCMPR, ADC_ADCMPR_CMPHDATA_Msk, (3000 << ADC_ADCMPR_CMPHDATA_Pos));
    /* ADC analog watchdog Low threshold value */
    MODIFY_REG(ADC1->ADCMPR, ADC_ADCMPR_CMPLDATA_Msk, (1000 << ADC_ADCMPR_CMPLDATA_Pos));

    /* The ADC channel to configure for the analog watchdog */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_CMPCH_Msk, 0x03 << ADC_ADCR_CMPCH_Pos);

    /* ADC Window Comparator Interrupt Enable */
    SET_BIT(ADC1->ADCR, ADC_ADCR_AWDIE);

    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config RV1(PA12) Analog Input Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE12, 0x00U << GPIO_CRH_MODE12_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF12,  0x00U << GPIO_CRH_CNF12_Pos);

    /* ADC Conversion Enable */
    SET_BIT(ADC1->ADCFG, ADC_ADCFG_ADEN);
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

    /* ADC Conversion Start */
    SET_BIT(ADC1->ADCR, ADC_ADCR_ADST);

    printf("\r\nTurn around RV1...");

    while (1)
    {
        if (0 != ADC_InterruptFlag)
        {
            ADC_InterruptFlag = 0;

            while (RESET == READ_BIT(ADC1->ADSTA, ADC_ADSTA_ADIF))
            {
            }

            SET_BIT(ADC1->ADSTA, ADC_ADSTA_ADIF);

            Value = ((READ_REG(ADC1->ADDR3) & ADC_ADDR_DATA) >> ADC_ADDR_DATA_Pos) ;

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


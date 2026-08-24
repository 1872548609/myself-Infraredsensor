/**
 * @file    adc_anychannel_singleconversion_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_ANYCHANNEL_SINGLECONVERSION_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_anychannel_singleconversion_interrupt.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_AnyChannel_SingleConversion_Interrupt
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

    /* ADC Interrupt Enable */
    SET_BIT(ADC1->ADCR, ADC_ADCR_ADIE);

    NVIC_SetPriority(ADC1_IRQn, 1);
    NVIC_EnableIRQ(ADC1_IRQn);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Enable GPIOB Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOB);

    /* Config RV1(PA12) Analog Input Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE12, 0x00U << GPIO_CRH_MODE12_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF12,  0x00U << GPIO_CRH_CNF12_Pos);

    /* Config RV2(PB0) Analog Input Mode */
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE0,  0x00U << GPIO_CRL_MODE0_Pos);
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF0,   0x00U << GPIO_CRL_CNF0_Pos);

    /* Config RV3(PA7) Analog Input Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE7,  0x00U << GPIO_CRL_MODE7_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF7,   0x00U << GPIO_CRL_CNF7_Pos);

    /* ADC Conversion Enable */
    SET_BIT(ADC1->ADCFG, ADC_ADCFG_ADEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
float ADC_GetChannelVoltage(uint32_t Channel)
{
    uint16_t Value = 0;
    float    Voltage;

    /* Number Of Any Channel Mode */
    MODIFY_REG(ADC1->ANY_CFG, ADC_ANY_CFG_CHANY_NUM, 0x00U << ADC_ANY_CFG_CHANY_NUM_Pos);

    /* Arbitrary Channel Selection */
    MODIFY_REG(ADC1->CHANY0, ADC_CHANY0_CHANY_SEL0, Channel << ADC_CHANY0_CHANY_SEL0_Pos);

    /* Arbitrary Channel Configuration Mode Enable */
    SET_BIT(ADC1->ANY_CR, ADC_ANY_CR_CHANY_MDEN);

    ADC_InterruptFlag = 0;

    /* ADC Conversion Start */
    SET_BIT(ADC1->ADCR, ADC_ADCR_ADST);

    while (0 == ADC_InterruptFlag)
    {
    }

    /* Read ADC Conversion Result */
    Value = (READ_REG(ADC1->ADDATA) & ADC_ADDATA_DATA_Msk) >> ADC_ADDATA_DATA_Pos;

    /* Arbitrary Channel Configuration Mode Disable */
    CLEAR_BIT(ADC1->ANY_CR, ADC_ANY_CR_CHANY_MDEN);

    Voltage = (float)Value / (float)4096 * (float)3.3;

    return (Voltage);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_AnyChannel_SingleConversion_Interrupt_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    while (1)
    {
        printf("\r\nRV1 Voltage = %0.2f  \tRV2 Voltage = %0.2f  \tRV3 Voltage = %0.2f",
               ADC_GetChannelVoltage(3),
               ADC_GetChannelVoltage(1),
               ADC_GetChannelVoltage(7));

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


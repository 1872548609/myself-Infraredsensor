/**
 * @file    pwr_pvd_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_PVD_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_pvd_interrupt.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_PVD_Interrupt
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
void PWR_Configure(void)
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

    /* Enable PWR Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWR);

    /* PVD Level Selection : 3.3V */
    MODIFY_REG(PWR->CR, PWR_CR_PLS, 0x05U << PWR_CR_PLS_Pos);

    /* Enable EXTI Line16 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR16);
    /* EXTI Line16 Rising Trigger */
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR16);
    /* EXTI Line16 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR16);

    NVIC_SetPriority(PVD_IRQn, 1);
    NVIC_EnableIRQ(PVD_IRQn);

    /* Enable PVD */
    SET_BIT(PWR->CR, PWR_CR_PVDE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_PVD_Interrupt_Sample(void)
{
    uint8_t PVD_Count  = 0;

    uint16_t ConversionResult = 0;
    uint16_t CalibrationValue = *((uint16_t *)(0x1FFFF7E0));

    float VDD_M_Voltage;

    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nPVD Level(5) Selection : 3.3V");

    PWR_Configure();

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
        ConversionResult = (READ_REG(ADC1->ADDATA) & ADC_ADDATA_DATA_Msk) >> ADC_ADDATA_DATA_Pos;

        VDD_M_Voltage = (float)3.3 * (float)CalibrationValue / (float)ConversionResult;

        /* PVD detection */
        if (0 != PWR_PVD_InterruptFlag)
        {
            PWR_PVD_InterruptFlag = 0;

            if (READ_BIT(PWR->CSR, PWR_CSR_PVDO))
            {
                printf("\r\n%03d : VDD/VDDA(%0.1fV) <= PVD(3.3V)", PVD_Count++, VDD_M_Voltage);
            }
            else
            {
                printf("\r\n%03d : VDD/VDDA(%0.1fV) >= PVD(3.3V)", PVD_Count++, VDD_M_Voltage);
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


/**
 * @file    adc_exti_trigger.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _ADC_EXTI_TRIGGER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "adc_exti_trigger.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup ADC
  * @{
  */

/**
  * @addtogroup ADC_EXTI_Trigger
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

    /* ADC Conversion Mode : Single-Cycle Conversion */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_ADMD, 0x01U << ADC_ADCR_ADMD_Pos);

    /* Channel Sample Time Selection : 240.5 Cycles */
    MODIFY_REG(ADC1->ADCFG, ADC_ADCFG_SAMCTL, 0x07U << ADC_ADCFG_SAMCTL_Pos);

    /* Number Of Any Channel Mode */
    MODIFY_REG(ADC1->ANY_CFG, ADC_ANY_CFG_CHANY_NUM, 0x02U << ADC_ANY_CFG_CHANY_NUM_Pos);

    /* Arbitrary Channel Selection */
    MODIFY_REG(ADC1->CHANY0, ADC_CHANY0_CHANY_SEL0, 0x03U << ADC_CHANY0_CHANY_SEL0_Pos);
    MODIFY_REG(ADC1->CHANY0, ADC_CHANY0_CHANY_SEL1, 0x01U << ADC_CHANY0_CHANY_SEL1_Pos);
    MODIFY_REG(ADC1->CHANY0, ADC_CHANY0_CHANY_SEL2, 0x07U << ADC_CHANY0_CHANY_SEL2_Pos);

    /* Arbitrary Channel Configuration Mode Enable */
    SET_BIT(ADC1->ANY_CR, ADC_ANY_CR_CHANY_MDEN);

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

    /* Enable SYSCFG Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SYSCFG);

    /* Config PA11 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE11, 0x00U << GPIO_CRH_MODE11_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF11,  0x02U << GPIO_CRH_CNF11_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS11);

    /* EXTI Line11 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR3, SYSCFG_EXTICR3_EXTI11, 0x00U << SYSCFG_EXTICR3_EXTI11_Pos);
    /* Enable EXTI Line11 Event */
    SET_BIT(EXTI->EMR, EXTI_EMR_EMR11);
    /* EXTI Line11 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR11);

    /* External Trigger Selection : EXTI Line11 */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_TRGSELH, 0x00U << ADC_ADCR_TRGSELH_Pos);
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_TRGSELL, 0x07U << ADC_ADCR_TRGSELL_Pos);

    /* Trigger Sources Edge Selection : Falling Edge Trigger */
    MODIFY_REG(ADC1->ADCR, ADC_ADCR_TRG_EDGE, 0x01U << ADC_ADCR_TRG_EDGE_Pos);

    /* Enable ADC Conversion By External Trigger Signal */
    SET_BIT(ADC1->ADCR, ADC_ADCR_TRGEN);

    /* ADC Conversion Enable */
    SET_BIT(ADC1->ADCFG, ADC_ADCFG_ADEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void ADC_EXTI_Trigger_Sample(void)
{
    float RVxVoltage[3];

    printf("\r\nTest %s", __FUNCTION__);

    ADC_Configure();

    printf("\r\nShort PA11 and PA3, press K4 to trigger adc conversion!");

    while (1)
    {
        if (READ_BIT(EXTI->PR, EXTI_PR_PR11))
        {
            /* Clear EXTI Line11 Pending Flag */
            SET_BIT(EXTI->PR, EXTI_PR_PR11);
        }

        if (READ_BIT(ADC1->ADSTA, ADC_ADSTA_ADIF))
        {
            /* Clear ADC Conversion Completed Flag */
            SET_BIT(ADC1->ADSTA, ADC_ADSTA_ADIF);

            RVxVoltage[0] = (float)((READ_REG(ADC1->ADDR3) & ADC_ADDR_DATA) >> ADC_ADDR_DATA_Pos) * (float)3.3 / (float)4096.0;
            RVxVoltage[1] = (float)((READ_REG(ADC1->ADDR1) & ADC_ADDR_DATA) >> ADC_ADDR_DATA_Pos) * (float)3.3 / (float)4096.0;
            RVxVoltage[2] = (float)((READ_REG(ADC1->ADDR7) & ADC_ADDR_DATA) >> ADC_ADDR_DATA_Pos) * (float)3.3 / (float)4096.0;

            printf("\r\nRV1 Voltage = %0.2f  \tRV2 Voltage = %0.2f  \tRV3 Voltage = %0.2f", RVxVoltage[0], RVxVoltage[1], RVxVoltage[2]);
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


/**
 * @file    tim3_pwm_inputcapture.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM3_PWM_INPUTCAPTURE_C_

/* Files include */
#include <math.h>
#include <stdio.h>
#include "platform.h"
#include "tim3_pwm_inputcapture.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_PWM_InputCapture
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
uint32_t TIM3_GetFrequency(void)
{
    uint32_t TIM3_Frequency     = 0;
    uint32_t PPRE1 = 0, APB_DIV = 0;

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        APB_DIV = 1;

        TIM3_Frequency = PLATFORM_GetPCLK1Frequency();
    }
    else
    {
        APB_DIV = pow(2, PPRE1 % 3);

        TIM3_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / APB_DIV;
    }

    return (TIM3_Frequency);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_Configure(void)
{
    /* Enable TIM3 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3);

    /* Config TIM3 Clock Division : DIV1 */
    MODIFY_REG(TIM3->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM3 Edge Alignment Mode */
    MODIFY_REG(TIM3->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM3 Up Count Direction */
    MODIFY_REG(TIM3->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM3 Auto-Reload Value */
    WRITE_REG(TIM3->ARR, UINT16_MAX);

    /* Config TIM3 Prescaler Value */
    WRITE_REG(TIM3->PSC, TIM3_GetFrequency() / 1000000 - 1);

    /* Channel 1 Is Configured As Input, IC1 Is Mapped On TI1 */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_CC1S, 0x01U << TIM_CCMR1_CC1S_Pos);

    /* Channel 1 Input Capture Filter */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_IC1F, 0x00U << TIM_CCMR1_IC1F_Pos);

    /* IC1 Is Active At High Level */
    CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1P);

    /* Capture Enable */
    SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

    /* Channel 1 Input/Capture Prescalar */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_IC1PSC, 0x00U << TIM_CCMR1_IC1PSC_Pos);

    /* Channel 2 Is Configured As Input, IC1 Is Mapped On TI1 */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_CC2S, 0x02U << TIM_CCMR1_CC2S_Pos);

    /* Channel 2 Input Capture Filter */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_IC2F, 0x00U << TIM_CCMR1_IC2F_Pos);

    /* IC2 Is Active At Low Level */
    SET_BIT(TIM3->CCER, TIM_CCER_CC2P);

    /* Capture Enable */
    SET_BIT(TIM3->CCER, TIM_CCER_CC2E);

    /* Channel 2 Input/Capture Prescalar */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_IC2PSC, 0x00U << TIM_CCMR1_IC2PSC_Pos);

    /* Slave Mode Selection : Reset Mode */
    MODIFY_REG(TIM3->SMCR, TIM_SMCR_SMS, 0x04U << TIM_SMCR_SMS_Pos);

    /* Timer Input After Filter1 */
    MODIFY_REG(TIM3->SMCR, TIM_SMCR_TS, 0x05U << TIM_SMCR_TS_Pos);

    /* Enable Capture/Compare 1 Interrupt */
    SET_BIT(TIM3->DIER, TIM_DIER_CC1IE);

    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA11 AF4 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR11, 0x04U << GPIO_AFRH_AFR11_Pos); /* TIM3_CH1 */

    /* Config PA11 Input Floating Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE11, 0x00U << GPIO_CRH_MODE11_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF11,  0x01U << GPIO_CRH_CNF11_Pos);

    /* Enable TIM3 */
    SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_PWM_InputCapture_Sample(void)
{
    float Frequency, Duty;

    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nInjection frequency range : [20Hz, 100kHz]");

    TIM3_Configure();

    while (1)
    {
        if (1 == TIM3_CC_InterruptFlag)
        {
            TIM3_CC_InterruptFlag = 0;

            Frequency = (float)1000000 / (float)TIM3_CC_Value1;

            Duty = (float)TIM3_CC_Value2 / (float)TIM3_CC_Value1 * (float)100;

            printf("\r\nTIM3:");

            if (Frequency >= 1000000.0)
            {
                printf("\r\nFrequency = %0.1fMHz, Duty = %0.1f%%", Frequency / (float)1000000, Duty);
            }
            else if (Frequency >= 1000.0)
            {
                printf("\r\nFrequency = %0.1fkHz, Duty = %0.1f%%", Frequency / (float)1000, Duty);
            }
            else
            {
                printf("\r\nFrequency = %0.1fHz,  Duty = %0.1f%%", Frequency, Duty);
            }
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


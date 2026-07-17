/**
 * @file    tim1_onepulseoutput.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM1_ONEPULSEOUTPUT_C_

/* Files include */
#include <math.h>
#include <stdio.h>
#include "platform.h"
#include "tim1_onepulseoutput.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_OnePulseOutput
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
uint32_t TIM1_GetFrequency(void)
{
    uint32_t TIM1_Frequency       = 0;
    uint32_t HPRE    = 0, PPRE1   = 0;
    uint32_t AHB_DIV = 0, APB_DIV = 0;

    HPRE = READ_BIT(RCC->CFGR, RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;

    if (HPRE < 8)
    {
        AHB_DIV = 1;
    }
    else
    {
        AHB_DIV = pow(2, HPRE % 7);
    }

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        APB_DIV = 1;
    }
    else
    {
        APB_DIV = pow(2, PPRE1 % 3);
    }

    if ((AHB_DIV == 1) && (APB_DIV == 1))
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency();
    }
    else if ((AHB_DIV != 1) && (APB_DIV == 1))
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / AHB_DIV;
    }
    else if ((AHB_DIV == 1) && (APB_DIV != 1))
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / APB_DIV;
    }
    else
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency() * 4 / AHB_DIV / APB_DIV;
    }

    return (TIM1_Frequency);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_Configure(void)
{
    /* Enable TIM1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM1);

    /* Config TIM1 Clock Division : DIV1 */
    MODIFY_REG(TIM1->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM1 Edge Alignment Mode */
    MODIFY_REG(TIM1->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM1 Up Count Direction */
    MODIFY_REG(TIM1->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM1 Auto-Reload Value */
    WRITE_REG(TIM1->ARR, 1000 - 1);

    /* Config TIM1 Prescaler Value */
    WRITE_REG(TIM1->PSC, TIM1_GetFrequency() / 1000000 - 1);

    /* Config TIM1 Repeat Count */
    MODIFY_REG(TIM1->RCR, TIM_RCR_REP, (10 - 1) << TIM_RCR_REP_Pos);

    /* Enable Auto Reload Preload */
    SET_BIT(TIM1->CR1, TIM_CR1_ARPE);

    /* Enable One-Plus Mode */
    SET_BIT(TIM1->CR1, TIM_CR1_OPM);


    /* Config TIM1 Channel 1 PWM Mode 1 */
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC1M, 0x06U << TIM_CCMR1_OC1M_Pos);

    /* Enable TIM1 Channel 1 Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC1E);

    /* Config TIM1 Channel 1 Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1P);

    /* Config TIM1 Channel 1 Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS1);

    /* Config TIM1 Channel 1 Compare Value */
    WRITE_REG(TIM1->CCR1, 500 - 1);


    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA9 AF2 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR9, 0x02U << GPIO_AFRH_AFR9_Pos);   /* TIM1_CH1 */

    /* Config PA9 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE9, 0x01U << GPIO_CRH_MODE9_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9,  0x02U << GPIO_CRH_CNF9_Pos);

    /* Enable PWM Output */
    SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_OnePulseOutput_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM1_Configure();

    while (1)
    {
        /* Enable TIM1 */
        SET_BIT(TIM1->CR1, TIM_CR1_CEN);

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(100);
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


/**
 * @file    tim1_pwm_6stepoutput_deadtime_break.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM1_PWM_6STEPOUTPUT_DEADTIME_BREAK_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim1_pwm_6stepoutput_deadtime_break.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_PWM_6StepOutput_DeadTime_Break
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
    WRITE_REG(TIM1->ARR, 10000 - 1);

    /* Config TIM1 Prescaler Value */
    WRITE_REG(TIM1->PSC, 0);


    /* Config TIM1 Channel 1 PWM Mode 1 */
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC1M, 0x06U << TIM_CCMR1_OC1M_Pos);

    /* Enable TIM1 Channel 1 Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC1E);

    /* Config TIM1 Channel 1 Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1P);

    /* Config TIM1 Channel 1 Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS1);

    /* Enable TIM1 Channel 1N Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC1NE);

    /* Config TIM1 Channel 1N Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1NP);

    /* Config TIM1 Channel 1N Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS1N);

    /* Config TIM1 Channel 1 Compare Value */
    WRITE_REG(TIM1->CCR1, 0);


    /* Config TIM1 Channel 2 PWM Mode 1 */
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M, 0x06U << TIM_CCMR1_OC2M_Pos);

    /* Enable TIM1 Channel 2 Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC2E);

    /* Config TIM1 Channel 2 Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2P);

    /* Config TIM1 Channel 2 Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS2);

    /* Enable TIM1 Channel 2N Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC2NE);

    /* Config TIM1 Channel 2N Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2NP);

    /* Config TIM1 Channel 2N Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS2N);

    /* Config TIM1 Channel 2 Compare Value */
    WRITE_REG(TIM1->CCR2, 0);


    /* Config TIM1 Channel 3 PWM Mode 1 */
    MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC3M, 0x06U << TIM_CCMR2_OC3M_Pos);

    /* Enable TIM1 Channel 3 Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC3E);

    /* Config TIM1 Channel 3 Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3P);

    /* Config TIM1 Channel 3 Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS3);

    /* Enable TIM1 Channel 3N Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC3NE);

    /* Config TIM1 Channel 3N Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3NP);

    /* Config TIM1 Channel 3N Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS3N);

    /* Config TIM1 Channel 3 Compare Value */
    WRITE_REG(TIM1->CCR3, 0);


    /* Off State Selection In The Idle Mode */
    SET_BIT(TIM1->BDTR, TIM_BDTR_OSSI);

    /* Off State Selection In The Run Mode */
    SET_BIT(TIM1->BDTR, TIM_BDTR_OSSR);

    /* Dead-Time Generator Setup Adjustment */
    MODIFY_REG(TIM1->BDTR, TIM_BDTR_DTG, 0x01U << TIM_BDTR_DTG_Pos);

    /* Enable Auto Output */
    SET_BIT(TIM1->BDTR, TIM_BDTR_AOE);

    /* Break Input Is Active High */
    SET_BIT(TIM1->BDTR, TIM_BDTR_BKP);

    /* Enable Break Input */
    SET_BIT(TIM1->BDTR, TIM_BDTR_BKE);


    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA4 AF2 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR4, 0x02U << GPIO_AFRL_AFR4_Pos);   /* TIM1_BKIN */

    /* Config PA4 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE4, 0x00U << GPIO_CRL_MODE4_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF4,  0x02U << GPIO_CRL_CNF4_Pos);
    /* Pull-Down */
    WRITE_REG(GPIOA->BRR, GPIO_BSRR_BR4);

    /* Config PA9 AF2 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR9, 0x02U << GPIO_AFRH_AFR9_Pos);   /* TIM1_CH1 */

    /* Config PA9 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE9, 0x01U << GPIO_CRH_MODE9_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9,  0x02U << GPIO_CRH_CNF9_Pos);

    /* Config PA8 AF1 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR8, 0x01U << GPIO_AFRH_AFR8_Pos);   /* TIM1_CH2 */

    /* Config PA8 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, 0x01U << GPIO_CRH_MODE8_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8,  0x02U << GPIO_CRH_CNF8_Pos);

    /* Config PA6 AF4 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR6, 0x04U << GPIO_AFRL_AFR6_Pos);   /* TIM1_CH3 */

    /* Config PA6 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE6, 0x01U << GPIO_CRL_MODE6_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF6,  0x02U << GPIO_CRL_CNF6_Pos);

    /* Config PA5 AF1 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR5, 0x01U << GPIO_AFRL_AFR5_Pos);   /* TIM1_CH1N */

    /* Config PA5 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE5, 0x01U << GPIO_CRL_MODE5_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF5,  0x02U << GPIO_CRL_CNF5_Pos);

    /* Config PA7 AF1 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR7, 0x01U << GPIO_AFRL_AFR7_Pos);   /* TIM1_CH2N */

    /* Config PA7 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE7, 0x01U << GPIO_CRL_MODE7_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF7,  0x02U << GPIO_CRL_CNF7_Pos);

    /* Config PA0 AF2 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR0, 0x02U << GPIO_AFRL_AFR0_Pos);   /* TIM1_CH3N */

    /* Config PA0 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0, 0x01U << GPIO_CRL_MODE0_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0,  0x02U << GPIO_CRL_CNF0_Pos);


    /* Enable TIM1 */
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);

    /* Enable PWM Output */
    SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_PWM_6StepOutput_DeadTime_Break_Sample(void)
{
    uint8_t Index   = 0;
    uint8_t Step[6] =
    {
        6, 2, 3, 1, 5, 4
    };

    printf("\r\nTest %s", __FUNCTION__);

    TIM1_Configure();

    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1E);
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1NE);

    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2E);
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2NE);

    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3E);
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3NE);

    while (1)
    {
        switch (Step[Index])
        {
            case 6:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1NE);

                WRITE_REG(TIM1->CCR2, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC2E);

                WRITE_REG(TIM1->CCR3, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC3NE);
                break;

            case 2:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3NE);

                WRITE_REG(TIM1->CCR1, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC1NE);

                WRITE_REG(TIM1->CCR2, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC2E);
                break;

            case 3:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2NE);

                WRITE_REG(TIM1->CCR1, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC1NE);

                WRITE_REG(TIM1->CCR3, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC3E);
                break;

            case 1:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1NE);

                WRITE_REG(TIM1->CCR2, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC2NE);

                WRITE_REG(TIM1->CCR3, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC3E);
                break;

            case 5:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3NE);

                WRITE_REG(TIM1->CCR1, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC1E);

                WRITE_REG(TIM1->CCR2, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC2NE);
                break;

            case 4:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2NE);

                WRITE_REG(TIM1->CCR1, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC1E);

                WRITE_REG(TIM1->CCR3, 5000 - 1);
                SET_BIT(TIM1->CCER, TIM_CCER_CC3NE);
                break;

            default:
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1NE);

                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2NE);

                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3E);
                CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3NE);
                break;
        }

        Index = (Index + 1) % 6;

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


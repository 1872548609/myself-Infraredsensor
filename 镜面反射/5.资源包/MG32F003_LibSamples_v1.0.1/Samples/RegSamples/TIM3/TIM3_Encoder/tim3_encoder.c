/**
 * @file    tim3_encoder.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM3_ENCODER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim3_encoder.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_Encoder
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
    WRITE_REG(TIM3->PSC, 0);

    /* Encodeer Mode 3 */
    MODIFY_REG(TIM3->SMCR, TIM_SMCR_SMS, 0x03 << TIM_SMCR_SMS_Pos);

    /* Channel 1 Is Configured As Input, IC1 Is Mapped On TI1 */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_CC1S, 0x01U << TIM_CCMR1_CC1S_Pos);

    /* IC1 Is Active At Low Level */
    CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1P);

    /* Enable Channel 1 Input/Capture Output */
    CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1E);

    /* Channel 2 Is Configured As Input, IC2 Is Mapped On TI2 */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_CC2S, 0x01U << TIM_CCMR1_CC2S_Pos);

    /* IC2 Is Active At Low Level */
    CLEAR_BIT(TIM3->CCER, TIM_CCER_CC2P);

    /* Enable Channel 2 Input/Capture Output */
    CLEAR_BIT(TIM3->CCER, TIM_CCER_CC2E);

    /* Channel 1 Input Capture Filer */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_IC1F, 0x06U << TIM_CCMR1_IC1F_Pos);

    /* Channel 1 Input/Capture Prescaler */
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_IC1PSC, 0x00U << TIM_CCMR1_IC1PSC_Pos);

    /* Reset Counter Value */
    WRITE_REG(TIM3->CNT, 0);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA11 AF4 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR11, 0x04U << GPIO_AFRH_AFR11_Pos); /* TIM3_CH1 */

    /* Config PA11 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE11, 0x00U << GPIO_CRH_MODE11_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF11,  0x02U << GPIO_CRH_CNF11_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS11);

    /* Config PA2 AF4 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR2, 0x04U << GPIO_AFRL_AFR2_Pos);   /* TIM3_CH2 */

    /* Config PA2 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE2, 0x00U << GPIO_CRL_MODE2_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF2,  0x02U << GPIO_CRL_CNF2_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS2);

    /* Enable TIM3 */
    SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_Encoder_Sample(void)
{
    static uint16_t OldCount = 0;
    static uint16_t NewCount = 0;

    printf("\r\nTest %s", __FUNCTION__);

    TIM3_Configure();

    while (1)
    {
        NewCount = READ_REG(TIM3->CNT);

        if (NewCount != OldCount)
        {
            OldCount = NewCount;

            printf("\r\nDIR : %d, CNT : %d", READ_BIT(TIM3->CR1, TIM_CR1_DIR) ? 1 : 0, NewCount);
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


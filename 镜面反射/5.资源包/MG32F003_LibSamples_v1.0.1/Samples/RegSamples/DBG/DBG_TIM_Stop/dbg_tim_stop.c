/**
 * @file    dbg_tim_stop.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _DBG_TIM_STOP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "dbg_tim_stop.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup DBG
  * @{
  */

/**
  * @addtogroup DBG_TIM_Stop
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
void DBG_Configure(void)
{
    uint32_t TimerPeriod   = 0;
    uint32_t Channel1Pulse = 0;
    uint32_t Channel2Pulse = 0;
    uint32_t Channel3Pulse = 0;

    /* Compute the value to be set in ARR regiter to generate signal frequency at 100 Khz */
    TimerPeriod = (PLATFORM_GetPCLK1Frequency() / 100000) - 1;

    /* Compute CCR1 value to generate a duty cycle at 80% for channel 1 */
    Channel1Pulse = ((uint32_t)800 * (TimerPeriod - 1)) / 1000;

    /* Compute CCR2 value to generate a duty cycle at 40% for channel 2 */
    Channel2Pulse = ((uint32_t)400 * (TimerPeriod - 1)) / 1000;

    /* Compute CCR3 value to generate a duty cycle at 25% for channel 3 */
    Channel3Pulse = ((uint32_t)250 * (TimerPeriod - 1)) / 1000;

    /* Enable TIM1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM1);

    /* Config TIM1 Clock Division */
    MODIFY_REG(TIM1->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM1 Edge Alignment Mode */
    MODIFY_REG(TIM1->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM1 Up Count Direction */
    MODIFY_REG(TIM1->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM1 Auto-Reload Value */
    WRITE_REG(TIM1->ARR, TimerPeriod);

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

    /* Config TIM1 Channel 1 Compare Value */
    WRITE_REG(TIM1->CCR1, Channel1Pulse);


    /* Config TIM1 Channel 2 PWM Mode 1 */
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M, 0x06U << TIM_CCMR1_OC2M_Pos);

    /* Enable TIM1 Channel 2 Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC2E);

    /* Config TIM1 Channel 2 Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC2P);

    /* Config TIM1 Channel 2 Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS2);

    /* Config TIM1 Channel 2 Compare Value */
    WRITE_REG(TIM1->CCR2, Channel2Pulse);


    /* Config TIM1 Channel 3 PWM Mode 1 */
    MODIFY_REG(TIM1->CCMR2, TIM_CCMR2_OC3M, 0x06U << TIM_CCMR2_OC3M_Pos);

    /* Enable TIM1 Channel 3 Output */
    SET_BIT(TIM1->CCER, TIM_CCER_CC3E);

    /* Config TIM1 Channel 3 Output Polarity : Active At High Level */
    CLEAR_BIT(TIM1->CCER, TIM_CCER_CC3P);

    /* Config TIM1 Channel 3 Output Idle State */
    SET_BIT(TIM1->CR2, TIM_CR2_OIS3);

    /* Config TIM1 Channel 3 Compare Value */
    WRITE_REG(TIM1->CCR3, Channel3Pulse);


    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

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

    /* Enable TIM1 */
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);

    /* Enable PWM Output */
    SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);

    /* Enable DBGMCU Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_DBG);

    /* Enable TIM1 Stop Work When The Core Enters The Debug Mode */
    SET_BIT(DBG->CR, DBG_CR_DBG_TIM1_STOP);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void DBG_TIM_Stop_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    DBG_Configure();

    while (1)
    {
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


/**
 * @file    pwr_sleep_wfe_wakeup.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_SLEEP_WFE_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_sleep_wfe_wakeup.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_Sleep_WFE_Wakeup
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
void PWR_EXTI_Configure(void)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Enable SYSCFG Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SYSCFG);

    /* Config KEY1(PA2) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE2, 0x00U << GPIO_CRL_MODE2_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF2,  0x02U << GPIO_CRL_CNF2_Pos);
    /* Pull-Down */
    WRITE_REG(GPIOA->BRR, GPIO_BSRR_BR2);

    /* EXTI Line2 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR1, SYSCFG_EXTICR1_EXTI2, 0x00U << SYSCFG_EXTICR1_EXTI2_Pos);
    /* Enable EXTI Line2 Interrupt */
    SET_BIT(EXTI->EMR, EXTI_EMR_EMR2);
    /* EXTI Line2 Rising Trigger */
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR2);

    /* Config KEY2(PA9) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE9, 0x00U << GPIO_CRH_MODE9_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9,  0x02U << GPIO_CRH_CNF9_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS9);

    /* EXTI Line9 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR3, SYSCFG_EXTICR3_EXTI9, 0x00U << SYSCFG_EXTICR3_EXTI9_Pos);
    /* Enable EXTI Line9 Interrupt */
    SET_BIT(EXTI->EMR, EXTI_EMR_EMR9);
    /* EXTI Line9 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR9);

    /* Config KEY3(PA8) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, 0x00U << GPIO_CRH_MODE8_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8,  0x02U << GPIO_CRH_CNF8_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS8);

    /* EXTI Line8 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR3, SYSCFG_EXTICR3_EXTI8, 0x00U << SYSCFG_EXTICR3_EXTI8_Pos);
    /* Enable EXTI Line8 Interrupt */
    SET_BIT(EXTI->EMR, EXTI_EMR_EMR8);
    /* EXTI Line8 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR8);

    /* Config KEY4(PA3) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE3, 0x00U << GPIO_CRL_MODE3_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF3,  0x02U << GPIO_CRL_CNF3_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS3);

    /* EXTI Line3 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR1, SYSCFG_EXTICR1_EXTI3, 0x00U << SYSCFG_EXTICR1_EXTI3_Pos);
    /* Enable EXTI Line3 Interrupt */
    SET_BIT(EXTI->EMR, EXTI_EMR_EMR3);
    /* EXTI Line3 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR3);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Sleep_WFE_Wakeup_Sample(void)
{
    uint32_t Count = 0;

    printf("\r\nTest %s", __FUNCTION__);

    /* Disable SysTick and interrupt */
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

    PWR_EXTI_Configure();

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);

        printf("\r\n%d", Count++);

        /* Enable PWR Clock */
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWR);

        /* Reset SLEEPDEEP bit of Cortex System Control Register */
        CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

        /* Reset SLEEPONEXIT bit of Cortex System Control Register */
        CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPONEXIT_Msk);

        /* Request Wait For Interrupt */
        __SEV();
        __WFE();
        __WFE();

        /* K1 */
        if (READ_BIT(EXTI->PR, EXTI_PR_PR2))
        {
            SET_BIT(EXTI->PR, EXTI_PR_PR2);
        }

        /* K4 */
        if (READ_BIT(EXTI->PR, EXTI_PR_PR3))
        {
            SET_BIT(EXTI->PR, EXTI_PR_PR3);
        }

        /* K3 */
        if (READ_BIT(EXTI->PR, EXTI_PR_PR8))
        {
            SET_BIT(EXTI->PR, EXTI_PR_PR8);
        }

        /* K2 */
        if (READ_BIT(EXTI->PR, EXTI_PR_PR9))
        {
            SET_BIT(EXTI->PR, EXTI_PR_PR9);
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


/**
 * @file    pwr_stop_iwdg_wakeup.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_STOP_IWDG_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_stop_iwdg_wakeup.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_Stop_IWDG_Wakeup
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
void PWR_GPIO_Configure(void)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Enable GPIOB Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOB);

    /* Config GPIOA Analog Input Mode */
    WRITE_REG(GPIOA->CRL, 0x00000000);
    WRITE_REG(GPIOA->CRH, 0x04400000);

    /* Config GPIOB Analog Input Mode */
    WRITE_REG(GPIOB->CRL, 0x00000000);
    WRITE_REG(GPIOB->CRH, 0x00000000);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_IWDG_Configure(void)
{
    /* Enable LSI Clock */
    SET_BIT(RCC->CSR, RCC_CSR_LSION);

    /* Wait LSI Ready */
    while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == 0)
    {
    }

    /* Wait Watchdog Prescaler Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU));
    /* Disable The Protection And Enable Access To Other Configuration Registers */
    WRITE_REG(IWDG->KR, 0x5555);
    /* IWDG Prescaler Divider */
    MODIFY_REG(IWDG->PR, IWDG_PR_PR, 0x03U << IWDG_PR_PR_Pos);
    /* Wait Watchdog Prescaler Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU));

    /* Wait Watchdog Counter Reload Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_RVU));
    /* Disable The Protection And Enable Access To Other Configuration Registers */
    WRITE_REG(IWDG->KR, 0x5555);
    /* Watchdog Counter Reload Value */
    MODIFY_REG(IWDG->RLR, IWDG_RLR_RL, 0xFFFU << IWDG_RLR_RL_Pos);
    /* Wait Watchdog Counter Reload Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU));

    /* Wait Watchdog Interrupt Generate Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_IVU));
    /* Disable The Protection And Enable Access To Other Configuration Registers */
    WRITE_REG(IWDG->KR, 0x5555);
    /* IWDG Interrupt Generate Value */
    MODIFY_REG(IWDG->IGEN, IWDG_IGEN_IGEN, 0x100U << IWDG_IGEN_IGEN_Pos);
    /* Wait Watchdog Interrupt Generate Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_IVU));

    /* Disable The Protection And Enable Access To Other Configuration Registers */
    WRITE_REG(IWDG->KR, 0x5555);
    /* Interrupt Generation Enable After Overflow */
    SET_BIT(IWDG->CR, IWDG_CR_IRQ_SEL);

    /* Feed The Dog */
    WRITE_REG(IWDG->KR, 0xAAAA);

    /* Enable SYSCFG Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SYSCFG);
    /* Enable EXTI Line17 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR17);
    /* EXTI Line17 Rising Trigger */
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR17);
    /* EXTI Line17 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR17);

    NVIC_SetPriority(IWDG_IRQn, 1);
    NVIC_EnableIRQ(IWDG_IRQn);

    /* Start The Watchdog */
    WRITE_REG(IWDG->KR, 0xCCCC);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Pretreatment(void)
{
    printf("\r\n-->Entry Stop Mode");

    PLATFORM_DelayMS(10);

    PWR_GPIO_Configure();

    PWR_IWDG_Configure();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Reinitialize(void)
{
    SystemInit();

    PLATFORM_Init();

    PLATFORM_DelayMS(100);

    printf("\r\n-->Wakeup By IWDG");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Stop_IWDG_Wakeup_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s", __FUNCTION__);

    while (1)
    {
        printf("\r\n-->Running...");

        for (i = 0; i < 20; i++)
        {
            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(100);
        }

        PWR_Pretreatment();

        /* Enable PWR Clock */
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWR);

        /* Stop Mode */
        CLEAR_BIT(PWR->CR, PWR_CR_LPDS);

        /* Set SLEEPDEEP bit of Cortex System Control Register */
        SET_BIT(SCB->SCR,   SCB_SCR_SLEEPDEEP_Msk);

        /* Request Wait For Interrupt */
        __WFI();

        /* Reset SLEEPDEEP bit of Cortex System Control Register */
        CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);

        PWR_Reinitialize();
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


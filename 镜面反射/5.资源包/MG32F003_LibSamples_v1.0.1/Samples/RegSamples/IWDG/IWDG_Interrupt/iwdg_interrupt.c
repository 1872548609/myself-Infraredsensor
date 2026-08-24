/**
 * @file    iwdg_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _IWDG_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "iwdg_interrupt.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup IWDG
  * @{
  */

/**
  * @addtogroup IWDG_Interrupt
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
void IWDG_Configure(void)
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
    MODIFY_REG(IWDG->IGEN, IWDG_IGEN_IGEN, 0x500U << IWDG_IGEN_IGEN_Pos);
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
void IWDG_Interrupt_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    IWDG_Configure();

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


/**
 * @file    pwr_stop_pvd_wakeup.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PWR_STOP_PVD_WAKEUP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "pwr_stop_pvd_wakeup.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup PWR
  * @{
  */

/**
  * @addtogroup PWR_Stop_PVD_Wakeup
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
void PWR_PVD_Configure(void)
{
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
void PWR_Pretreatment(void)
{
    printf("\r\n-->Entry Stop Mode");

    PLATFORM_DelayMS(10);

    PWR_GPIO_Configure();

    PWR_PVD_Configure();
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

    printf("\r\n-->Wakeup By PVD");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PWR_Stop_PVD_Wakeup_Sample(void)
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


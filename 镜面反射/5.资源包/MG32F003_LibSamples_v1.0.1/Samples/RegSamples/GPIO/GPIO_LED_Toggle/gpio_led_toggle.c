/**
 * @file    gpio_led_toggle.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _GPIO_LED_TOGGLE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "gpio_led_toggle.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup GPIO
  * @{
  */

/**
  * @addtogroup GPIO_LED_Toggle
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
void GPIO_Configure(void)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config LED1(PA15) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE15, 0x01U << GPIO_CRH_MODE15_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF15,  0x00U << GPIO_CRH_CNF15_Pos);

    /* Config LED2(PA10) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE10, 0x01U << GPIO_CRH_MODE10_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10,  0x00U << GPIO_CRH_CNF10_Pos);

    /* Config LED3(PA6) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE6,  0x01U << GPIO_CRL_MODE6_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF6,   0x00U << GPIO_CRL_CNF6_Pos);

    /* Config LED4(PA5) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE5,  0x01U << GPIO_CRL_MODE5_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF5,   0x00U << GPIO_CRL_CNF5_Pos);

    /* Set LED1(PA15) */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS15);

    /* Set LED2(PA10) */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS10);

    /* Set LED3(PA6) */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6);

    /* Set LED4(PA5) */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_LED_Toggle_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    GPIO_Configure();

    while (1)
    {
        /* LED1(PA15) Toggle */
        if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR15))
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR15);
        }
        else
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS15);
        }

        /* LED2(PA10) Toggle */
        if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR10))
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR10);
        }
        else
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS10);
        }

        /* LED3(PA6) Toggle */
        if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR6))
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR6);
        }
        else
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6);
        }

        /* LED4(PA5) Toggle */
        if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR5))
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR5);
        }
        else
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5);
        }

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


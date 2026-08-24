/**
 * @file    gpio_swd_remap.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _GPIO_SWD_REMAP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "gpio_swd_remap.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup GPIO
  * @{
  */

/**
  * @addtogroup GPIO_SWD_Remap
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

    /* Config PA13 AF7 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR13, 0x07U << GPIO_AFRH_AFR13_Pos);

    /* Config PA14 AF7 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR14, 0x07U << GPIO_AFRH_AFR14_Pos);

    /* Config PA13 General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE13, 0x01U << GPIO_CRH_MODE13_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF13,  0x00U << GPIO_CRH_CNF13_Pos);

    /* Config PA14 General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE14, 0x01U << GPIO_CRH_MODE14_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF14,  0x00U << GPIO_CRH_CNF14_Pos);

    /* Set PA13 */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS13);

    /* Set PA14 */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS14);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_SWD_Remap_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nDownloadable programm...");

    for (i = 0; i < 10; i++)
    {
        PLATFORM_LED_Toggle(LED1);
        printf(".");

        PLATFORM_DelayMS(500);
    }

    GPIO_Configure();

    printf("\r\nCan't download program unless reset manually!");

    while (1)
    {
        /* PA13 Toggle */
        if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR13))
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR13);
        }
        else
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS13);
        }

        /* PA14 Toggle */
        if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR14))
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR14);
        }
        else
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS14);
        }

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


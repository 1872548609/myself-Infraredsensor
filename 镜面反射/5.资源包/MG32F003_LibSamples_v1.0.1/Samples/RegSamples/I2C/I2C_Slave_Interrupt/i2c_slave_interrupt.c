/**
 * @file    i2c_slave_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_SLAVE_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_slave_interrupt.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Slave_Interrupt
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
void I2C_Configure(void)
{
    /* Enable I2C1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1);

    /* I2C Module Disable */
    CLEAR_BIT(I2C1->ENR, I2C_ENR_ENABLE);

    /* 7-bit Addreesing */
    CLEAR_BIT(I2C1->CR, I2C_CR_SLAVE10);

    /* The Slave Address Of I2C */
    MODIFY_REG(I2C1->SAR, I2C_SAR_ADDR, 0x58 << I2C_SAR_ADDR_Pos);

    /* Slave Enable */
    CLEAR_BIT(I2C1->CR, I2C_CR_DISSLAVE);

    /* Master Disable */
    CLEAR_BIT(I2C1->CR, I2C_CR_MASTER);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA0 AF3 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR0, 0x03U << GPIO_AFRL_AFR0_Pos);

    /* Config PA0 Alternate Function Output Open-Draw */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0, 0x01U << GPIO_CRL_MODE0_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0, 0x03U << GPIO_CRL_CNF0_Pos);

    /* Config PA4 AF3 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR4, 0x03U << GPIO_AFRL_AFR4_Pos);

    /* Config PA4 Alternate Function Output Open-Draw */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE4, 0x01U << GPIO_CRL_MODE4_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF4, 0x03U << GPIO_CRL_CNF4_Pos);

    /* Clear Interrupt Mask Register */
    WRITE_REG(I2C1->IMR, 0);

    /* Enable RX_FULL Interrupt */
    SET_BIT(I2C1->IMR, I2C_IMR_RX_FULL);

    /* Enable RD_REQ Interrupt */
    SET_BIT(I2C1->IMR, I2C_IMR_RD_REQ);

    NVIC_SetPriority(I2C1_IRQn, 0);
    NVIC_EnableIRQ(I2C1_IRQn);

    /* I2C Module Enable */
    SET_BIT(I2C1->ENR, I2C_ENR_ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Slave_Interrupt_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

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


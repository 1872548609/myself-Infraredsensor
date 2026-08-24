/**
 * @file    i2c_slave_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_SLAVE_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_slave_polling.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Slave_Polling
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

    /* I2C Module Enable */
    SET_BIT(I2C1->ENR, I2C_ENR_ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Slave_Polling_Sample(void)
{
    uint8_t i = 0, Buffer[10];
    uint8_t RxLength = 0, TxLength = 0;

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

    while (1)
    {
        /* Receive Buffer Full */
        if(READ_BIT(I2C1->RAWISR, I2C_RAWISR_RX_FULL))
        {
            /* Read Data */
            Buffer[RxLength++] = READ_BIT(I2C1->DR, I2C_DR_DAT) >> I2C_DR_DAT_Pos;

            if(RxLength == 10)
            {
                /* Abort Operation In Progress */
                SET_BIT(I2C1->ENR, I2C_ENR_ABORT);

                /* Abort Operation In Progress */
                while (READ_BIT(I2C1->ENR, I2C_ENR_ABORT))
                {
                }

                /* Clear The TX_ABRT */
                READ_BIT(I2C1->TX_ABRT, I2C_TX_ABRT_TX_ABRT);

                /* STOP Condition Detection */
                while (0 == READ_BIT(I2C1->RAWISR, I2C_RAWISR_STOP))
                {
                }

                TxLength = 0;

                printf("\r\nI2C Slave Receive : ");

                for(i = 0; i < RxLength; i++)
                {
                    printf("0x%02X ", Buffer[i]);
                }
            }
        }

        /* Read Request */
        if(READ_BIT(I2C1->RAWISR, I2C_RAWISR_RD_REQ))
        {
            /* Write Data */
            MODIFY_REG(I2C1->DR, I2C_DR_DAT, Buffer[TxLength++] << I2C_DR_DAT_Pos);

            /* Reading This Rgister Clears The RD_REQ Interrupt */
            READ_BIT(I2C1->RD_REQ, I2C_RD_REQ_RD_REQ);

            if(TxLength == RxLength)
            {
                RxLength = 0;

                printf("\r\nI2C Slave Send Finish.");
            }
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


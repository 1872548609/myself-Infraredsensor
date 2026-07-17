/**
 * @file    mg32f003_it.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _MG32F003_IT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_slave_interrupt.h"
#include "mg32f003_it.h"

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
  * @brief  This function handles NMI exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void NMI_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles Hard Fault exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/***********************************************************************************************************************
  * @brief  This function handles SVCall exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SVC_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles PendSVC exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PendSV_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles SysTick Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SysTick_Handler(void)
{
    if (0 != PLATFORM_DelayTick)
    {
        PLATFORM_DelayTick--;
    }
}

/***********************************************************************************************************************
  * @brief  This function handles I2C1 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C1_IRQHandler(void)
{
    uint8_t i = 0;

    /* Receive Buffer Full */
    if(READ_BIT(I2C1->RAWISR, I2C_RAWISR_RX_FULL))
    {
        I2C_Buffer[I2C_RxLength++] = READ_BIT(I2C1->DR, I2C_DR_DAT) >> I2C_DR_DAT_Pos;

        if(I2C_RxLength == 10)
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

            I2C_TxLength = 0;

            printf("\r\nI2C Slave Receive : ");

            for(i = 0; i < I2C_RxLength; i++)
            {
                printf("0x%02X ", I2C_Buffer[i]);
            }
        }
    }

    /* Read Request */
    if(READ_BIT(I2C1->RAWISR, I2C_RAWISR_RD_REQ))
    {
        /* Write Data */
        MODIFY_REG(I2C1->DR, I2C_DR_DAT, I2C_Buffer[I2C_TxLength++] << I2C_DR_DAT_Pos);

        /* Reading This Rgister Clears The RD_REQ Interrupt */
        READ_BIT(I2C1->RD_REQ, I2C_RD_REQ_RD_REQ);

        if(I2C_TxLength == I2C_RxLength)
        {
            I2C_RxLength = 0;

            printf("\r\nI2C Slave Send Finish.");
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


/**
 * @file    mg32f003_it.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
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
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup Peripheral
  * @{
  */

/**
  * @addtogroup Peripheral_SampleFunction
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

    if (RESET != I2C_GetITStatus(I2C1, I2C_IT_RX_FULL))
    {
        I2C_ClearITPendingBit(I2C1, I2C_IT_RX_FULL);

        I2C_Buffer[I2C_RxLength++] = I2C_ReceiveData(I2C1);

        if(I2C_RxLength == 10)
        {
            I2C_GenerateSTOP(I2C1, ENABLE);
            I2C_TxLength = 0;

            printf("\r\nI2C Slave Receive : ");

            for(i = 0; i < I2C_RxLength; i++)
            {
                printf("0x%02X ", I2C_Buffer[i]);
            }
        }
    }

    if (RESET != I2C_GetITStatus(I2C1, I2C_IT_RD_REQ))
    {
        I2C_SendData(I2C1, I2C_Buffer[I2C_TxLength++]);

        I2C_ClearITPendingBit(I2C1, I2C_IT_RD_REQ);

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


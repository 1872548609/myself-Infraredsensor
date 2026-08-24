/**
 * @file    i2c_slave_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_SLAVE_POLLING_C_

/* Files include */
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "i2c_slave_polling.h"

/**
  * @addtogroup MG32F003_LibSamples
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
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef  I2C_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C1, ENABLE);

    I2C_DeInit(I2C1);

    I2C_StructInit(&I2C_InitStruct);
    I2C_InitStruct.I2C_Mode       = I2C_Mode_Slave;
    I2C_InitStruct.I2C_OwnAddress = 0x00;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_SlaveAddressConfig(I2C1, 0xB0);

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_3);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Slave_Polling_Sample(void)
{
    uint8_t i = 0, Buffer[100];
    uint8_t RxLength = 0, TxLength = 0;

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

    while (1)
    {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_RX_FULL) != RESET)
        {
            Buffer[RxLength++] = I2C_ReceiveData(I2C1);

            if (RxLength == 10)
            {
                I2C_GenerateSTOP(I2C1, ENABLE);

                TxLength = 0;

                printf("\r\nI2C Slave Rx : ");

                for (i = 0; i < RxLength; i++)
                {
                    printf("0x%02X ", Buffer[i]);
                }
            }
        }

        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_RD_REQ) != RESET)
        {
            I2C_SendData(I2C1, Buffer[TxLength++]);

            I2C_ClearFlag(I2C1, I2C_FLAG_RD_REQ);

            if (TxLength == RxLength)
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


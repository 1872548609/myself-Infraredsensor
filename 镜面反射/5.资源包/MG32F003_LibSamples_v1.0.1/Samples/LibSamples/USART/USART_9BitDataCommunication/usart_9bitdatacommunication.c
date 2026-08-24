/**
 * @file    usart_9bitdatacommunication.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_9BITDATACOMMUNICATION_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "usart_9bitdatacommunication.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup USART
  * @{
  */

/**
  * @addtogroup USART_9BitDataCommunication
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
void USART_Configure(uint32_t Baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_2);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2, ENABLE);

    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate   = Baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits   = USART_StopBits_1;
    USART_InitStruct.USART_Parity     = USART_Parity_No;
    USART_InitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStruct);

    USART_ITConfig(USART2, USART_IT_PE, ENABLE);
    USART_ITConfig(USART2, USART_IT_ERR, ENABLE);

    USART_Cmd(USART2, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_9BitDataCommunication_Sample(void)
{
    uint16_t Data = 0;

    printf("\r\nTest %s", __FUNCTION__);

    USART_Configure(115200);

    while (1)
    {
#if 1
        Data += 0x001;
        Data ^= 0x100;

        USART_SendData(USART2, Data);

        while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
        {
        }

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(500);
#else
        if (RESET != USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
        {
            Data = USART_ReceiveData(USART2);

            printf("\r\nRX : 0x%03X", Data);

            USART_SendData(USART2, Data);

            while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
            {
            }
        }
#endif
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


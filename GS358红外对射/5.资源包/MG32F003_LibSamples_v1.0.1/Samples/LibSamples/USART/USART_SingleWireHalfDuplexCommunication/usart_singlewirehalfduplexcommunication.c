/**
 * @file    usart_singlewirehalfduplexcommunication.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_SINGLEWIREHALFDUPLEXCOMMUNICATION_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "usart_singlewirehalfduplexcommunication.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup USART
  * @{
  */

/**
  * @addtogroup USART_SingleWireHalfDuplexCommunication
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

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE);

    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate   = Baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits   = USART_StopBits_1;
    USART_InitStruct.USART_Parity     = USART_Parity_No;
    USART_InitStruct.USART_Mode       = USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStruct);

    USART_HalfDuplexCmd(USART2, ENABLE);

    USART_ITConfig(USART2, USART_IT_PE, ENABLE);
    USART_ITConfig(USART2, USART_IT_ERR, ENABLE);

    USART_Cmd(USART2, ENABLE);

    if(RESET != USART_GetFlagStatus(USART2, USART_FLAG_PE | USART_FLAG_FE | USART_FLAG_NF | USART_FLAG_ORE))
    {
        USART_ReceiveData(USART2);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_SingleWireHalfDuplexCommunication_Sample(void)
{
    uint8_t Data = 0;

    printf("\r\nTest %s", __FUNCTION__);

    USART_Configure(115200);

    while (1)
    {
#if 0
        USART_RxCmd(USART2, DISABLE);
        USART_TxCmd(USART2, ENABLE);

        USART_SendData(USART2, Data++);

        while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
        {
        }

        USART_TxCmd(USART2, DISABLE);
        USART_RxCmd(USART2, ENABLE);

        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(500);
#else
        if (RESET != USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
        {
            Data = USART_ReceiveData(USART2);

            PLATFORM_DelayMS(100);

            USART_RxCmd(USART2, DISABLE);
            USART_TxCmd(USART2, ENABLE);

            USART_SendData(USART2, Data);

            while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
            {
            }

            USART_TxCmd(USART2, DISABLE);
            USART_RxCmd(USART2, ENABLE);
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


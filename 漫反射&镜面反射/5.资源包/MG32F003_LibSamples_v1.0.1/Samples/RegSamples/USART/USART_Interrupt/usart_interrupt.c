/**
 * @file    usart_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "usart_interrupt.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup USART
  * @{
  */

/**
  * @addtogroup USART_Interrupt
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
    uint32_t PCLK1_Frequency = 0;

    PCLK1_Frequency = PLATFORM_GetPCLK1Frequency();

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA1 AF2 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR1, 0x02U << GPIO_AFRL_AFR1_Pos);

    /* Config PA1 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE1, 0x01U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF1,  0x02U << GPIO_CRL_CNF1_Pos);

    /* Enable GPIOB Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOB);

    /* Config PB1 AF2 */
    MODIFY_REG(GPIOB->AFRL, GPIO_AFRL_AFR1, 0x02U << GPIO_AFRL_AFR1_Pos);

    /* Config PB1 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE1, 0x00U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF1,  0x02U << GPIO_CRL_CNF1_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS1);

    /* Enable USART2 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2);

    /* Parity : None */
    CLEAR_BIT(USART2->CR1, USART_CR1_PCE);
    CLEAR_BIT(USART2->CR1, USART_CR1_PS);

    /* Data Length : 8-bit */
    CLEAR_BIT(USART2->CR1, USART_CR1_DL);

    /* Stop Bit : 1-bit */
    MODIFY_REG(USART2->CR2, USART_CR2_STOP, 0x00U << USART_CR2_STOP_Pos);

    /* Baudrate */
    MODIFY_REG(USART2->BRR, USART_BRR_MFD, ((PCLK1_Frequency / Baudrate) / 16) << USART_BRR_MFD_Pos);
    MODIFY_REG(USART2->BRR, USART_BRR_FFD, ((PCLK1_Frequency / Baudrate) % 16) << USART_BRR_FFD_Pos);

    /* Enable Transmit And Receive */
    SET_BIT(USART2->CR1, USART_CR1_TE | USART_CR1_RE);

    /* Enable PE Interrupt */
    SET_BIT(USART2->CR1, USART_CR1_PEIEN);

    /* Enable Error(FE/ORE/NF) Interrupt */
    SET_BIT(USART2->CR3, USART_CR3_ERRIEN);

    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);

    /* Enable USART2 */
    SET_BIT(USART2->CR1, USART_CR1_UE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_RxData_Interrupt(uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        USART_RxStruct.Buffer[i] = 0;
    }

    USART_RxStruct.Length = Length;
    USART_RxStruct.CurrentCount = 0;
    USART_RxStruct.CompleteFlag = 0;

    /* Enable RXNE Interrupt */
    SET_BIT(USART2->CR1, USART_CR1_RXNEIEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_TxData_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        USART_TxStruct.Buffer[i] = Buffer[i];
    }

    USART_TxStruct.Length = Length;
    USART_TxStruct.CurrentCount = 0;
    USART_TxStruct.CompleteFlag = 0;

    /* Enable TXE Interrupt */
    SET_BIT(USART2->CR1, USART_CR1_TXEIEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_Interrupt_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    USART_RxStruct.CompleteFlag = 0;
    USART_TxStruct.CompleteFlag = 1;

    USART_Configure(115200);

    USART_RxData_Interrupt(10);

    printf("\r\nSend 10 bytes to USART every time");

    while (1)
    {
        if (0 != USART_RxStruct.CompleteFlag)
        {
            USART_TxData_Interrupt((uint8_t *)USART_RxStruct.Buffer, USART_RxStruct.Length);

            while (0 == USART_TxStruct.CompleteFlag)
            {
            }

            USART_RxData_Interrupt(10);
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


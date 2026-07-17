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
#include "platform.h"
#include "usart_interrupt.h"
#include "mg32f003_it.h"

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
  * @brief  This function handles USART2 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART2_IRQHandler(void)
{
    uint8_t RxData = 0, TxData = 0;

    if (READ_BIT(USART2->SR, USART_SR_PE) || READ_BIT(USART2->SR, USART_SR_FE) ||
        READ_BIT(USART2->SR, USART_SR_NF) || READ_BIT(USART2->SR, USART_SR_ORE))
    {
        READ_REG(USART2->DR);
    }

    if (READ_BIT(USART2->SR, USART_SR_RXNE))
    {
        RxData = READ_BIT(USART2->DR, USART_DR_DR) >> USART_DR_DR_Pos;

        if (0 == USART_RxStruct.CompleteFlag)
        {
            USART_RxStruct.Buffer[USART_RxStruct.CurrentCount++] = RxData;

            if (USART_RxStruct.CurrentCount == USART_RxStruct.Length)
            {
                USART_RxStruct.CompleteFlag = 1;

                /* Disable RXNE Interrupt */
                CLEAR_BIT(USART2->CR1, USART_CR1_RXNEIEN);
            }
        }
    }

    if (READ_BIT(USART2->SR, USART_SR_TXE))
    {
        if (0 == USART_TxStruct.CompleteFlag)
        {
            TxData = USART_TxStruct.Buffer[USART_TxStruct.CurrentCount++];

            MODIFY_REG(USART2->DR, USART_DR_DR, TxData << USART_DR_DR_Pos);

            if (USART_TxStruct.CurrentCount == USART_TxStruct.Length)
            {
                USART_TxStruct.CompleteFlag = 1;

                /* Disable TXE Interrupt */
                CLEAR_BIT(USART2->CR1, USART_CR1_TXEIEN);
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


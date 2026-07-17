/**
 * @file    usart_singlewirehalfduplexcommunication.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_SINGLEWIREHALFDUPLEXCOMMUNICATION_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "usart_singlewirehalfduplexcommunication.h"

/**
  * @addtogroup MG32F003_RegSamples
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
    uint32_t PCLK1_Frequency = 0;

    PCLK1_Frequency = PLATFORM_GetPCLK1Frequency();

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA1 AF2 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR1, 0x02U << GPIO_AFRL_AFR1_Pos);

    /* Config PA1 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE1, 0x01U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF1,  0x02U << GPIO_CRL_CNF1_Pos);

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

    /* Enable Receive */
    SET_BIT(USART2->CR1, USART_CR1_RE);

    /* Half-Duplex Mode */
    SET_BIT(USART2->CR3, USART_CR3_HDSEL);

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
void USART_SingleWireHalfDuplexCommunication_Sample(void)
{
    uint8_t Data = 0;

    printf("\r\nTest %s", __FUNCTION__);

    USART_Configure(115200);

    while (1)
    {
#if 0
            /* Receiver Disable */
            CLEAR_BIT(USART2->CR1, USART_CR1_RE);
            /* Transmitter Enable */
            SET_BIT(USART2->CR1,   USART_CR1_TE);

            MODIFY_REG(USART2->DR, USART_DR_DR, Data++ << USART_DR_DR_Pos);

            while (0 == READ_BIT(USART2->SR, USART_SR_TC))
            {
            }

            /* Transmitter Disable */
            CLEAR_BIT(USART2->CR1, USART_CR1_TE);
            /* Receiver Enable */
            SET_BIT(USART2->CR1,   USART_CR1_RE);

            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(500);
#else
        if (READ_BIT(USART2->SR, USART_SR_RXNE))
        {
            Data = READ_BIT(USART2->DR, USART_DR_DR) >> USART_DR_DR_Pos;

            PLATFORM_DelayMS(100);

            /* Receiver Disable */
            CLEAR_BIT(USART2->CR1, USART_CR1_RE);
            /* Transmitter Enable */
            SET_BIT(USART2->CR1,   USART_CR1_TE);

            MODIFY_REG(USART2->DR, USART_DR_DR, Data << USART_DR_DR_Pos);

            while (0 == READ_BIT(USART2->SR, USART_SR_TC))
            {
            }

            /* Transmitter Disable */
            CLEAR_BIT(USART2->CR1, USART_CR1_TE);
            /* Receiver Enable */
            SET_BIT(USART2->CR1,   USART_CR1_RE);
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


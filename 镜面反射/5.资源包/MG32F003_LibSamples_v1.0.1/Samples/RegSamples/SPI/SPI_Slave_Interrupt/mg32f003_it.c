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
#include "spi_slave_interrupt.h"
#include "mg32f003_it.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup SPI
  * @{
  */

/**
  * @addtogroup SPI_Slave_Interrupt
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
  * @brief  This function handles EXTI0_1 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI0_1_IRQHandler(void)
{
    if (READ_BIT(EXTI->PR, EXTI_PR_PR0) && READ_BIT(EXTI->IMR, EXTI_IMR_IMR0))
    {
        if (READ_BIT(GPIOA->IDR, GPIO_IDR_IDR0))
        {
            /* SPI_CS High */
        }
        else
        {
            /* SPI_CS Low */
        }

        SET_BIT(EXTI->PR, EXTI_PR_PR0);
    }
}

/***********************************************************************************************************************
  * @brief  This function handles SPI1 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI1_IRQHandler(void)
{
    uint8_t Data = 0;

    if (SPI_RxStruct.CurrentCount != SPI_TxStruct.CurrentCount)
    {
        if (READ_BIT(SPI1->INTSTAT, SPI_INTSTAT_RX_INTF))
        {
            /* Receipt Data */
            Data = READ_REG(SPI1->RXREG);

            /* Receipt Interrupt Clear */
            SET_BIT(SPI1->INTCLR, SPI_INTCLR_RX_ICLR);

            if (0 == SPI_RxStruct.CompleteFlag)
            {
                SPI_RxStruct.Buffer[SPI_RxStruct.CurrentCount++] = Data;

                if (SPI_RxStruct.CurrentCount == SPI_RxStruct.Length)
                {
                    SPI_RxStruct.CompleteFlag = 1;

                    /* Transmission Buffer Enpty Interrupt Disable */
                    CLEAR_BIT(SPI1->INTEN, SPI_INTEN_TX_IEN);

                    /* Receipt Data Interrupt Disable */
                    CLEAR_BIT(SPI1->INTEN, SPI_INTEN_RX_IEN);
                }
            }
        }
    }
    else
    {
        if (READ_BIT(SPI1->INTSTAT, SPI_INTSTAT_TX_INTF))
        {
            /* Transmission Buffer Empty Interrupt Clear */
            SET_BIT(SPI1->INTCLR, SPI_INTCLR_TX_ICLR);

            if (0 == SPI_TxStruct.CompleteFlag)
            {
                /* Transmission Data */
                WRITE_REG(SPI1->TXREG, SPI_TxStruct.Buffer[SPI_TxStruct.CurrentCount++]);

                if (SPI_TxStruct.CurrentCount == SPI_TxStruct.Length)
                {
                    SPI_TxStruct.CompleteFlag = 1;
                }
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


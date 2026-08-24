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
#include "platform.h"
#include "spi_master_flash_interrupt.h"
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
        if (RESET != SPI_GetITStatus(SPI1, SPI_IT_RX))
        {
            Data = SPI_ReceiveData(SPI1);

            SPI_ClearITPendingBit(SPI1, SPI_IT_RX);

            if (0 == SPI_RxStruct.CompleteFlag)
            {
                SPI_RxStruct.Buffer[SPI_RxStruct.CurrentCount++] = Data;

                if (SPI_RxStruct.CurrentCount == SPI_RxStruct.Length)
                {
                    SPI_RxStruct.CompleteFlag = 1;

                    SPI_ITConfig(SPI1, SPI_IT_TX, DISABLE);
                    SPI_ITConfig(SPI1, SPI_IT_RX, DISABLE);
                }
            }
        }
    }
    else
    {
        if (RESET != SPI_GetITStatus(SPI1, SPI_IT_TX))
        {
            SPI_ClearITPendingBit(SPI1, SPI_IT_TX);

            if (0 == SPI_TxStruct.CompleteFlag)
            {
                SPI_SendData(SPI1, SPI_TxStruct.Buffer[SPI_TxStruct.CurrentCount++]);

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


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
#include "i2c_master_interrupt.h"
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
  * @addtogroup I2C_Master_Interrupt
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
  * @brief  This function handles EXTI2_3 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI2_3_IRQHandler(void)
{
    /* K1 */
    if (READ_BIT(EXTI->PR, EXTI_PR_PR2) && READ_BIT(EXTI->IMR, EXTI_IMR_IMR2))
    {
        K1_PressFlag = 1;

        SET_BIT(EXTI->PR, EXTI_PR_PR2);
    }
}

/***********************************************************************************************************************
  * @brief  This function handles EXTI4_15 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI4_15_IRQHandler(void)
{
    /* K2 */
    if (READ_BIT(EXTI->PR, EXTI_PR_PR9) && READ_BIT(EXTI->IMR, EXTI_IMR_IMR9))
    {
        K2_PressFlag = 1;

        SET_BIT(EXTI->PR, EXTI_PR_PR9);
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
    uint8_t Data = 0;

    if (READ_BIT(I2C1->RAWISR, I2C_RAWISR_RX_FULL))
    {
        Data = READ_BIT(I2C1->DR, I2C_DR_DAT) >> I2C_DR_DAT_Pos;

        if (0 == I2C_RxStruct.CompleteFlag)
        {
            I2C_RxStruct.Buffer[I2C_RxStruct.CurrentCount++] = Data;

            if (I2C_RxStruct.CurrentCount == I2C_RxStruct.Length)
            {
                I2C_RxStruct.CompleteFlag = 1;

                /* Disable RX_FULL Interrupt */
                CLEAR_BIT(I2C1->IMR, I2C_IMR_RX_FULL);
            }
            else
            {
                /* Read Command */
                SET_BIT(I2C1->DR, I2C_DR_CMD);
            }
        }
    }

    if (READ_BIT(I2C1->RAWISR, I2C_RAWISR_TX_EMPTY))
    {
        if (0 == I2C_TxStruct.CompleteFlag)
        {
            MODIFY_REG(I2C1->DR, I2C_DR_DAT, I2C_TxStruct.Buffer[I2C_TxStruct.CurrentCount++] << I2C_DR_DAT_Pos);

            if (I2C_TxStruct.CurrentCount == I2C_TxStruct.Length)
            {
                I2C_TxStruct.CompleteFlag = 1;

                /* Disable TX_EMPTY Interrupt */
                CLEAR_BIT(I2C1->IMR, I2C_IMR_TX_EMPTY);
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


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
#include "usart_simulatewithgpio.h"
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
  * @addtogroup USART_SimulateWithGPIO
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
    if (READ_BIT(EXTI->PR, EXTI_PR_PR1) && READ_BIT(EXTI->IMR, EXTI_IMR_IMR1))
    {
        if (sUSART_RX_BIT_STOP == SimulateUSART_RxStep)
        {
            SimulateUSART_RxStep = sUSART_RX_BIT_START;

            SimulateUSART_DelayUS(sUSART_BAUTRATE_DELAY / 3);

            /* Enable TIM3 */
            SET_BIT(TIM3->CR1, TIM_CR1_CEN);
        }

        SET_BIT(EXTI->PR, EXTI_PR_PR1);
    }
}

/***********************************************************************************************************************
  * @brief  This function handles TIM3 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_IRQHandler(void)
{
    SimulateUSART_RxStep++;

    if (sUSART_RX_BIT_STOP == SimulateUSART_RxStep)
    {
        /* Disable TIM3 */
        CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);

        SimulateUSART_RxFlag = 1;
    }
    else
    {
        if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR1))
        {
            SimulateUSART_RxData |= (0x01 << (SimulateUSART_RxStep - 1));
        }
        else
        {
            SimulateUSART_RxData &= ~(0x01 << (SimulateUSART_RxStep - 1));
        }
    }

    /* Clear TIM3 Update Interrupt Flag */
    CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
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


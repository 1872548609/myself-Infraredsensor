/**
 * @file    usart_simulatewithgpio.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_SIMULATEWITHGPIO_C_

/* Files include */
#include <math.h>
#include <stdio.h>
#include "platform.h"
#include "usart_simulatewithgpio.h"

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
  * @brief  
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint32_t TIM3_GetFrequency(void)
{
    uint32_t TIM3_Frequency     = 0;
    uint32_t PPRE1 = 0, APB_DIV = 0;

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        APB_DIV = 1;

        TIM3_Frequency = PLATFORM_GetPCLK1Frequency();
    }
    else
    {
        APB_DIV = pow(2, PPRE1 % 3);

        TIM3_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / APB_DIV;
    }

    return (TIM3_Frequency);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_Configure(void)
{
    /* Enable TIM3 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3);

    /* Config TIM3 Clock Division */
    MODIFY_REG(TIM3->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM3 Edge Alignment Mode */
    MODIFY_REG(TIM3->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM3 Up Count Direction */
    MODIFY_REG(TIM3->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM3 Auto-Reload Value */
    WRITE_REG(TIM3->ARR, sUSART_BAUTRATE_DELAY - 1);

    /* Config TIM3 Prescaler Value */
    WRITE_REG(TIM3->PSC, TIM3_GetFrequency() / 1000000 - 1);

    /* Initialize The Counter, And Generate A Update Event */
    SET_BIT(TIM3->EGR, TIM_EGR_UG);

    /* Clear TIM3 Update Interrupt Flag */
    CLEAR_BIT(TIM3->SR, TIM_SR_UIF);

    /* Enable Update Interrupt */
    SET_BIT(TIM3->DIER, TIM_DIER_UIE);

    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);

    /* Disable TIM3 */
    CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief  
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint32_t TIM14_GetFrequency(void)
{
    uint32_t TIM14_Frequency    = 0;
    uint32_t PPRE1 = 0, APB_DIV = 0;

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        APB_DIV = 1;

        TIM14_Frequency = PLATFORM_GetPCLK1Frequency();
    }
    else
    {
        APB_DIV = pow(2, PPRE1 % 3);

        TIM14_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / APB_DIV;
    }

    return (TIM14_Frequency);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM14_Configure(void)
{
    /* Enable TIM14 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM14);

    /* Config TIM14 Clock Division */
    MODIFY_REG(TIM14->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM14 Edge Alignment Mode */
    MODIFY_REG(TIM14->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM14 Up Count Direction */
    MODIFY_REG(TIM14->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM14 Auto-Reload Value */
    WRITE_REG(TIM14->ARR, sUSART_BAUTRATE_DELAY - 1);

    /* Config TIM14 Prescaler Value */
    WRITE_REG(TIM14->PSC, TIM14_GetFrequency() / 1000000 - 1);

    /* Initialize The Counter, And Generate A Update Event */
    SET_BIT(TIM14->EGR, TIM_EGR_UG);

    /* Clear TIM14 Update Interrupt Flag */
    CLEAR_BIT(TIM14->SR, TIM_SR_UIF);

    /* Disable TIM14 */
    CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUSART_DelayUS(uint32_t Tick)
{
    /* Config TIM14 Auto-Reload Value */
    WRITE_REG(TIM14->ARR, Tick - 1);

    /* Enable TIM14 */
    SET_BIT(TIM14->CR1, TIM_CR1_CEN);

    while (0 == READ_BIT(TIM14->SR, TIM_SR_UIF))
    {
    }

    /* Clear TIM14 Update Interrupt Flag */
    CLEAR_BIT(TIM14->SR, TIM_SR_UIF);

    /* Disable TIM14 */
    CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUSART_Configure(void)
{
    /* Simulate USART Tx : Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA1 General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE1, 0x01U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF1,  0x00U << GPIO_CRL_CNF1_Pos);

    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS1);

    /* Simulate USART Rx : Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOB);

    /* Enable SYSCFG Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SYSCFG);

    /* Config PB1 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE1, 0x00U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF1,  0x02U << GPIO_CRL_CNF1_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS1);

    /* EXTI Line1 Bound To PortB */
    MODIFY_REG(SYSCFG->EXTICR1, SYSCFG_EXTICR1_EXTI1, 0x01U << SYSCFG_EXTICR1_EXTI1_Pos);
    /* Enable EXTI Line1 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR1);
    /* EXTI Line1 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR1);

    NVIC_SetPriority(EXTI0_1_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUSART_SendData(uint8_t Data)
{
    uint8_t i = 0;

    WRITE_REG(GPIOA->BRR, GPIO_BRR_BR1);

    SimulateUSART_DelayUS(sUSART_BAUTRATE_DELAY);

    for (i = 0; i < 8; i++)
    {
        if (Data & 0x01)
        {
            WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS1);
        }
        else
        {
            WRITE_REG(GPIOA->BRR, GPIO_BRR_BR1);
        }

        Data >>= 1;

        SimulateUSART_DelayUS(sUSART_BAUTRATE_DELAY);
    }

    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS1);

    SimulateUSART_DelayUS(sUSART_BAUTRATE_DELAY);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_SimulateWithGPIO_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    SimulateUSART_RxData = 0;
    SimulateUSART_RxFlag = 0;
    SimulateUSART_RxStep = sUSART_RX_BIT_STOP;

    TIM3_Configure();

    TIM14_Configure();

    SimulateUSART_Configure();

    while (1)
    {
        if (1 == SimulateUSART_RxFlag)
        {
            SimulateUSART_RxFlag = 0;

            SimulateUSART_SendData(SimulateUSART_RxData);
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


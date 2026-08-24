/**
 * @file    usart_simulatewithgpio.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_SIMULATEWITHGPIO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "usart_simulatewithgpio.h"

/**
  * @addtogroup MG32F003_LibSamples
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
void TIM3_Configure(void)
{
    NVIC_InitTypeDef        NVIC_InitStruct;
    RCC_ClocksTypeDef       RCC_Clocks;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);

    TIM_DeInit(TIM3);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM3) / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (sUSART_BAUTRATE_DELAY - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM3, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM14_Configure(void)
{
    RCC_ClocksTypeDef       RCC_Clocks;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;

    RCC_GetClocksFreq(&RCC_Clocks);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM14, ENABLE);

    TIM_DeInit(TIM14);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
    TIM_TimeBaseStruct.TIM_Prescaler         = (TIM_GetTIMxClock(TIM14) / 1000000 - 1);
    TIM_TimeBaseStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_Period            = (sUSART_BAUTRATE_DELAY - 1);
    TIM_TimeBaseStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStruct);

    TIM_ClearFlag(TIM14, TIM_FLAG_Update);

    TIM_Cmd(TIM14, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUSART_DelayUS(uint32_t Tick)
{
    TIM_SetAutoreload(TIM14, Tick - 1);

    TIM_Cmd(TIM14, ENABLE);

    while (RESET == TIM_GetFlagStatus(TIM14, TIM_FLAG_Update))
    {
    }

    TIM_ClearFlag(TIM14, TIM_FLAG_Update);

    TIM_Cmd(TIM14, DISABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SimulateUSART_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Simulate USART Tx */
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);

    /* Simulate USART Rx */
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOB, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SYSCFG, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line1;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
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

    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);

    SimulateUSART_DelayUS(sUSART_BAUTRATE_DELAY);

    for (i = 0; i < 8; i++)
    {
        if (Data & 0x01)
        {
            GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
        }
        else
        {
            GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
        }

        Data >>= 1;

        SimulateUSART_DelayUS(sUSART_BAUTRATE_DELAY);
    }

    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);

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


/**
 * @file    tim3_hall.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM3_HALL_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim3_hall.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_Hall
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
    GPIO_InitTypeDef        GPIO_InitStruct;
    NVIC_InitTypeDef        NVIC_InitStruct;
    TIM_ICInitTypeDef       TIM_ICInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);   /* TIM3_CH1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_4);    /* TIM3_CH2 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_4);    /* TIM3_CH3 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Period            = (TIM_GetTIMxClock(TIM3) / 1000000 - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Prescaler         = UINT16_MAX;
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_Div1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity  = TIM_ICPolarity_BothEdge;
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_TRC;
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_Div1;
    TIM_ICInitStruct.TIM_ICFilter    = 0x05;
    TIM_ICInit(TIM3, &TIM_ICInitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_SelectHallSensor(TIM3, ENABLE);

    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);

    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

    TIM_ClearFlag(TIM3, TIM_FLAG_Trigger);
    TIM_ITConfig(TIM3, TIM_IT_Trigger, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_Hall_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM3_Configure();

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(100);
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


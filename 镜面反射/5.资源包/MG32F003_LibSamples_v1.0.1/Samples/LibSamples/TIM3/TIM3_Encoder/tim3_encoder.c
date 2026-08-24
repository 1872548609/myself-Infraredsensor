/**
 * @file    tim3_encoder.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM3_ENCODER_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "tim3_encoder.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_Encoder
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
    TIM_ICInitTypeDef       TIM_ICInitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);   /* TIM3_CH1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_4);    /* TIM3_CH2 */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_Period    = UINT16_MAX;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

//    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI1, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
//    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI2, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);

    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_Channel  = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICFilter = 6;
    TIM_ICInit(TIM3, &TIM_ICInitStruct);

    TIM_SetCounter(TIM3, 0);

    TIM_Cmd(TIM3, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_Encoder_Sample(void)
{
    static uint16_t OldCount = 0;
    static uint16_t NewCount = 0;

    printf("\r\nTest %s", __FUNCTION__);

    TIM3_Configure();

    while (1)
    {
        NewCount = TIM_GetCounter(TIM3);

        if (NewCount != OldCount)
        {
            OldCount = NewCount;

            printf("\r\nDIR : %d, CNT : %d", READ_BIT(TIM3->CR1, TIM_CR1_DIR) ? 1 : 0, NewCount);
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


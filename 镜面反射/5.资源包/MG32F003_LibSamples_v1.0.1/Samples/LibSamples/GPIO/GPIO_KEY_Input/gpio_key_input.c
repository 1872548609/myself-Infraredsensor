/**
 * @file    gpio_key_input.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _GPIO_KEY_INPUT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "gpio_key_input.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup GPIO
  * @{
  */

/**
  * @addtogroup GPIO_KEY_Input
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
void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    /* K1->PA2 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* K2->PA9 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* K3->PA8 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* K4->PA3 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void KEY_FSM_Handler(uint8_t *State, uint8_t *Count, uint8_t InputLevel, uint8_t ActiveLevel, char *Name)
{
    if (0 == *State)
    {
        if (InputLevel == ActiveLevel)
        {
            *Count += 1;

            if (*Count >= 5)
            {
                *State = 1;
                *Count = 0;
                printf("\r\n%s Pressed", Name);
            }
        }
        else
        {
            *Count = 0;
        }
    }
    else
    {
        if (InputLevel != ActiveLevel)
        {
            *Count += 1;

            if (*Count >= 5)
            {
                *State = 0;
                *Count = 0;
                printf("\r\n%s Release", Name);
            }
        }
        else
        {
            *Count = 0;
        }
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_KEY_Input_Sample(void)
{
    static uint8_t KeyState[4] =
    {
        0, 0, 0, 0
    };
    static uint8_t KeyCount[4] =
    {
        0, 0, 0, 0
    };

    printf("\r\nTest %s", __FUNCTION__);

    GPIO_Configure();

    printf("\r\nPress K1-K4...");

    while (1)
    {
        KEY_FSM_Handler(&KeyState[0], &KeyCount[0], GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2), Bit_SET,   "K1");
        KEY_FSM_Handler(&KeyState[1], &KeyCount[1], GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9), Bit_RESET, "K2");
        KEY_FSM_Handler(&KeyState[2], &KeyCount[2], GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8), Bit_RESET, "K3");
        KEY_FSM_Handler(&KeyState[3], &KeyCount[3], GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3), Bit_RESET, "K4");

        PLATFORM_DelayMS(10);
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


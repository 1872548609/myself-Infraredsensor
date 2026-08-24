/**
 * @file    gpio_led_toggle.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _GPIO_LED_TOGGLE_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "gpio_led_toggle.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup GPIO
  * @{
  */

/**
  * @addtogroup GPIO_LED_Toggle
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

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15 | GPIO_Pin_10 | GPIO_Pin_6 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_15 | GPIO_Pin_10 | GPIO_Pin_6 | GPIO_Pin_5, Bit_SET);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_IO_Toggle(GPIO_TypeDef *GPIOn, uint16_t PINn)
{
    if (Bit_RESET == GPIO_ReadOutputDataBit(GPIOn, PINn))
    {
        GPIO_SetBits(GPIOn, PINn);
    }
    else
    {
        GPIO_ResetBits(GPIOn, PINn);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_LED_Toggle_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    GPIO_Configure();

    while (1)
    {
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_15);
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_10);
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_6);
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_5);

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


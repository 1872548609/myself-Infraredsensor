/**
 * @file    gpio_swd_remap.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _GPIO_SWD_REMAP_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "gpio_swd_remap.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup GPIO
  * @{
  */

/**
  * @addtogroup GPIO_SWD_Remap
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

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_7);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_13 | GPIO_Pin_14, Bit_SET);
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
        GPIO_WriteBit(GPIOn, PINn, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOn, PINn, Bit_RESET);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   download program after manual reset
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void GPIO_SWD_Remap_Sample(void)
{
    uint8_t i = 0;

    printf("\r\nTest %s...", __FUNCTION__);

    printf("\r\nDownloadable programm...");

    for (i = 0; i < 10; i++)
    {
        PLATFORM_LED_Toggle(LED1);
        printf(".");

        PLATFORM_DelayMS(500);
    }

    GPIO_Configure();

    printf("\r\nCan't download program unless reset manually!");

    while (1)
    {
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_13);
        GPIO_IO_Toggle(GPIOA, GPIO_Pin_14);

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


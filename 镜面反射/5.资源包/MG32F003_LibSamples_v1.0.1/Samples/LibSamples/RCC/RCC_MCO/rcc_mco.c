/**
 * @file    rcc_mco.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _RCC_MCO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "rcc_mco.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup RCC
  * @{
  */

/**
  * @addtogroup RCC_MCO
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
void RCC_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_3);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_MCOConfig(RCC_MCO_HSI_Div4);

}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void RCC_MCO_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    RCC_Configure();

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


/**
 * @file    rcc_getresetreason.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _RCC_GETRESETREASON_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "rcc_getresetreason.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup RCC
  * @{
  */

/**
  * @addtogroup RCC_GetResetReason
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
void RCC_GetResetReason_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    if(SET == RCC_GetFlagStatus(RCC_FLAG_PINRST))
    {
        printf("\r\nPIN Reset Flag");
    }

    if(SET == RCC_GetFlagStatus(RCC_FLAG_PORRST))
    {
        printf("\r\nPOR/PDR Reset Flag");
    }

    if(SET == RCC_GetFlagStatus(RCC_FLAG_SFTRST))
    {
        printf("\r\nSoftware Reset Flag");
    }

    if(SET == RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
    {
        printf("\r\nIndependent Watchdog Reset Flag");
    }

    RCC_ClearFlag();

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


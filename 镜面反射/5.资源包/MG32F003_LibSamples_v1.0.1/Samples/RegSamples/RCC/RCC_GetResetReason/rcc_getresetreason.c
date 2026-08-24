/**
 * @file    rcc_getresetreason.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _RCC_GETRESETREASON_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "rcc_getresetreason.h"

/**
  * @addtogroup MG32F003_RegSamples
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

    if (READ_BIT(RCC->CSR, RCC_CSR_PINRSTF))
    {
        printf("\r\nPIN Reset Flag");
    }

    if (READ_BIT(RCC->CSR, RCC_CSR_PORRSTF))
    {
        printf("\r\nPOR/PDR Reset Flag");
    }

    if (READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF))
    {
        printf("\r\nSoftware Reset Flag");
    }

    if (READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF))
    {
        printf("\r\nIndependent Watchdog Reset Flag");
    }

    /* Clear Reset Flag */
    SET_BIT(RCC->CSR, RCC_CSR_RMVF);

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


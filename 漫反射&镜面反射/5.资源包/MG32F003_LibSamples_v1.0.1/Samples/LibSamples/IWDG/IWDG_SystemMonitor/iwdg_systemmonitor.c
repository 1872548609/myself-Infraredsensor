/**
 * @file    iwdg_systemmonitor.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _IWDG_SYSTEMMONITOR_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "iwdg_systemmonitor.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup IWDG
  * @{
  */

/**
  * @addtogroup IWDG_SystemMonitor
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
void IWDG_Configure(uint32_t Timeout)
{
    uint16_t Reload = LSI_VALUE / 1000 * Timeout / 32;

    RCC_LSICmd(ENABLE);

    while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSIRDY))
    {
    }

    PVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    RVU_CheckStatus();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(Reload);

    IWDG_ReloadCounter();

    IWDG_Enable();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void IWDG_SystemMonitor_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    IWDG_Configure(500);

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(100);

        IWDG_ReloadCounter();
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


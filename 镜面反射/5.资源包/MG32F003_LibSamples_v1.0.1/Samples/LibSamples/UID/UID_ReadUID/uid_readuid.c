/**
 * @file    uid_readuid.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _UID_READUID_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "uid_readuid.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup UID
  * @{
  */

/**
  * @addtogroup UID_ReadUID
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
void UID_ReadUID_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nUIDw0 : 0x%08x", Get_ChipsetUIDw0());
    printf("\r\nUIDw1 : 0x%08x", Get_ChipsetUIDw1());
    printf("\r\nUIDw2 : 0x%08x", Get_ChipsetUIDw2());

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


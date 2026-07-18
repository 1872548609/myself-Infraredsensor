/**
 * @file    uid_readuid.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _UID_READUID_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "uid_readuid.h"

/**
  * @addtogroup MG32F003_RegSamples
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

    printf("\r\nUIDw0 : 0x%08x", *((volatile uint32_t *)(UID_BASE + 0)));
    printf("\r\nUIDw1 : 0x%08x", *((volatile uint32_t *)(UID_BASE + 4)));
    printf("\r\nUIDw2 : 0x%08x", *((volatile uint32_t *)(UID_BASE + 8)));

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


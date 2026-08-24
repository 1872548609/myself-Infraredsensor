/**
 * @file    flash_mainflash_readprotection.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _FLASH_MAINFLASH_READPROTECTION_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flash_mainflash_readprotection.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup FLASH
  * @{
  */

/**
  * @addtogroup FLASH_MainFlash_ReadProtection
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
void FLASH_MainFlash_ReadProtection_Sample(void)
{
    uint16_t RDP = *((volatile uint16_t *)(0x1FFFF800));

    printf("\r\nTest %s", __FUNCTION__);

    /* Main flash block read protection */
    if (RDP == 0x5AA5)
    {
        printf("\r\nFLASH not read protected, perform protection actions...");

        FLASH_Unlock();
        FLASH_EraseOptionBytes();
        FLASH_ProgramOptionHalfWord(0x1FFFF800, 0x807F);
        FLASH_Lock();

        PLATFORM_DelayMS(100);

        NVIC_SystemReset();
    }
    else
    {
        printf("\r\nFlash read protected!!!");
    }

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


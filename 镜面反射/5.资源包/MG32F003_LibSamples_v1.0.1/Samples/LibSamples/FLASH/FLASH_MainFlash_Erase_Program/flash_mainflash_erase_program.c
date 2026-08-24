/**
 * @file    flash_mainflash_erase_program.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _FLASH_MAINFLASH_ERASE_PROGRAM_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flash_mainflash_erase_program.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup FLASH
  * @{
  */

/**
  * @addtogroup FLASH_MainFlash_Erase_Program
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define FLASH_START_ADDRESS     ((uint32_t)0x08000000)
#define FLASH_PAGE_SIZE         (1024)

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t FLASH_MainFlash_Erase_Program_Test(void)
{
    uint32_t WriteValue    = 0x12345678;
    uint32_t ReadValue     = 0;
    uint32_t OffsetAddress = FLASH_START_ADDRESS + 15 * FLASH_PAGE_SIZE;

    printf("\r\nWrite Value : 0x%08x", WriteValue);

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    FLASH_ErasePage(OffsetAddress);
    FLASH_ClearFlag(FLASH_FLAG_EOP);

    FLASH_ProgramWord(OffsetAddress, WriteValue);
    FLASH_ClearFlag(FLASH_FLAG_EOP);

    FLASH_Lock();

    ReadValue = *(volatile uint32_t *)(OffsetAddress);

    printf("\r\nRead  Value : 0x%08x", ReadValue);

    if (ReadValue == WriteValue)
    {
        printf("\t-->Passed");
        return (1);
    }
    else
    {
        printf("\t-->Failed");
        return (0);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FLASH_MainFlash_Erase_Program_Sample(void)
{
    uint8_t Result = 0;

    printf("\r\nTest %s", __FUNCTION__);

    Result = FLASH_MainFlash_Erase_Program_Test();

    while (1)
    {
        if (0 != Result)
        {
            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(500);
        }
        else
        {
            PLATFORM_LED_Toggle(LED1);
            PLATFORM_DelayMS(100);
        }
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


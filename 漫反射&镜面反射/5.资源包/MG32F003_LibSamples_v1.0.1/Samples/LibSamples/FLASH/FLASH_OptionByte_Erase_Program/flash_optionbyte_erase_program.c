/**
 * @file    flash_optionbyte_erase_program.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _FLASH_OPTIONBYTE_ERASE_PROGRAM_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flash_optionbyte_erase_program.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup FLASH
  * @{
  */

/**
  * @addtogroup FLASH_OptionByte_Erase_Program
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
void FLASH_OptionByte_Erase_Program_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nWrite OptionByte Data0 : 0x%02x", 0x12);
    printf("\r\nWrite OptionByte Data1 : 0x%02x", 0x34);

    FLASH_Unlock();

    FLASH_OPTB_Enable();

    FLASH_EraseOptionBytes();

    FLASH_ProgramOptionByteData(0x1FFFF800, 0xA5);

    FLASH_ProgramOptionByteData(0x1FFFF804, 0x12); /* Data0 */
    FLASH_ProgramOptionByteData(0x1FFFF806, 0x34); /* Data1 */

    FLASH_Lock();

    printf("\r\nRead  OptionByte Data0 : 0x%02x", *((volatile uint16_t *)(0x1FFFF804)));
    printf("\r\nRead  OptionByte Data1 : 0x%02x", *((volatile uint16_t *)(0x1FFFF806)));

    while(1)
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


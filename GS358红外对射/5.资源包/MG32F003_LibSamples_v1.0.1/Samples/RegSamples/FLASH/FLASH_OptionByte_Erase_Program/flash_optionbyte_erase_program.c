/**
 * @file    flash_optionbyte_erase_program.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _FLASH_OPTIONBYTE_ERASE_PROGRAM_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flash_optionbyte_erase_program.h"

/**
  * @addtogroup MG32F003_RegSamples
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

    /* Unlock The Flash Memory */
    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK))
    {
        WRITE_REG(FLASH->KEYR, 0x45670123);
        WRITE_REG(FLASH->KEYR, 0xCDEF89AB);
    }

    /* Unlock The Option Byte Memory */
    WRITE_REG(FLASH->OPTKEYR, 0x45670123);
    WRITE_REG(FLASH->OPTKEYR, 0xCDEF89AB);

    /* Enable Option Byte Erase */
    SET_BIT(FLASH->CR, FLASH_CR_OPTER);
    /* Write Flash Address Register */
    WRITE_REG(FLASH->AR, 0x1FFFF800U);
    /* Start Operation */
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
    /* Wait Operation Complete */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
    /* Disable Option Byte Erase */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTER);
    /* Clear Flash Status Register */
    WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Enable Option Byte Programming */
    SET_BIT(FLASH->CR, FLASH_CR_OPTPG);
    *(volatile uint16_t *)(0x1FFFF804) = 0x12;
    /* Wait Operation Complete */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
    /* Disable Option Byte Programming */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTPG);
    /* Clear Flash Status Register */
    WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Enable Option Byte Programming */
    SET_BIT(FLASH->CR, FLASH_CR_OPTPG);
    *(volatile uint16_t *)(0x1FFFF806) = 0x34;
    /* Wait Operation Complete */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
    /* Disable Option Byte Programming */
    CLEAR_BIT(FLASH->CR, FLASH_CR_OPTPG);
    /* Clear Flash Status Register */
    WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Lock The Flash Memory */
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    printf("\r\nRead  OptionByte Data0 : 0x%02x", *((volatile uint16_t *)(0x1FFFF804)));
    printf("\r\nRead  OptionByte Data1 : 0x%02x", *((volatile uint16_t *)(0x1FFFF806)));

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


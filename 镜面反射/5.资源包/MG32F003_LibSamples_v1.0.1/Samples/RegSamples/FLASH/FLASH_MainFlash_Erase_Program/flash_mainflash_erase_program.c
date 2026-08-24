/**
 * @file    flash_mainflash_erase_program.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _FLASH_MAINFLASH_ERASE_PROGRAM_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flash_mainflash_erase_program.h"

/**
  * @addtogroup MG32F003_RegSamples
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
void FLASH_MainFlash_Erase_Program_Sample(void)
{
    uint32_t WriteValue    = 0x12345678;
    uint32_t ReadValue     = 0;
    uint32_t OffsetAddress = FLASH_START_ADDRESS + 15 * FLASH_PAGE_SIZE;

    printf("\r\nTest %s", __FUNCTION__);

    /* Unlock The Flash Memory */
    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK))
    {
        WRITE_REG(FLASH->KEYR, 0x45670123);
        WRITE_REG(FLASH->KEYR, 0xCDEF89AB);
    }

    /* Clear Flash Status Register */
    WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Enable Page Erase */
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    /* Write Flash Address Register */
    WRITE_REG(FLASH->AR, OffsetAddress);
    /* Start Operation */
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
    /* Wait Operation Complete */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
    /* Disable Page Erase */
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
    /* Clear Flash Status Register */
    WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Enable Programming */
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    *(volatile uint16_t *)(OffsetAddress + 0) = (uint16_t)(WriteValue >> 0);
    /* Wait Operation Complete */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
    /* Disable Programming */
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    /* Clear Flash Status Register */
    WRITE_REG(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Enable Programming */
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    *(volatile uint16_t *)(OffsetAddress + 2) = (uint16_t)(WriteValue >> 16);
    /* Wait Operation Complete */
    while(READ_BIT(FLASH->SR, FLASH_SR_BSY));
    /* Disable Programming */
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    /* Clear Flash Status Register */
    SET_BIT(FLASH->SR, FLASH_SR_EOP | FLASH_SR_WRPRTERR | FLASH_SR_PGERR);

    /* Lock The Flash Memory */
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);

    ReadValue = *(volatile uint32_t *)(OffsetAddress);

    printf("\r\nWrite Value : 0x%08x", WriteValue);
    printf("\r\nRead  Value : 0x%08x", ReadValue);

    if (ReadValue == WriteValue)
    {
        printf("\t-->Passed");
    }
    else
    {
        printf("\t-->Failed");
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


/**
 * @file    flash_simulateeeprom.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _FLASH_SIMULATEEEPROM_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "flash_simulateeeprom.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup FLASH
  * @{
  */

/**
  * @addtogroup FLASH_SimulateEEPROM
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define FLASH_PAGE_NUMBER                   (16)
#define FLASH_PAGE_SIZE                     (1024)
#define FLASH_PAGE_BASE                     (0x08000000)

#define FLASH_SimulateEEPROM_PAGE_SIZE      (FLASH_PAGE_SIZE * 1)
#define FLASH_SimulateEEPROM_PAGE1_START    (FLASH_PAGE_BASE + FLASH_PAGE_SIZE * (FLASH_PAGE_NUMBER - 2))
#define FLASH_SimulateEEPROM_PAGE2_START    (FLASH_PAGE_BASE + FLASH_PAGE_SIZE * (FLASH_PAGE_NUMBER - 1))

/* Private variables **************************************************************************************************/
uint32_t FLASH_SimulateEEPROM_ActivePage = 0;
uint32_t FLASH_SimulateEEPROM_Offset     = 0;

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FLASH_SimulateEEPROM_ErasePage(uint32_t Address)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(Address);
    FLASH_ClearFlag(FLASH_FLAG_EOP);
    FLASH_Lock();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FLASH_SimulateEEPROM_ProgramWord(uint32_t Address, uint32_t Data)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ProgramWord(Address, Data);
    FLASH_ClearFlag(FLASH_FLAG_EOP);
    FLASH_Lock();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FLASH_SimulateEEPROM_Init(void)
{
    uint32_t i = 0;

    printf("\r\n%s", __FUNCTION__);
    printf("\r\nPAGE1_START : 0x%08x", FLASH_SimulateEEPROM_PAGE1_START);
    printf("\r\nPAGE2_START : 0x%08x", FLASH_SimulateEEPROM_PAGE2_START);

    if ((*(volatile uint32_t *)FLASH_SimulateEEPROM_PAGE1_START != 0xFFFFFFFF) &&
        (*(volatile uint32_t *)FLASH_SimulateEEPROM_PAGE2_START == 0xFFFFFFFF))
    {
        for (i = 0; i < FLASH_SimulateEEPROM_PAGE_SIZE; i += FLASH_PAGE_SIZE)
        {
            FLASH_SimulateEEPROM_ErasePage(FLASH_SimulateEEPROM_PAGE2_START + i);
        }

        FLASH_SimulateEEPROM_ActivePage = FLASH_SimulateEEPROM_PAGE1_START;

        printf("\r\nSave To Page1");
    }
    else if ((*(volatile uint32_t *)FLASH_SimulateEEPROM_PAGE1_START == 0xFFFFFFFF) &&
             (*(volatile uint32_t *)FLASH_SimulateEEPROM_PAGE2_START != 0xFFFFFFFF))
    {
        for (i = 0; i < FLASH_SimulateEEPROM_PAGE_SIZE; i += FLASH_PAGE_SIZE)
        {
            FLASH_SimulateEEPROM_ErasePage(FLASH_SimulateEEPROM_PAGE1_START + i);
        }

        FLASH_SimulateEEPROM_ActivePage = FLASH_SimulateEEPROM_PAGE2_START;

        printf("\r\nSave To Page2");
    }
    else
    {
        for (i = 0; i < FLASH_SimulateEEPROM_PAGE_SIZE; i += FLASH_PAGE_SIZE)
        {
            FLASH_SimulateEEPROM_ErasePage(FLASH_SimulateEEPROM_PAGE1_START + i);
        }

        for (i = 0; i < FLASH_SimulateEEPROM_PAGE_SIZE; i += FLASH_PAGE_SIZE)
        {
            FLASH_SimulateEEPROM_ErasePage(FLASH_SimulateEEPROM_PAGE2_START + i);
        }

        FLASH_SimulateEEPROM_ActivePage = FLASH_SimulateEEPROM_PAGE1_START;

        printf("\r\nSave To Page1");
    }

    while (*(volatile uint32_t *)(FLASH_SimulateEEPROM_ActivePage + FLASH_SimulateEEPROM_Offset) != 0xFFFFFFFF)
    {
        FLASH_SimulateEEPROM_Offset += 4;
    }

    printf("\r\nStart--->Page : 0x%08x, Offset = 0x%08x", FLASH_SimulateEEPROM_ActivePage, FLASH_SimulateEEPROM_Offset);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
int FLASH_SimulateEEPROM_Read(uint8_t Index, uint8_t *Value)
{
    uint32_t Start = 0, Address = 0, Data = 0;
    uint8_t Buffer[4];

    Start   = FLASH_SimulateEEPROM_ActivePage;
    Address = FLASH_SimulateEEPROM_ActivePage + FLASH_SimulateEEPROM_Offset - 4;

    while (1)
    {
        if (Address < Start)
        {
            break;
        }

        Data = *(volatile uint32_t *)Address;

        if (0xFFFFFFFF != Data)
        {
            Buffer[0] = (Data & 0xFF000000) >> 0x18;
            Buffer[1] = (Data & 0x00FF0000) >> 0x10;
            Buffer[2] = (Data & 0x0000FF00) >> 0x08;
            Buffer[3] = (Data & 0x000000FF) >> 0x00;

            if ((Buffer[0] == UINT8_MAX - Buffer[1]) && (Buffer[2] == UINT8_MAX - Buffer[3]) && (Buffer[1] == Index))
            {
                *Value = Buffer[3];

                return (0);
            }
        }

        Address -= 4;
    }

    return (-1);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FLASH_SimulateEEPROM_Switch(uint32_t OldPage, uint32_t NewPage)
{
    uint32_t i    = 0;
    uint32_t Data = 0, Offset = FLASH_SimulateEEPROM_PAGE_SIZE;
    uint8_t Index = 0, Value = 0;

    printf("\r\n%s", __FUNCTION__);

    FLASH_SimulateEEPROM_ActivePage = NewPage;
    FLASH_SimulateEEPROM_Offset     = 0;

    while (Offset > 0)
    {
        Data  = *(volatile uint32_t *)(OldPage + Offset - 4);

        Index = (uint8_t)((Data & 0x00FF0000) >> 0x10);

        if ((-1) == FLASH_SimulateEEPROM_Read(Index, &Value))
        {
            FLASH_SimulateEEPROM_ProgramWord(NewPage + FLASH_SimulateEEPROM_Offset, Data);

            FLASH_SimulateEEPROM_Offset += 4;

            if (FLASH_SimulateEEPROM_Offset >= FLASH_SimulateEEPROM_PAGE_SIZE)
            {
                printf("\r\nERROR : Memory is full!!!\r\n");
                return;
            }
        }

        Offset -= 4;
    }

    for (i = 0; i < FLASH_SimulateEEPROM_PAGE_SIZE; i += FLASH_PAGE_SIZE)
    {
        FLASH_SimulateEEPROM_ErasePage(OldPage + i);
    }

    printf("\r\nSwitch-->Page : 0x%08x, Offset = 0x%08x", FLASH_SimulateEEPROM_ActivePage, FLASH_SimulateEEPROM_Offset);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
int FLASH_SimulateEEPROM_Write(uint8_t Index, uint8_t Value)
{
    uint32_t Data = 0, Address;

    Data  |= (UINT8_MAX - Index);
    Data <<= (0x08);
    Data  |= (Index);
    Data <<= (0x08);
    Data  |= (UINT8_MAX - Value);
    Data <<= (0x08);
    Data  |= (Value);

    Address = FLASH_SimulateEEPROM_ActivePage + FLASH_SimulateEEPROM_Offset;

    if (*(volatile uint32_t *)Address == 0xFFFFFFFF)
    {
        FLASH_SimulateEEPROM_ProgramWord(Address, Data);
    }
    else
    {
        return (-1);
    }

    FLASH_SimulateEEPROM_Offset += 4;

    if (FLASH_SimulateEEPROM_Offset >= FLASH_SimulateEEPROM_PAGE_SIZE)
    {
        if (FLASH_SimulateEEPROM_ActivePage == FLASH_SimulateEEPROM_PAGE1_START)
        {
            FLASH_SimulateEEPROM_Switch(FLASH_SimulateEEPROM_PAGE1_START, FLASH_SimulateEEPROM_PAGE2_START);
        }
        else
        {
            FLASH_SimulateEEPROM_Switch(FLASH_SimulateEEPROM_PAGE2_START, FLASH_SimulateEEPROM_PAGE1_START);
        }
    }

    return (0);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void FLASH_SimulateEEPROM_Sample(void)
{
    int      Result = 0;
    uint16_t i      = 0;
    uint8_t  Value  = 0;

    printf("\r\nTest %s", __FUNCTION__);

    printf("\r\nFlash simulate EEPROM capacity : %dbytes", FLASH_SimulateEEPROM_PAGE_SIZE * 2);

    FLASH_SimulateEEPROM_Init();

    for (i = 0; i < 50; i++)
    {
        Value = i + 100;

        Result = FLASH_SimulateEEPROM_Write(i, Value);

        if ((-1) == Result)
        {
            printf("\r\nFLASH Simulate EEPROM Write Error!!!");
        }
    }

    printf("\r\nWrite--->Page : 0x%08x, Offset = 0x%08x", FLASH_SimulateEEPROM_ActivePage, FLASH_SimulateEEPROM_Offset);

    for (i = 0; i < 50; i++)
    {
        Result = FLASH_SimulateEEPROM_Read(i, &Value);

        if ((-1) != Result)
        {
            printf("\r\nIndex = %03d, Value = %03d", i, Value);
        }
        else
        {
            printf("\r\nNonexistent!!!");
        }
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


/**
 * @file    spi_master_simulatewithgpio.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _SPI_MASTER_SIMULATEWITHGPIO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "spi_master_simulatewithgpio.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup SPI
  * @{
  */

/**
  * @addtogroup SPI_Master_SimulateWithGPIO
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define SPI_FLASH_CS_H()    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET)
#define SPI_FLASH_CS_L()    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET)

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t SPI_SimulateRxTxData(uint8_t TxData)
{
    uint8_t i = 0, RxData = 0;

    for (i = 0; i < 8; i++)
    {
        if (TxData & 0x80)
        {
            GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
        }
        else
        {
            GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
        }

        TxData <<= 1;

        GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);

        RxData <<= 1;

        if (Bit_SET == GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))
        {
            RxData |= 0x01;
        }
    }

    GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);

    return (RxData);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_RxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        Buffer[i] = SPI_SimulateRxTxData(0x00);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_TxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        SPI_SimulateRxTxData(Buffer[i]);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_RxBuffer(uint8_t *Buffer, uint8_t Length)
{
    if (Length)
    {
        SPI_RxData_Polling(Buffer, Length);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_TxBuffer(uint8_t *Buffer, uint8_t Length)
{
    if (Length)
    {
        SPI_TxData_Polling(Buffer, Length);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_ReadDeviceID(void)
{
    uint8_t  Command[4] =
    {
        0x90, 0xFF, 0xFF, 0x00
    };
    uint8_t  Buffer[2];
    uint16_t DeviceID = 0;

    SPI_FLASH_CS_L();

    SPI_FLASH_TxBuffer(Command, 0x04);
    SPI_FLASH_RxBuffer(Buffer, 0x02);

    SPI_FLASH_CS_H();

    DeviceID   = Buffer[0];
    DeviceID <<= 8;
    DeviceID  |= Buffer[1];

    printf("\r\nSPI Flash DeviceID : 0x%04x", DeviceID);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_ReadJEDEC_ID(void)
{
    uint8_t  Command = 0x9F;
    uint8_t  Buffer[3];
    uint32_t JEDEC_ID = 0;

    SPI_FLASH_CS_L();
    SPI_FLASH_TxBuffer(&Command, 0x01);
    SPI_FLASH_RxBuffer(Buffer, 0x03);
    SPI_FLASH_CS_H();

    JEDEC_ID   = Buffer[0];
    JEDEC_ID <<= 8;
    JEDEC_ID  |= Buffer[1];
    JEDEC_ID <<= 8;
    JEDEC_ID  |= Buffer[2];

    printf("\r\nSPI Flash JEDEC ID : 0x%06x", JEDEC_ID);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_WaitBusy(void)
{
    uint8_t  Status     = 0;
    uint8_t  Command[2] =
    {
        0x05, 0xFF
    };
    uint32_t Timeout = 0;

    do
    {
        SPI_FLASH_CS_L();
        SPI_FLASH_TxBuffer(Command, 0x02);
        SPI_FLASH_RxBuffer(&Status, 0x01);
        SPI_FLASH_CS_H();

        if (Timeout++ > 0xFFFF)
        {
            break;
        }
    }
    while (Status & 0x01);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
    uint8_t Command = 0x06;

    SPI_FLASH_CS_L();
    SPI_FLASH_TxBuffer(&Command, 0x01);
    SPI_FLASH_CS_H();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_SectorErase(uint16_t Index)
{
    uint8_t  Command[4] =
    {
        0x20, 0x00, 0x00, 0x00
    };
    uint32_t Address = Index * 4 * 1024;

    Command[1] = (uint8_t)((Address >> 16) & 0x000000FF);
    Command[2] = (uint8_t)((Address >> 8) & 0x000000FF);
    Command[3] = (uint8_t)((Address >> 0) & 0x000000FF);

    SPI_FLASH_WriteEnable();

    SPI_FLASH_CS_L();
    SPI_FLASH_TxBuffer(Command, 4);
    SPI_FLASH_CS_H();

    SPI_FLASH_WaitBusy();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_FastRead(uint32_t Address, uint8_t *Buffer, uint32_t Length)
{
    uint8_t Command[5] =
    {
        0x0B, 0x00, 0x00, 0x00, 0xFF
    };

    Command[1] = (uint8_t)((Address >> 16) & 0x000000FF);
    Command[2] = (uint8_t)((Address >> 8) & 0x000000FF);
    Command[3] = (uint8_t)((Address >> 0) & 0x000000FF);

    SPI_FLASH_CS_L();
    SPI_FLASH_TxBuffer(Command, 0x05);
    SPI_FLASH_RxBuffer(Buffer, Length);
    SPI_FLASH_CS_H();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_FLASH_PageProgram(uint32_t Address, uint8_t *Buffer, uint32_t Length)
{
    uint8_t Command[4] =
    {
        0x02, 0x00, 0x00, 0x00
    };

    Command[1] = (uint8_t)((Address >> 16) & 0x000000FF);
    Command[2] = (uint8_t)((Address >> 8) & 0x000000FF);
    Command[3] = (uint8_t)((Address >> 0) & 0x000000FF);

    SPI_FLASH_WriteEnable();

    SPI_FLASH_CS_L();
    SPI_FLASH_TxBuffer(Command, 0x04);
    SPI_FLASH_TxBuffer(Buffer, Length);
    SPI_FLASH_CS_H();

    SPI_FLASH_WaitBusy();
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_Master_SimulateWithGPIO_Sample(void)
{
    uint8_t i = 0;
    uint8_t WriteBuffer[100], ReadBuffer[100];

    printf("\r\nTest %s", __FUNCTION__);

    SPI_Configure();

    SPI_FLASH_ReadDeviceID();

    SPI_FLASH_ReadJEDEC_ID();

    printf("\r\nSPI FLASH Sector Erase...");

    SPI_FLASH_SectorErase(0);

    printf("\r\nSPI FLASH Read...");

    SPI_FLASH_FastRead(0, ReadBuffer, 100);

    for (i = 0; i < 100; i++)
    {
        if (0 == (i % 10))
        {
            printf("\r\n");
        }

        printf("0x%02x ", ReadBuffer[i]);
    }

    printf("\r\nSPI FLASH Page Program...");

    for (i = 0; i < 100; i++)
    {
        WriteBuffer[i] = i;
    }

    SPI_FLASH_PageProgram(0, WriteBuffer, 100);

    printf("\r\nSPI FLASH Read...");

    SPI_FLASH_FastRead(0, ReadBuffer, 100);

    for (i = 0; i < 100; i++)
    {
        if (0 == (i % 10))
        {
            printf("\r\n");
        }

        printf("0x%02x ", ReadBuffer[i]);
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


/**
 * @file    i2c_master_simulatewithgpio.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_MASTER_SIMULATEWITHGPIO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_master_simulatewithgpio.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_SimulateWithGPIO
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define EEPROM_I2C_ADDRESS      0xA0
#define EEPROM_PAGE_SIZE        0x08

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_SCL_Configure(uint8_t Direction)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    if (1 == Direction)
    {
        /* Config PA0 Input Floating Mode */
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0,  0x00U << GPIO_CRL_MODE0_Pos);
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0,   0x01U << GPIO_CRL_CNF0_Pos);
    }
    else
    {
        /* Config PA0 General Purpose Output Push-Pull */
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0,  0x01U << GPIO_CRL_MODE0_Pos);
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0,   0x00U << GPIO_CRL_CNF0_Pos);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_SCL_Write(uint8_t Level)
{
    if (Level)
    {
        WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS0);
    }
    else
    {
        WRITE_REG(GPIOA->BRR, GPIO_BRR_BR0);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_SDA_Configure(uint8_t Direction)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    if (1 == Direction)
    {
        /* Config PA4 Input Floating Mode */
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE4,  0x00U << GPIO_CRL_MODE4_Pos);
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF4,   0x01U << GPIO_CRL_CNF4_Pos);
    }
    else
    {
        /* Config PA4 General Purpose Output Push-Pull */
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE4,  0x01U << GPIO_CRL_MODE4_Pos);
        MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF4,   0x00U << GPIO_CRL_CNF4_Pos);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_SDA_Write(uint8_t Level)
{
    if (Level)
    {
        WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS4);
    }
    else
    {
        WRITE_REG(GPIOA->BRR, GPIO_BRR_BR4);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t sI2C_SDA_Read(void)
{
    return (READ_BIT(GPIOA->IDR, GPIO_IDR_IDR4) ? 1 : 0);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_GenerateStart(void)
{
    sI2C_SDA_Write(1);
    sI2C_SCL_Write(1);
    sI2C_SDA_Write(0);
    sI2C_SCL_Write(0);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_GenerateStop(void)
{
    sI2C_SDA_Write(0);
    sI2C_SCL_Write(1);
    sI2C_SDA_Write(1);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_PutACK(uint8_t Type)
{
    uint8_t i = 0;

    if (Type)
    {
        sI2C_SDA_Write(1);             /* NACK */
    }
    else
    {
        sI2C_SDA_Write(0);             /* ACK */
    }

    sI2C_SCL_Write(1);

    for (i = 0; i < 5; i++)
    {
        __NOP();
    }

    sI2C_SCL_Write(0);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t sI2C_GetACK(void)
{
    uint8_t ACK = 0;

    sI2C_SDA_Write(1);

    sI2C_SDA_Configure(1);

    sI2C_SCL_Write(1);

    ACK = sI2C_SDA_Read();

    sI2C_SCL_Write(0);

    sI2C_SDA_Configure(0);

    return (ACK);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t sI2C_ReadByte(void)
{
    uint8_t i = 0, j = 0, Data = 0;

    sI2C_SDA_Write(1);

    sI2C_SDA_Configure(1);

    for (i = 0; i < 8; i++)
    {
        sI2C_SCL_Write(1);

        Data <<= 1;

        if (sI2C_SDA_Read())
        {
            Data |= 0x01;
        }
        else
        {
            Data &= 0xFE;
        }

        sI2C_SCL_Write(0);

        for (j = 0; j < 5; j++)
        {
            __NOP();
        }
    }

    sI2C_SDA_Configure(0);

    return (Data);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void sI2C_WriteByte(uint8_t Data)
{
    uint8_t i = 0, j = 0;

    for (i = 0; i < 8; i++)
    {
        if (Data & 0x80)
        {
            sI2C_SDA_Write(1);
        }
        else
        {
            sI2C_SDA_Write(0);
        }

        Data <<= 1;

        sI2C_SCL_Write(1);

        for (j = 0; j < 5; j++)
        {
            __NOP();
        }

        sI2C_SCL_Write(0);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Configure(void)
{
    sI2C_SCL_Configure(0);
    sI2C_SDA_Configure(0);

    sI2C_SCL_Write(1);
    sI2C_SDA_Write(1);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t I2C_TxData_Polling(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    uint16_t i = 0;

    sI2C_GenerateStart();

    sI2C_WriteByte(EEPROM_I2C_ADDRESS);

    if (sI2C_GetACK())
    {
        sI2C_GenerateStop();
        return (1);
    }

    sI2C_WriteByte(Address);

    if (sI2C_GetACK())
    {
        sI2C_GenerateStop();
        return (1);
    }

    for (i = 0; i < Length; i++)
    {
        sI2C_WriteByte(*Buffer++);

        if (sI2C_GetACK())
        {
            break;
        }
    }

    sI2C_GenerateStop();

    if (i == Length)
    {
        return (0);
    }
    else
    {
        return (1);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint8_t I2C_RxData_Polling(uint8_t Address, uint8_t *Buffer, uint16_t Length)
{
    sI2C_GenerateStart();

    sI2C_WriteByte(EEPROM_I2C_ADDRESS);

    if (sI2C_GetACK())
    {
        sI2C_GenerateStop();
        return (1);
    }

    sI2C_WriteByte(Address);

    if (sI2C_GetACK())
    {
        sI2C_GenerateStop();
        return (1);
    }

    sI2C_GenerateStart();

    sI2C_WriteByte(EEPROM_I2C_ADDRESS + 1);

    if (sI2C_GetACK())
    {
        sI2C_GenerateStop();
        return (1);
    }

    while (1)
    {
        *Buffer++ = sI2C_ReadByte();

        if (--Length == 0)
        {
            sI2C_PutACK(1);
            break;
        }

        sI2C_PutACK(0);
    }

    sI2C_GenerateStop();

    return (0);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EEPROM_WritePage(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_Polling(Address, Buffer, Length);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EEPROM_ReadData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_RxData_Polling(Address, Buffer, Length);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EEPROM_WriteData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    uint8_t Start = 0;
    uint8_t StartCount = 0, PageNumber = 0, FinalCount = 0;

    if ((Address % EEPROM_PAGE_SIZE) == 0)
    {
        StartCount = 0;
        PageNumber = Length / EEPROM_PAGE_SIZE;
        FinalCount = Length % EEPROM_PAGE_SIZE;
    }
    else
    {
        Start = Address % EEPROM_PAGE_SIZE;

        if (((Start + Length) / EEPROM_PAGE_SIZE) == 0)
        {
            StartCount = Length;
            PageNumber = 0;
            FinalCount = 0;
        }
        else
        {
            StartCount = EEPROM_PAGE_SIZE - Start;
            PageNumber = (Length - StartCount) / EEPROM_PAGE_SIZE;
            FinalCount = (Length - StartCount) % EEPROM_PAGE_SIZE;
        }
    }

    if (StartCount)
    {
        EEPROM_WritePage(Address, Buffer, StartCount);

        Address += StartCount;
        Buffer  += StartCount;

        PLATFORM_DelayMS(50);
    }

    while (PageNumber--)
    {
        EEPROM_WritePage(Address, Buffer, EEPROM_PAGE_SIZE);

        Address += EEPROM_PAGE_SIZE;
        Buffer  += EEPROM_PAGE_SIZE;

        PLATFORM_DelayMS(50);
    }

    if (FinalCount)
    {
        EEPROM_WritePage(Address, Buffer, FinalCount);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Master_SimulateWithGPIO_Sample(void)
{
    uint8_t i = 0;
    uint8_t ReadBuffer[20], WriteBuffer[20];

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure();

    for (i = 0; i < 20; i++)
    {
        ReadBuffer[i]  = 0;
        WriteBuffer[i] = i;
    }

    printf("\r\n\r\nEEPROM Write : ");

    EEPROM_WriteData(0, WriteBuffer, 20);

    printf("OK");

    printf("\r\n\r\nEEPROM Read  : \r\n");

    EEPROM_ReadData(0, ReadBuffer, 20);

    for (i = 0; i < 20; i++)
    {
        printf("0x%02x ", ReadBuffer[i]);

        if (0 == ((i + 1) % 10))
        {
            printf("\r\n");
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


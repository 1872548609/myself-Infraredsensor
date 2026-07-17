/**
 * @file    i2c_master_eeprom_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_MASTER_EEPROM_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_master_eeprom_polling.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_EEPROM_Polling
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
void I2C_Configure(uint32_t ClockSpeed)
{
    uint32_t SCLH    = 0, SCLL   = 0;
    uint32_t tSYNC1  = 0, tSYNC2 = 0;
    uint32_t I2C_CLK = 0;

    uint32_t SSHR = 0, SSLR = 0;
    uint32_t FSHR = 0, FSLR = 0;

    /* Enable I2C1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1);

    /* I2C Module Disable */
    CLEAR_BIT(I2C1->ENR, I2C_ENR_ENABLE);

    if (ClockSpeed <= 100000)
    {
        /* UM 14.3.3.7 SCL Configuration */
        SCLH    = 4000000000U / ClockSpeed / 2;
        SCLL    = 4000000000U / ClockSpeed - SCLH;

        I2C_CLK = 4000000000U / PLATFORM_GetPCLK1Frequency();

        tSYNC1  = 1 * I2C_CLK;
        tSYNC2  = 1 * I2C_CLK;

        SSHR    = (SCLH - tSYNC1) / I2C_CLK - 12;
        SSLR    = (SCLL - tSYNC2) / I2C_CLK - 1;

        /* Standard Mode SCL High-Period Count(MIN Valid Value Is 6) */
        MODIFY_REG(I2C1->SSHR, I2C_SSHR_CNT, SSHR << I2C_SSHR_CNT_Pos);

        /* Standard Mode SCL Low-Period Count(MIN Valid Value Is 8) */
        MODIFY_REG(I2C1->SSLR, I2C_SSLR_CNT, SSLR << I2C_SSLR_CNT_Pos);

        /* Standard Mode(Up To 100kbps) */
        MODIFY_REG(I2C1->CR, I2C_CR_SPEED, 0x01U << I2C_CR_SPEED_Pos);
    }
    else
    {
        /* UM 14.3.3.7 SCL Configuration */
        SCLH    = 4000000000U / ClockSpeed / 2;
        SCLL    = 4000000000U / ClockSpeed - SCLH;

        I2C_CLK = 4000000000U / PLATFORM_GetPCLK1Frequency();

        tSYNC1  = 3 * I2C_CLK;
        tSYNC2  = 3 * I2C_CLK;

        FSHR    = (SCLH - tSYNC1) / I2C_CLK - 12;
        FSLR    = (SCLL - tSYNC2) / I2C_CLK - 1;

        /* Fast Mode SCL High-Period Count(MIN Valid Value Is 6) */
        MODIFY_REG(I2C1->FSHR, I2C_FSHR_CNT, FSHR << I2C_FSHR_CNT_Pos);

        /* Fast Mode SCL Low-Period Count(MIN Valid Value Is 8) */
        MODIFY_REG(I2C1->FSLR, I2C_FSLR_CNT, FSLR << I2C_FSLR_CNT_Pos);

        /* Fast Mode(Up To 400kbps) */
        MODIFY_REG(I2C1->CR, I2C_CR_SPEED, 0x02U << I2C_CR_SPEED_Pos);
    }

    /* 7-bit Addreesing */
    CLEAR_BIT(I2C1->CR, I2C_CR_MASTER10);

    /* Target Address For Any Master Transaction */
    MODIFY_REG(I2C1->TAR, I2C_TAR_ADDR, (EEPROM_I2C_ADDRESS >> 1) << I2C_TAR_ADDR_Pos);

    /* Slave Disable */
    SET_BIT(I2C1->CR, I2C_CR_DISSLAVE);

    /* Master Enable */
    SET_BIT(I2C1->CR, I2C_CR_MASTER);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA0 AF3 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR0, 0x03U << GPIO_AFRL_AFR0_Pos);

    /* Config PA0 Alternate Function Output Open-Draw */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0, 0x01U << GPIO_CRL_MODE0_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0, 0x03U << GPIO_CRL_CNF0_Pos);

    /* Config PA4 AF3 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR4, 0x03U << GPIO_AFRL_AFR4_Pos);

    /* Config PA4 Alternate Function Output Open-Draw */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE4, 0x01U << GPIO_CRL_MODE4_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF4, 0x03U << GPIO_CRL_CNF4_Pos);

    /* I2C Module Enable */
    SET_BIT(I2C1->ENR, I2C_ENR_ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_TxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        /* Write Data */
        MODIFY_REG(I2C1->DR, I2C_DR_DAT, Buffer[i] << I2C_DR_DAT_Pos);

        /* Transmit FIFO Completely Empty */
        while (0 == READ_BIT(I2C1->SR, I2C_SR_TFE))
        {
        }
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_RxData_Polling(uint8_t *Buffer, uint16_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        /* Read Command */
        SET_BIT(I2C1->DR, I2C_DR_CMD);

        /* Receive FIOF Not Empty */
        while (0 == READ_BIT(I2C1->SR, I2C_SR_RFNE))
        {
        }

        /* Read Data */
        Buffer[i] = READ_BIT(I2C1->DR, I2C_DR_DAT) >> I2C_DR_DAT_Pos;
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EEPROM_WritePage(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_Polling((uint8_t *)&Address, 0x01);

    I2C_TxData_Polling((uint8_t *)Buffer, Length);

    /* Abort Operation In Progress */
    SET_BIT(I2C1->ENR, I2C_ENR_ABORT);

    /* Abort Operation In Progress */
    while (READ_BIT(I2C1->ENR, I2C_ENR_ABORT))
    {
    }

    /* Clear The TX_ABRT */
    READ_BIT(I2C1->TX_ABRT, I2C_TX_ABRT_TX_ABRT);

    /* STOP Condition Detection */
    while (0 == READ_BIT(I2C1->RAWISR, I2C_RAWISR_STOP))
    {
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EEPROM_ReadData(uint8_t Address, uint8_t *Buffer, uint8_t Length)
{
    I2C_TxData_Polling((uint8_t *)&Address, 0x01);

    I2C_RxData_Polling((uint8_t *)Buffer, Length);

    /* Abort Operation In Progress */
    SET_BIT(I2C1->ENR, I2C_ENR_ABORT);

    /* Abort Operation In Progress */
    while (READ_BIT(I2C1->ENR, I2C_ENR_ABORT))
    {
    }

    /* Clear The TX_ABRT */
    READ_BIT(I2C1->TX_ABRT, I2C_TX_ABRT_TX_ABRT);

    /* STOP Condition Detection */
    while (0 == READ_BIT(I2C1->RAWISR, I2C_RAWISR_STOP))
    {
    }
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
void I2C_Master_EEPROM_Polling_Sample(void)
{
    uint8_t i = 0;
    uint8_t ReadBuffer[20], WriteBuffer[20];

    printf("\r\nTest %s", __FUNCTION__);

    I2C_Configure(100000);

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


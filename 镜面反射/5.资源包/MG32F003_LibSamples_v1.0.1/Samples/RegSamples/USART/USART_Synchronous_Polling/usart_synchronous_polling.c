/**
 * @file    usart_synchronous_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _USART_SYNCHRONOUS_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "usart_synchronous_polling.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup USART
  * @{
  */

/**
  * @addtogroup USART_Synchronous_Polling
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define SPI_FLASH_CS_H()    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5)
#define SPI_FLASH_CS_L()    WRITE_REG(GPIOA->BRR,  GPIO_BRR_BR5)

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_Configure(uint32_t Baudrate)
{
    uint32_t PCLK1_Frequency = 0;

    PCLK1_Frequency = PLATFORM_GetPCLK1Frequency();

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Enable GPIOB Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOB);

    /* Config PA5(SPI FLASH CS) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE5,  0x01U << GPIO_CRL_MODE5_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF5,   0x00U << GPIO_CRL_CNF5_Pos);

    SPI_FLASH_CS_H();

    /* Config PA1 AF2(USART2_TX) */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR1, 0x02U << GPIO_AFRL_AFR1_Pos);

    /* Config PA1 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE1, 0x01U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF1,  0x02U << GPIO_CRL_CNF1_Pos);

    /* Config PB1 AF2(USART2_RX) */
    MODIFY_REG(GPIOB->AFRL, GPIO_AFRL_AFR1, 0x02U << GPIO_AFRL_AFR1_Pos);

    /* Config PB1 Input Floating Mode */
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE1, 0x00U << GPIO_CRL_MODE1_Pos);
    MODIFY_REG(GPIOB->CRL, GPIO_CRL_CNF1,  0x01U << GPIO_CRL_CNF1_Pos);

    /* Config PA3 AF2(USART2_SCK) */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR3, 0x02U << GPIO_AFRL_AFR3_Pos);

    /* Config PA3 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE3, 0x01U << GPIO_CRL_MODE3_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF3,  0x02U << GPIO_CRL_CNF3_Pos);

    /* Enable USART2 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2);

    /* Baud Rate */
    MODIFY_REG(USART2->BRR, USART_BRR_MFD, (PCLK1_Frequency / Baudrate / 4) << USART_BRR_MFD_Pos);

    /* Synchronous Mode */
    SET_BIT(USART2->CR1, USART_CR1_SAS);

    /* MSB Mode */
    SET_BIT(USART2->CR1, USART_CR1_MLS);

    /* Full-Duplex Mode */
    CLEAR_BIT(USART2->CR3, USART_CR3_HDSEL);

    /* The First Clock Transition Is The First Data Capture Edge */
    CLEAR_BIT(USART2->CR2, USART_CR2_CPHA);

    /* The Clock Is At Low Level When Idle */
    CLEAR_BIT(USART2->CR2, USART_CR2_CPOL);

    /* Enable Transmit And Receive */
    SET_BIT(USART2->CR1, USART_CR1_TE | USART_CR1_RE);

    /* Enable USART2 */
    SET_BIT(USART2->CR1, USART_CR1_UE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_Synchronous_RxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0, Data = 0x00;

    for (i = 0; i < Length; i++)
    {
        MODIFY_REG(USART2->DR, USART_DR_DR, Data << USART_DR_DR_Pos);

        while (0 == READ_BIT(USART2->SR, USART_SR_TC))
        {
        }

        while (0 == READ_BIT(USART2->SR, USART_SR_RXNE))
        {
        }

        Buffer[i] = READ_BIT(USART2->DR, USART_DR_DR) >> USART_DR_DR_Pos;
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void USART_Synchronous_TxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0, Data = 0x00;

    for (i = 0; i < Length; i++)
    {
        MODIFY_REG(USART2->DR, USART_DR_DR, Buffer[i] << USART_DR_DR_Pos);

        while (0 == READ_BIT(USART2->SR, USART_SR_TC))
        {
        }

        while (0 == READ_BIT(USART2->SR, USART_SR_RXNE))
        {
        }

        Data = READ_BIT(USART2->DR, USART_DR_DR) >> USART_DR_DR_Pos;
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
        USART_Synchronous_RxData_Polling(Buffer, Length);
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
        USART_Synchronous_TxData_Polling(Buffer, Length);
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
void USART_Synchronous_Polling_Sample(void)
{
    uint8_t i = 0;
    uint8_t WriteBuffer[100], ReadBuffer[100];

    /* Hardware Environment */
    PLATFORM_LED_Enable(LED1, ENABLE);
    PLATFORM_LED_Enable(LED2, ENABLE);
    PLATFORM_LED_Enable(LED3, ENABLE);
    PLATFORM_LED_Enable(LED4, ENABLE);

    printf("\r\nShort PA5 To PA15");
    printf("\r\nShort PA3 To PA8");
    printf("\r\nShort PA1 To PA9");
    printf("\r\nShort PB1 To PA2");
    printf("\r\n");

    for (i = 0; i < 100; i++)
    {
        WriteBuffer[i] = i;
    }

    printf("\r\nTest %s", __FUNCTION__);

    USART_Configure(1000000);

    printf("\r\n");

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
        PLATFORM_LED_Toggle(LED3);
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


/**
 * @file    spi_master_flash_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _SPI_MASTER_FLASH_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "spi_master_flash_interrupt.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup SPI
  * @{
  */

/**
  * @addtogroup SPI_Master_FLASH_Interrupt
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define SPI_FLASH_CS_H() SET_BIT(SPI1->NSSR, SPI_NSSR_NSS)
#define SPI_FLASH_CS_L() CLEAR_BIT(SPI1->NSSR, SPI_NSSR_NSS)

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
    /* Enable SPI1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI1);

    /* Receipt Data Number */
    MODIFY_REG(SPI1->RXDNR, SPI_RXDNR_RXDNR, 0x00U << SPI_RXDNR_RXDNR_Pos);

    /* Only Valid In Low 8 Bit */
    CLEAR_BIT(SPI1->GCTL, SPI_GCTL_DW8_32);

    /* SPI Data Width Bit : 8 Bit Data */
    SET_BIT(SPI1->CCTL, SPI_CCTL_SPILEN);

    /* NSS Output Control By The NSSR Register Value */
    CLEAR_BIT(SPI1->GCTL, SPI_GCTL_NSS);

    /* Host/Master Mode */
    SET_BIT(SPI1->GCTL, SPI_GCTL_MODE);

    /* Data Transmission Or Receipt Highest Bit In Front : MSB */
    CLEAR_BIT(SPI1->CCTL, SPI_CCTL_LSBFE);

    /* Clock In Low Level In Idle Status */
    CLEAR_BIT(SPI1->CCTL, SPI_CCTL_CPOL);

    /* The First Data Bit Sampling Begins From The First Clock Edge */
    SET_BIT(SPI1->CCTL, SPI_CCTL_CPHA);

    /* SPI Baud Rate Control, Baud Rate = PCLK1 / SPBRG */
    MODIFY_REG(SPI1->SPBRG, SPI_SPBRG_SPBRG, 0x100U << SPI_SPBRG_SPBRG_Pos);

    /* Receipt Enable */
    SET_BIT(SPI1->GCTL, SPI_GCTL_RXEN);

    /* Transmission Enable */
    SET_BIT(SPI1->GCTL, SPI_GCTL_TXEN);

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA2 AF3 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR2, 0x03U << GPIO_AFRL_AFR2_Pos);   /* SPI1_MISO */

    /* Config PA2 Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE2, 0x00U << GPIO_CRL_MODE2_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF2,  0x02U << GPIO_CRL_CNF2_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS2);

    /* Config PA8 AF0 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR8, 0x00U << GPIO_AFRH_AFR8_Pos);   /* SPI1_SCK */

    /* Config PA8 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, 0x01U << GPIO_CRH_MODE8_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8,  0x02U << GPIO_CRH_CNF8_Pos);

    /* Config PA9 AF0 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR9, 0x00U << GPIO_AFRH_AFR9_Pos);   /* SPI1_MOSI */

    /* Config PA9 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE9, 0x01U << GPIO_CRH_MODE9_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9,  0x02U << GPIO_CRH_CNF9_Pos);

    /* Config PA15 AF0 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR15, 0x00U << GPIO_AFRH_AFR15_Pos); /* SPI1_NSS */

    /* Config PA15 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE15, 0x01U << GPIO_CRH_MODE15_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF15,  0x02U << GPIO_CRH_CNF15_Pos);

    NVIC_SetPriority(SPI1_IRQn, 1);
    NVIC_EnableIRQ(SPI1_IRQn);

    /* SPI Enable */
    SET_BIT(SPI1->GCTL, SPI_GCTL_SPIEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_RxData_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        SPI_RxStruct.Buffer[i] = 0;
        SPI_TxStruct.Buffer[i] = 0;
    }

    SPI_RxStruct.Length = Length;
    SPI_RxStruct.CurrentCount = 0;
    SPI_RxStruct.CompleteFlag = 0;

    SPI_TxStruct.Length = Length;
    SPI_TxStruct.CurrentCount = 0;
    SPI_TxStruct.CompleteFlag = 0;

    /* Enable SPI Interrupt */
    SET_BIT(SPI1->GCTL, SPI_GCTL_INTEN);

    /* Receipt Data Interrupt Enable */
    SET_BIT(SPI1->INTEN, SPI_INTEN_RX_IEN);

    /* Transmission Buffer Enpty Interrupt Enable */
    SET_BIT(SPI1->INTEN, SPI_INTEN_TX_IEN);

    while (0 == SPI_TxStruct.CompleteFlag)
    {
    }

    while (0 == SPI_RxStruct.CompleteFlag)
    {
    }

    for (i = 0; i < Length; i++)
    {
        Buffer[i] = SPI_RxStruct.Buffer[i];
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_TxData_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        SPI_RxStruct.Buffer[i] = 0;
        SPI_TxStruct.Buffer[i] = Buffer[i];
    }

    SPI_RxStruct.Length = Length;
    SPI_RxStruct.CurrentCount = 0;
    SPI_RxStruct.CompleteFlag = 0;

    SPI_TxStruct.Length = Length;
    SPI_TxStruct.CurrentCount = 0;
    SPI_TxStruct.CompleteFlag = 0;

    /* Enable SPI Interrupt */
    SET_BIT(SPI1->GCTL, SPI_GCTL_INTEN);

    /* Receipt Data Interrupt Enable */
    SET_BIT(SPI1->INTEN, SPI_INTEN_RX_IEN);

    /* Transmission Buffer Enpty Interrupt Enable */
    SET_BIT(SPI1->INTEN, SPI_INTEN_TX_IEN);

    while (0 == SPI_TxStruct.CompleteFlag)
    {
    }

    while (0 == SPI_RxStruct.CompleteFlag)
    {
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
        SPI_RxData_Interrupt(Buffer, Length);
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
        SPI_TxData_Interrupt(Buffer, Length);
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
void SPI_Master_FLASH_Interrupt_Sample(void)
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


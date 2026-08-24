/**
 * @file    spi_master_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _SPI_MASTER_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "spi_master_polling.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup SPI
  * @{
  */

/**
  * @addtogroup SPI_Master_Polling
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/
#define SPI_FLASH_CS_H() SPI_CSInternalSelected(SPI1, DISABLE)
#define SPI_FLASH_CS_L() SPI_CSInternalSelected(SPI1, ENABLE)

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SYSCFG, ENABLE);

    /* K3->PA8->EXTI_Line8 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line8;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* K4->PA3->EXTI_Line3 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line3;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* EXTI Interrupt */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef  SPI_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI1, ENABLE);

    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStruct.SPI_DataWidth = 8;
    SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS       = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudratePrescaler_256;
    SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_BiDirectionalLineConfig(SPI1, SPI_Enable_RX);
    SPI_BiDirectionalLineConfig(SPI1, SPI_Enable_TX);

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_0);   /* NSS  */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_0);    /* SCLK */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_0);    /* MOSI */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_3);    /* MISO */

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_RxData_Polling(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0, Data = 0;

    for (i = 0; i < Length; i++)
    {
        SPI_SendData(SPI1, Data);

        while (RESET == SPI_GetFlagStatus(SPI1, SPI_FLAG_TXEPT))
        {
        }

        while (RESET == SPI_GetFlagStatus(SPI1, SPI_FLAG_RXAVL))
        {
        }

        Buffer[i] = SPI_ReceiveData(SPI1);
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
        SPI_SendData(SPI1, Buffer[i]);

        while (RESET == SPI_GetFlagStatus(SPI1, SPI_FLAG_TXEPT))
        {
        }

        while (RESET == SPI_GetFlagStatus(SPI1, SPI_FLAG_RXAVL))
        {
        }

        SPI_ReceiveData(SPI1);
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_ReadBuffer(uint8_t *Buffer, uint8_t Length)
{
    if (Length)
    {
        SPI_FLASH_CS_L();
        SPI_RxData_Polling(Buffer, Length);
        SPI_FLASH_CS_H();
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_WriteBuffer(uint8_t *Buffer, uint8_t Length)
{
    if (Length)
    {
        SPI_FLASH_CS_L();
        SPI_TxData_Polling(Buffer, Length);
        SPI_FLASH_CS_H();
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_ShowOperationTips(void)
{
    printf("\r\n------------------------------");
    printf("\r\nK3 : write");
    printf("\r\nK4 : read ");
    printf("\r\n------------------------------");
    printf("\r\n");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SPI_Master_Polling_Sample(void)
{
    uint8_t i;
    uint8_t rBuffer[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t wBuffer[10] = {0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA};

    printf("\r\nTest %s", __FUNCTION__);

    K3_PressFlag = 0;
    K4_PressFlag = 0;

    EXTI_Configure();

    SPI_Configure();

    SPI_ShowOperationTips();

    while (1)
    {
        if (K3_PressFlag == 1)
        {
            K3_PressFlag = 0;

            printf("\r\nWrite...");

            SPI_WriteBuffer(wBuffer, sizeof(wBuffer));

            printf("\r\nOK");

            SPI_ShowOperationTips();
        }

        if (K4_PressFlag == 1)
        {
            K4_PressFlag = 0;

            printf("\r\nRead...");

            SPI_ReadBuffer(rBuffer, sizeof(rBuffer));

            printf("\r\nOK : ");

            for(i = 0; i < sizeof(rBuffer); i++)
            {
                printf("0x%02X ", rBuffer[i]);
            }

            SPI_ShowOperationTips();
        }

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


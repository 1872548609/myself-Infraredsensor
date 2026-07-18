/**
 * @file    spi_master_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _SPI_MASTER_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "spi_master_polling.h"

/**
  * @addtogroup MG32F003_RegSamples
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
void EXTI_Configure(void)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Enable SYSCFG Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SYSCFG);

    /* Config KEY3(PA8) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, 0x00U << GPIO_CRH_MODE8_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8,  0x02U << GPIO_CRH_CNF8_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS8);

    /* EXTI Line8 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR3, SYSCFG_EXTICR3_EXTI8, 0x00U << SYSCFG_EXTICR3_EXTI8_Pos);
    /* Enable EXTI Line8 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR8);
    /* EXTI Line8 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR8);

    /* Config KEY4(PA3) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE3, 0x00U << GPIO_CRL_MODE3_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF3,  0x02U << GPIO_CRL_CNF3_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS3);

    /* EXTI Line3 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR1, SYSCFG_EXTICR1_EXTI3, 0x00U << SYSCFG_EXTICR1_EXTI3_Pos);
    /* Enable EXTI Line3 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR3);
    /* EXTI Line3 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR3);

    NVIC_SetPriority(EXTI2_3_IRQn, 1);
    NVIC_EnableIRQ(EXTI2_3_IRQn);

    NVIC_SetPriority(EXTI4_15_IRQn, 1);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

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

    /* Config PA5 AF0 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR5, 0x00U << GPIO_AFRL_AFR5_Pos);   /* SPI1_SCK */

    /* Config PA5 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE5, 0x01U << GPIO_CRL_MODE5_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF5,  0x02U << GPIO_CRL_CNF5_Pos);

    /* Config PA6 AF0 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR6, 0x00U << GPIO_AFRL_AFR6_Pos);   /* SPI1_MOSI */

    /* Config PA6 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE6, 0x01U << GPIO_CRL_MODE6_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF6,  0x02U << GPIO_CRL_CNF6_Pos);

    /* Config PA0 AF0 */
    MODIFY_REG(GPIOA->AFRL, GPIO_AFRL_AFR0, 0x00U << GPIO_AFRL_AFR0_Pos);   /* SPI1_NSS */

    /* Config PA0 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE0, 0x01U << GPIO_CRL_MODE0_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF0,  0x02U << GPIO_CRL_CNF0_Pos);

    /* SPI Enable */
    SET_BIT(SPI1->GCTL, SPI_GCTL_SPIEN);
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
        /* Transmission Data */
        WRITE_REG(SPI1->TXREG, Data);

        /* Wait Transmission Buffer And Shift Register Both Are Empty */
        while (0 == READ_BIT(SPI1->CSTAT, SPI_CSTAT_TXEPT))
        {
        }

        /* Wait Receipt Buffer Is Not Empty */
        while (0 == READ_BIT(SPI1->CSTAT, SPI_CSTAT_RXAVL))
        {
        }

        /* Receipt Data */
        Buffer[i] = READ_REG(SPI1->RXREG);
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
    uint8_t i = 0, Data = 0;

    for (i = 0; i < Length; i++)
    {
        /* Transmission Data */
        WRITE_REG(SPI1->TXREG, Buffer[i]);

        /* Wait Transmission Buffer And Shift Register Both Are Empty */
        while (0 == READ_BIT(SPI1->CSTAT, SPI_CSTAT_TXEPT))
        {
        }

        /* Wait Receipt Buffer Is Not Empty */
        while (0 == READ_BIT(SPI1->CSTAT, SPI_CSTAT_RXAVL))
        {
        }

        /* Receipt Data */
        Data = READ_REG(SPI1->RXREG);

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
    uint8_t i, Buffer[10];

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

            SPI_WriteBuffer(Buffer, sizeof(Buffer));

            printf("\r\nOK");

            SPI_ShowOperationTips();
        }

        if (K4_PressFlag == 1)
        {
            K4_PressFlag = 0;

            printf("\r\nRead...");

            SPI_ReadBuffer(Buffer, sizeof(Buffer));

            printf("\r\nOK : ");

            for(i = 0; i < sizeof(Buffer); i++)
            {
                printf("0x%02X ", Buffer[i]);
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


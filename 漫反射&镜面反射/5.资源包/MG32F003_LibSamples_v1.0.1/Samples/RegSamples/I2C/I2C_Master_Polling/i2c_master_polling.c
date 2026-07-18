/**
 * @file    i2c_master_polling.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_MASTER_POLLING_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_master_polling.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_Polling
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
void EXTI_Configure(void)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Enable SYSCFG Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SYSCFG);

    /* Config KEY1(PA2) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE2, 0x00U << GPIO_CRL_MODE2_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF2,  0x02U << GPIO_CRL_CNF2_Pos);
    /* Pull-Down */
    WRITE_REG(GPIOA->BRR, GPIO_BSRR_BR2);

    /* EXTI Line2 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR1, SYSCFG_EXTICR1_EXTI2, 0x00U << SYSCFG_EXTICR1_EXTI2_Pos);
    /* Enable EXTI Line2 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR2);
    /* EXTI Line2 Rising Trigger */
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR2);

    /* Config KEY2(PA9) Input Pull-Up/Pull-Down Mode */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE9, 0x00U << GPIO_CRH_MODE9_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF9,  0x02U << GPIO_CRH_CNF9_Pos);
    /* Pull-Up */
    WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS9);

    /* EXTI Line9 Bound To PortA */
    MODIFY_REG(SYSCFG->EXTICR3, SYSCFG_EXTICR3_EXTI9, 0x00U << SYSCFG_EXTICR3_EXTI9_Pos);
    /* Enable EXTI Line9 Interrupt */
    SET_BIT(EXTI->IMR, EXTI_IMR_IMR9);
    /* EXTI Line9 Falling Trigger */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR9);

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
    MODIFY_REG(I2C1->TAR, I2C_TAR_ADDR, 0x58 << I2C_TAR_ADDR_Pos);

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
void I2C_WriteBuffer(uint8_t *Buffer, uint8_t Length)
{
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
void I2C_ReadBuffer(uint8_t *Buffer, uint8_t Length)
{
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
void I2C_ShowOperationTips(void)
{
    printf("\r\n------------------------------");
    printf("\r\nK1 : write");
    printf("\r\nK2 : read ");
    printf("\r\n------------------------------");
    printf("\r\n");
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_Master_Polling_Sample(void)
{
    uint8_t i, Buffer[10];

    printf("\r\nTest %s", __FUNCTION__);

    K1_PressFlag = 0;
    K2_PressFlag = 0;

    EXTI_Configure();

    I2C_Configure(100000);

    I2C_ShowOperationTips();

    while (1)
    {
        if (K1_PressFlag == 1)
        {
            K1_PressFlag = 0;

            printf("\r\nWrite...");

            I2C_WriteBuffer(Buffer, sizeof(Buffer));

            printf("\r\nOK");

            I2C_ShowOperationTips();
        }

        if (K2_PressFlag == 1)
        {
            K2_PressFlag = 0;

            printf("\r\nRead...");

            I2C_ReadBuffer(Buffer, sizeof(Buffer));

            printf("\r\nOK : ");

            for(i = 0; i < sizeof(Buffer); i++)
            {
                printf("0x%02X ", Buffer[i]);
            }

            I2C_ShowOperationTips();
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


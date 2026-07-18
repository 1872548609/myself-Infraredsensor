/**
 * @file    i2c_master_interrupt.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _I2C_MASTER_INTERRUPT_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "i2c_master_interrupt.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup I2C
  * @{
  */

/**
  * @addtogroup I2C_Master_Interrupt
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
    EXTI_InitTypeDef EXTI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SYSCFG, ENABLE);

    /* K1->PA2->EXTI_Line2 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line2;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* K2->PA9->EXTI_Line9 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);

    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line    = EXTI_Line9;
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
void I2C_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef  I2C_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C1, ENABLE);

    I2C_DeInit(I2C1);

    I2C_StructInit(&I2C_InitStruct);
    I2C_InitStruct.I2C_Mode       = I2C_Mode_Master;
    I2C_InitStruct.I2C_OwnAddress = 0x00;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_Init(I2C1, &I2C_InitStruct);

    I2C_TargetAddressConfig(I2C1, 0xB0);

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_3);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_3);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = I2C1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    I2C_Cmd(I2C1, ENABLE);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_TxData_Interrupt(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i = 0;

    for (i = 0; i < Length; i++)
    {
        I2C_TxStruct.Buffer[i] = Buffer[i];
    }

    I2C_TxStruct.Length = Length;
    I2C_TxStruct.CurrentCount = 0;
    I2C_TxStruct.CompleteFlag = 0;

    I2C_ClearFlag(I2C1, I2C_FLAG_TX_EMPTY);
    I2C_ITConfig(I2C1, I2C_IT_TX_EMPTY, ENABLE);

    while (0 == I2C_TxStruct.CompleteFlag)
    {
    }
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void I2C_RxData_Interrupt(uint8_t *Buffer, uint16_t Length)
{
    uint8_t i = 0;

    I2C_RxStruct.Length = Length;
    I2C_RxStruct.CurrentCount = 0;
    I2C_RxStruct.CompleteFlag = 0;

    I2C_ClearFlag(I2C1, I2C_FLAG_RX_FULL);
    I2C_ITConfig(I2C1, I2C_IT_RX_FULL, ENABLE);

    I2C_ReadCmd(I2C1);

    while (0 == I2C_RxStruct.CompleteFlag)
    {
    }

    for (i = 0; i < Length; i++)
    {
        Buffer[i] = I2C_RxStruct.Buffer[i];
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
    I2C_TxData_Interrupt((uint8_t *)Buffer, Length);

    while (RESET == I2C_GetFlagStatus(I2C1, I2C_STATUS_FLAG_TFE))
    {
    }

    I2C_GenerateSTOP(I2C1, ENABLE);

    while (RESET == I2C_GetITStatus(I2C1, I2C_IT_STOP_DET))
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
    I2C_RxData_Interrupt((uint8_t *)Buffer, Length);

    I2C_GenerateSTOP(I2C1, ENABLE);

    while (!I2C_GetITStatus(I2C1, I2C_IT_STOP_DET))
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
void I2C_Master_Interrupt_Sample(void)
{
    uint8_t i, Buffer[10];

    printf("\r\nTest %s", __FUNCTION__);

    K1_PressFlag = 0;
    K2_PressFlag = 0;

    EXTI_Configure();

    I2C_Configure();

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


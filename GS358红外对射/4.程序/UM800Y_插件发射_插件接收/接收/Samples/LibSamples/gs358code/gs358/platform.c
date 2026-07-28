/**
 * @file    platform.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PLATFORM_C_

/* Files include */
#include <stdio.h>
#include "platform.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup MG32
  * @{
  */

/**
  * @addtogroup PLATFORM
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  Initialize SysTick for delay function
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_InitDelay(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
    {
        while (1)
        {
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 0x0);
}

/***********************************************************************************************************************
  * @brief  Millisecond delay
  * @note   none
  * @param  Millisecond: delay time unit
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_DelayMS(uint32_t Millisecond)
{
    PLATFORM_DelayTick = Millisecond;

    while (0 != PLATFORM_DelayTick)
    {
    }
}

/***********************************************************************************************************************
  * @brief  Initialize console for printf
  * @note   none
  * @param  Baudrate : USART1 communication baudrate
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_InitConsole(uint32_t Baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART1, ENABLE);

    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = Baudrate;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity   = USART_Parity_No;
    USART_InitStruct.USART_Mode     = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

#if   defined (__ICCARM__)

#if   (__VER__ >= 9030001)

/* Files include */
#include <stddef.h>
#include <LowLevelIOInterface.h>

/***********************************************************************************************************************
  * @brief  redefine __write function
  * @note   for printf
  * @param  handle
  * @param  *buf
  * @param  bufSize
  * @retval nChars
  *********************************************************************************************************************/
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
    size_t nChars = 0;

     /* Check for the command to flush all handles */
    if (-1 == handle)
    {
        return (0);
    }

     /* Check for stdout and stderr (only necessary if FILE descriptors are enabled.) */
    if ((_LLIO_STDOUT != handle) && (_LLIO_STDERR != handle))
    {
        return (-1);
    }

    for (/* Empty */; bufSize > 0; --bufSize)
    {
        USART_SendData(USART1, *buf);

        while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC))
        {
        }

        ++buf;
        ++nChars;
    }

    return (nChars);
}

#else

/***********************************************************************************************************************
  * @brief  redefine fputc function
  * @note   for printf
  * @param  ch
  * @param  f
  * @retval ch
  *********************************************************************************************************************/
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);

    while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC))
    {
    }

    return (ch);
}

#endif

#elif defined (__GNUC__)

/***********************************************************************************************************************
  * @brief  redefine fputc function
  * @note   for printf
  * @param  ch
  * @param  f
  * @retval ch
  *********************************************************************************************************************/
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);

    while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC))
    {
    }

    return (ch);
}

#else

/***********************************************************************************************************************
  * @brief  redefine fputc function
  * @note   for printf
  * @param  ch
  * @param  f
  * @retval ch
  *********************************************************************************************************************/
int fputc(int ch, FILE *f)
{
    USART_SendData(USART2, (uint8_t)ch);

    while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
    {
    }

    return (ch);
}

#endif

/***********************************************************************************************************************
  * @brief  Initialize LED GPIO pin
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_InitLED(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);

    PLATFORM_LED_Enable(LED1, DISABLE);
    PLATFORM_LED_Enable(LED2, DISABLE);
    PLATFORM_LED_Enable(LED3, DISABLE);
    PLATFORM_LED_Enable(LED4, DISABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/***********************************************************************************************************************
  * @brief  LED on or off
  * @note   none
  * @param  LEDn : LED index
  * @arg    LED1, LED2, LED3, LED4
  * @param  State
  * @arg    ENABLE, DISABLE
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_LED_Enable(LEDn_TypeDef LEDn, FunctionalState State)
{
    switch (LEDn)
    {
        case LED1:
            GPIO_WriteBit(GPIOA, GPIO_Pin_15, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        case LED2:
            GPIO_WriteBit(GPIOA, GPIO_Pin_10, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        case LED3:
            GPIO_WriteBit(GPIOA, GPIO_Pin_6, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        case LED4:
            GPIO_WriteBit(GPIOA, GPIO_Pin_5, (ENABLE == State) ? Bit_RESET : Bit_SET);
            break;

        default:
            break;
    }
}

/***********************************************************************************************************************
  * @brief  LED toggle display
  * @note   none
  * @param  LEDn : LED index
  * @arg    LED1, LED2, LED3, LED4
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_LED_Toggle(LEDn_TypeDef LEDn)
{
    switch (LEDn)
    {
        case LED1:
            GPIO_WriteBit(GPIOA, GPIO_Pin_15, GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15) ? Bit_RESET : Bit_SET);
            break;

        case LED2:
            GPIO_WriteBit(GPIOA, GPIO_Pin_10, GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_10) ? Bit_RESET : Bit_SET);
            break;

        case LED3:
            GPIO_WriteBit(GPIOA, GPIO_Pin_6, GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_6) ? Bit_RESET : Bit_SET);
            break;

        case LED4:
            GPIO_WriteBit(GPIOA, GPIO_Pin_5, GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) ? Bit_RESET : Bit_SET);
            break;

        default:
            break;
    }
}

/***********************************************************************************************************************
  * @brief  Print information
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_PrintInfo(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    printf("\r\nBOARD : Megawin_EV");
    printf("\r\nMCU : MG32F003");

    printf("\r\n");

    switch (READ_BIT(RCC->CFGR, RCC_CFGR_SWS))
    {
        case 0x00:
            printf("\r\nHSI/6 used as system clock source");
            break;

        case 0x04:
            printf("\r\nHSE used as system clock source");
            break;

        case 0x08:
            printf("\r\nHSI used as system clock source");
            break;

        case 0x0C:
            printf("\r\nLSI used as system clock source");
            break;

        default:
            break;
    }

    RCC_GetClocksFreq(&RCC_Clocks);

    printf("\r\n");
    printf("\r\nSYSCLK Frequency : %7.3f MHz", (double)RCC_Clocks.SYSCLK_Frequency / (double)1000000.0);
    printf("\r\nHCLK   Frequency : %7.3f MHz", (double)RCC_Clocks.HCLK_Frequency / (double)1000000.0);
    printf("\r\nPCLK1  Frequency : %7.3f MHz", (double)RCC_Clocks.PCLK1_Frequency / (double)1000000.0);
    printf("\r\n");
}

/***********************************************************************************************************************
  * @brief  Initialize Platform
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_Init(void)
{
    /*
     * USART1 已专用于 TOF 通讯，USART2 已专用于 RS485 调试输出。
     * 因此这里不能再调用 PLATFORM_InitConsole()/PLATFORM_PrintInfo()，
     * 否则 printf 会抢占或错误初始化串口引脚。
     */
    PLATFORM_InitDelay();

    PLATFORM_InitLED();
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


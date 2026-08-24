/**
 * @file    platform.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _PLATFORM_C_

/* Files include */
#include <math.h>
#include <stdio.h>
#include "platform.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup RCC
  * @{
  */

/**
  * @addtogroup RCC_MCO
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  Get SYSCLK
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint32_t PLATFORM_GetSystemFrequency(void)
{
    uint32_t SYSCLK_Frequency = 0;

    switch (READ_BIT(RCC->CFGR, RCC_CFGR_SWS))
    {
        case 0x00:
            SYSCLK_Frequency = HSI_48MHz_DIV6;
            break;

        case 0x04:
            SYSCLK_Frequency = HSE_VALUE;
            break;

        case 0x08:
            SYSCLK_Frequency = HSI_48MHz;
            break;

        case 0x0C:
            SYSCLK_Frequency = LSI_VALUE;
            break;

        default:
            break;
    }

    return (SYSCLK_Frequency);
}

/***********************************************************************************************************************
  * @brief  Get HCLK
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint32_t PLATFORM_GetHCLKFrequency(void)
{
    uint32_t HCLK_Frequency = 0;
    uint32_t HPRE           = 0;

    HPRE = READ_BIT(RCC->CFGR, RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;

    if (HPRE < 8)
    {
        HCLK_Frequency = PLATFORM_GetSystemFrequency();
    }
    else
    {
        HCLK_Frequency = PLATFORM_GetSystemFrequency() / pow(2, HPRE - 7);
    }

    return (HCLK_Frequency);
}

/***********************************************************************************************************************
  * @brief  Get PCLK1
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
uint32_t PLATFORM_GetPCLK1Frequency(void)
{
    uint32_t PCLK1_Frequency = 0;
    uint32_t PPRE1           = 0;

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        PCLK1_Frequency = PLATFORM_GetHCLKFrequency();
    }
    else
    {
        PCLK1_Frequency = PLATFORM_GetHCLKFrequency() / pow(2, PPRE1 - 3);
    }

    return (PCLK1_Frequency);
}

/***********************************************************************************************************************
  * @brief  Initialize SysTick for delay function
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PLATFORM_InitDelay(void)
{
    if (SysTick_Config(48000000U / 1000))
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
    uint32_t PCLK1_Frequency = 0;

    PCLK1_Frequency = PLATFORM_GetPCLK1Frequency();

    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA10 AF4 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR10, 0x04U << GPIO_AFRH_AFR10_Pos);

    /* Config PA10 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE10, 0x01U << GPIO_CRH_MODE10_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10,  0x02U << GPIO_CRH_CNF10_Pos);

    /* Enable USART1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART1);

    /* Parity : None */
    CLEAR_BIT(USART1->CR1, USART_CR1_PCE);
    CLEAR_BIT(USART1->CR1, USART_CR1_PS);

    /* Data Length : 8-bit */
    CLEAR_BIT(USART1->CR1, USART_CR1_DL);

    /* Stop Bit : 1-bit */
    MODIFY_REG(USART1->CR2, USART_CR2_STOP, 0x00U << USART_CR2_STOP_Pos);

    /* Baudrate */
    MODIFY_REG(USART1->BRR, USART_BRR_MFD, ((PCLK1_Frequency / Baudrate) / 16) << USART_BRR_MFD_Pos);
    MODIFY_REG(USART1->BRR, USART_BRR_FFD, ((PCLK1_Frequency / Baudrate) % 16) << USART_BRR_FFD_Pos);

    /* Enable Transmit */
    SET_BIT(USART1->CR1, USART_CR1_TE);

    /* Enable USART1 */
    SET_BIT(USART1->CR1, USART_CR1_UE);
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
        USART1->DR = *buf;

        while (0 == READ_BIT(USART1->SR, USART_SR_TC))
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
    USART1->DR = (uint8_t)ch;

    while (0 == READ_BIT(USART1->SR, USART_SR_TC))
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
    USART1->DR = (uint8_t)ch;

    while (0 == READ_BIT(USART1->SR, USART_SR_TC))
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
    USART1->DR = (uint8_t)ch;

    while (0 == READ_BIT(USART1->SR, USART_SR_TC))
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
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    PLATFORM_LED_Enable(LED1, DISABLE);
    PLATFORM_LED_Enable(LED2, DISABLE);
    PLATFORM_LED_Enable(LED3, DISABLE);
    PLATFORM_LED_Enable(LED4, DISABLE);

    /* Config LED1(PA15) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE15, 0x01U << GPIO_CRH_MODE15_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF15,  0x00U << GPIO_CRH_CNF15_Pos);

    /* Config LED2(PA10) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE10, 0x01U << GPIO_CRH_MODE10_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10,  0x00U << GPIO_CRH_CNF10_Pos);

    /* Config LED3(PA6) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE6,  0x01U << GPIO_CRL_MODE6_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF6,   0x00U << GPIO_CRL_CNF6_Pos);

    /* Config LED4(PA5) General Purpose Output Push-Pull */
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE5,  0x01U << GPIO_CRL_MODE5_Pos);
    MODIFY_REG(GPIOA->CRL, GPIO_CRL_CNF5,   0x00U << GPIO_CRL_CNF5_Pos);
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
            if (ENABLE == State)
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR15);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS15);
            }
            break;

        case LED2:
            if (ENABLE == State)
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR10);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS10);
            }
            break;

        case LED3:
            if (ENABLE == State)
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR6);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6);
            }
            break;

        case LED4:
            if (ENABLE == State)
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR5);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5);
            }
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
            if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR15))
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR15);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS15);
            }
            break;

        case LED2:
            if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR10))
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR10);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS10);
            }
            break;

        case LED3:
            if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR6))
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR6);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6);
            }
            break;

        case LED4:
            if (READ_BIT(GPIOA->ODR, GPIO_ODR_ODR5))
            {
                WRITE_REG(GPIOA->BRR, GPIO_BRR_BR5);
            }
            else
            {
                WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5);
            }
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

    printf("\r\n");
    printf("\r\nSYSCLK Frequency : %7.3f MHz", (double)PLATFORM_GetSystemFrequency() / (double)1000000.0);
    printf("\r\nHCLK   Frequency : %7.3f MHz", (double)PLATFORM_GetHCLKFrequency()   / (double)1000000.0);
    printf("\r\nPCLK1  Frequency : %7.3f MHz", (double)PLATFORM_GetPCLK1Frequency()  / (double)1000000.0);
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
    PLATFORM_InitDelay();

    PLATFORM_InitLED();

    PLATFORM_InitConsole(115200);

    PLATFORM_PrintInfo();
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


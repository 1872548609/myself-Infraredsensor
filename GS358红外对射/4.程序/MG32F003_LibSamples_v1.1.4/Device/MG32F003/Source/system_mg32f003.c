/**
 * @file    system_mg32f003.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    11-March-2025
 * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Source File.
 */

/* Files includes ----------------------------------------------------------------------------------------------------*/
#include "mg32f003_device.h"

/*
 *  Only one of SYSCLK_HSI_XXMHz and SYSCLK_HSE_XXMHz can be defined at a time.
 *  When HSI(HSI_VALUE =  48MHz) is used as the clock source, SYSCLK_HSI_XXMHz is used.
 *  When HSE is used as the clock source, SYSCLK_HSE_XXMHz is used.
 */

#define SYSCLK_HSI_XXMHz  48000000     /*Do not change this value if hsi is used as the system clock */
// #define SYSCLK_HSE_XXMHz    HSE_VALUE  /*Modify HSE_VALUE in mg32f003.h */

/* Clock Definitions -------------------------------------------------------------------------------------------------*/
/* System Clock Frequency Select */
#if defined SYSCLK_HSE_XXMHz
uint32_t SystemCoreClock         = SYSCLK_HSE_XXMHz; /*!< HSE Selected as System Clock source */

#elif defined SYSCLK_HSI_XXMHz
uint32_t SystemCoreClock         = SYSCLK_HSI_XXMHz; /*!< HSI Selected as System Clock source */
#endif

#define RCC_CFGR_HPRE_DIV1              (0x00U << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV2              (0x08U << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV4              (0x09U << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV8              (0x0AU << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV16             (0x0BU << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV64             (0x0CU << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV128            (0x0DU << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV256            (0x0EU << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV512            (0x0FU << RCC_CFGR_HPRE_Pos)

#define RCC_CFGR_PPRE1_DIV1             (0x00U << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV2             (0x04U << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV4             (0x05U << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV8             (0x06U << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV16            (0x07U << RCC_CFGR_PPRE1_Pos)

#define RCC_CFGR_SW_HSI_DIV6            (0x00U << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSE                 (0x01U << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSI                 (0x02U << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_LSI                 (0x03U << RCC_CFGR_SW_Pos)

#define RCC_CFGR_SWS_HSI_DIV6           (0x00U << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_HSE                (0x01U << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_HSI                (0x02U << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_LSI                (0x03U << RCC_CFGR_SWS_Pos)

/**
  * @brief  Configures the System clock frequency, HCLK, and PCLK1 prescalers.
  * @param  None
  * @retval None
  */
static void SetSysClockToDefine(void)
{
    __IO uint32_t StartUpCounter = 0, ClkSrcStatus = 1;
    uint32_t temp = 0;

#ifdef SYSCLK_HSE_XXMHz
     /* Enable HSE */
    RCC->CR |= (0x01U << RCC_CR_HSEON_Pos);

     /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        ClkSrcStatus = RCC->CR & RCC_CR_HSERDY_Msk;
        StartUpCounter++;
    }
    while((ClkSrcStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY_Msk) != RESET)
    {
        ClkSrcStatus = (uint32_t)0x01;
    }
    else
    {
        ClkSrcStatus = (uint32_t)0x00;
    }

    SystemCoreClock = SYSCLK_HSE_XXMHz;

    if (ClkSrcStatus == (uint32_t)0x01)
    {
         /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
         /* Flash 0 wait state, bit0~2 */
        FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;

         /* HCLK = SYSCLK */
        temp      = RCC->CFGR;
        temp     &= ~RCC_CFGR_HPRE_Msk;
        temp     |= RCC_CFGR_HPRE_DIV1;
        RCC->CFGR = temp;

         /* PCLK1 = HCLK */
        temp      = RCC->CFGR;
        temp     &= ~RCC_CFGR_PPRE1_Msk;
        temp     |= RCC_CFGR_PPRE1_DIV1;
        RCC->CFGR = temp;

         /* Select HSI as system clock source */
        RCC->CFGR &= ~RCC_CFGR_SW_Msk;
        RCC->CFGR |= RCC_CFGR_SW_HSE;

         /* Wait till HSE is used as system clock source */
        while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_HSE)
        {
            __ASM("nop");              /* __NOP(); */
        }
    }
    else
    {
        /* If HSE not ready within the given time, the program will stop here.
           User can add here some code to deal with this error */
        while (1)
        {
            /* please check Whether the crystal oscillator starts*/
        }
    }
#elif defined SYSCLK_HSI_XXMHz
    RCC->CR |= (0x01U << RCC_CR_HSION_Pos);

    while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
    {
    }

    SystemCoreClock = SYSCLK_HSI_XXMHz;

     /* RCC->CFGR = RCC_CFGR_PPRE1_2; */
    FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
    FLASH->ACR |= FLASH_ACR_PRFTBE_Msk;
    temp = (SystemCoreClock - 1) / 24000000;

    if (temp > 1)
    {
        temp = 1;
    }

    FLASH->ACR |= temp;

     /* HCLK = SYSCLK */
    temp      = RCC->CFGR;
    temp     &= ~RCC_CFGR_HPRE_Msk;
    temp     |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR = temp;

     /* PCLK1 = HCLK */
    temp      = RCC->CFGR;
    temp     &= ~RCC_CFGR_PPRE1_Msk;
    temp     |= RCC_CFGR_PPRE1_DIV1;
    RCC->CFGR = temp;

     /* Select HSI as system clock source */
    RCC->CFGR &= ~RCC_CFGR_SW_Msk;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

     /* Wait till HSI is used as system clock source */
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_HSI)
    {
        __ASM("nop");                  /* __NOP(); */
    }
#endif
}

/**
  * @brief  Setup the microcontroller system Initialize the Embedded Flash Interface,
  *         SystemCoreClock variable.
  * @note   This function should be used only after reset.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
     /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
     /* Set HSION bit */
    RCC->CR |= (0x01U << RCC_CR_HSION_Pos);

     /* Reset SW, HPRE, PPRE1, PPRE2 and MCO bits */
    RCC->CFGR &= ~(RCC_CFGR_SW_Msk | RCC_CFGR_HPRE_Msk | RCC_CFGR_PPRE1_Msk | RCC_CFGR_MCO_Msk);

     /* Reset HSEON */
    RCC->CR &= ~RCC_CR_HSEON_Msk;

     /* Disable all interrupts and clear pending bits */
    RCC->CIR = 0xFFFFFFFF;
    RCC->CIR = 0;

     /* Configure the System clock frequency, HCLK, PCLK1 prescalers */
     /* Configure the Flash Latency cycles and enable prefetch buffer */
    SetSysClockToDefine();
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


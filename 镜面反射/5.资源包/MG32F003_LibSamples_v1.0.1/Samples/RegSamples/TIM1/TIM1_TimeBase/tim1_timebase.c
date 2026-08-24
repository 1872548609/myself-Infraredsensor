/**
 * @file    tim1_timebase.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM1_TIMEBASE_C_

/* Files include */
#include <math.h>
#include <stdio.h>
#include "platform.h"
#include "tim1_timebase.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup TIM1
  * @{
  */

/**
  * @addtogroup TIM1_TimeBase
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
uint32_t TIM1_GetFrequency(void)
{
    uint32_t TIM1_Frequency       = 0;
    uint32_t HPRE    = 0, PPRE1   = 0;
    uint32_t AHB_DIV = 0, APB_DIV = 0;

    HPRE = READ_BIT(RCC->CFGR, RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos;

    if (HPRE < 8)
    {
        AHB_DIV = 1;
    }
    else
    {
        AHB_DIV = pow(2, HPRE % 7);
    }

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        APB_DIV = 1;
    }
    else
    {
        APB_DIV = pow(2, PPRE1 % 3);
    }

    if ((AHB_DIV == 1) && (APB_DIV == 1))
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency();
    }
    else if ((AHB_DIV != 1) && (APB_DIV == 1))
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / AHB_DIV;
    }
    else if ((AHB_DIV == 1) && (APB_DIV != 1))
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / APB_DIV;
    }
    else
    {
        TIM1_Frequency = PLATFORM_GetPCLK1Frequency() * 4 / AHB_DIV / APB_DIV;
    }

    return (TIM1_Frequency);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_Configure(void)
{
    /* Enable TIM1 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM1);

    /* Config TIM1 Clock Division */
    MODIFY_REG(TIM1->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM1 Edge Alignment Mode */
    MODIFY_REG(TIM1->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM1 Up Count Direction */
    MODIFY_REG(TIM1->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM1 Auto-Reload Value */
    WRITE_REG(TIM1->ARR, 5000 - 1);

    /* Config TIM1 Prescaler Value */
    WRITE_REG(TIM1->PSC, TIM1_GetFrequency() / 10000 - 1);

    /* Enable Update Interrupt */
    SET_BIT(TIM1->DIER, TIM_DIER_UIE);

    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    /* Enable TIM1 */
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM1_TimeBase_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM1_UpdateFlag = 0;

    TIM1_Configure();

    while (1)
    {
        if (0 != TIM1_UpdateFlag)
        {
            TIM1_UpdateFlag = 0;

            PLATFORM_LED_Toggle(LED1);
        }
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


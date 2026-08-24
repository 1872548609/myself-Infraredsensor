/**
 * @file    tim14_timebase.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _TIM14_TIMEBASE_C_

/* Files include */
#include <math.h>
#include <stdio.h>
#include "platform.h"
#include "tim14_timebase.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup TIM14
  * @{
  */

/**
  * @addtogroup TIM14_TimeBase
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
uint32_t TIM14_GetFrequency(void)
{
    uint32_t TIM14_Frequency    = 0;
    uint32_t PPRE1 = 0, APB_DIV = 0;

    PPRE1 = READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;

    if (PPRE1 < 4)
    {
        APB_DIV = 1;

        TIM14_Frequency = PLATFORM_GetPCLK1Frequency();
    }
    else
    {
        APB_DIV = pow(2, PPRE1 % 3);

        TIM14_Frequency = PLATFORM_GetPCLK1Frequency() * 2 / APB_DIV;
    }

    return (TIM14_Frequency);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM14_Configure(void)
{
    /* Enable TIM14 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM14);

    /* Config TIM14 Clock Division */
    MODIFY_REG(TIM14->CR1, TIM_CR1_CKD, 0x00U << TIM_CR1_CKD_Pos);

    /* Config TIM14 Edge Alignment Mode */
    MODIFY_REG(TIM14->CR1, TIM_CR1_CMS, 0x00U << TIM_CR1_CMS_Pos);

    /* Config TIM14 Up Count Direction */
    MODIFY_REG(TIM14->CR1, TIM_CR1_DIR, 0x00U << TIM_CR1_DIR_Pos);

    /* Config TIM14 Auto-Reload Value */
    WRITE_REG(TIM14->ARR, 5000 - 1);

    /* Config TIM14 Prescaler Value */
    WRITE_REG(TIM14->PSC, TIM14_GetFrequency() / 10000 - 1);

    /* Enable Update Interrupt */
    SET_BIT(TIM14->DIER, TIM_DIER_UIE);

    NVIC_SetPriority(TIM14_IRQn, 1);
    NVIC_EnableIRQ(TIM14_IRQn);

    /* Enable TIM14 */
    SET_BIT(TIM14->CR1, TIM_CR1_CEN);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM14_TimeBase_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    TIM14_UpdateFlag = 0;

    TIM14_Configure();

    while (1)
    {
        if (0 != TIM14_UpdateFlag)
        {
            TIM14_UpdateFlag = 0;

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


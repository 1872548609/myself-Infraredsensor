/**
 * @file    mg32f003_it.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _MG32F003_IT_C_

/* Files include */
#include "platform.h"
#include "tim3_pwm_inputcapture.h"
#include "mg32f003_it.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup Peripheral
  * @{
  */

/**
  * @addtogroup Peripheral_SampleFunction
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  This function handles NMI exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void NMI_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles Hard Fault exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/***********************************************************************************************************************
  * @brief  This function handles SVCall exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SVC_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles PendSVC exception
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void PendSV_Handler(void)
{
}

/***********************************************************************************************************************
  * @brief  This function handles SysTick Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void SysTick_Handler(void)
{
    if (0 != PLATFORM_DelayTick)
    {
        PLATFORM_DelayTick--;
    }
}

/***********************************************************************************************************************
  * @brief  This function handles TIM3 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void TIM3_IRQHandler(void)
{
    if (RESET != TIM_GetITStatus(TIM3, TIM_IT_CC1))
    {
        TIM3_CC_InterruptFlag = 1;

        TIM3_CC_Value1 = TIM_GetCapture1(TIM3);
        TIM3_CC_Value2 = TIM_GetCapture2(TIM3);

        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
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


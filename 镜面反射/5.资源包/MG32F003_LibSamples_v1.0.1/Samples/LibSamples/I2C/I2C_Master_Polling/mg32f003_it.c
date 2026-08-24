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
#include "i2c_master_polling.h"
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
  * @brief  This function handles EXTI2_3 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI2_3_IRQHandler(void)
{
    /* K1 */
    if (RESET != EXTI_GetITStatus(EXTI_Line2))
    {
        K1_PressFlag = 1;

        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

/***********************************************************************************************************************
  * @brief  This function handles EXTI4_15 Handler
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void EXTI4_15_IRQHandler(void)
{
    /* K2 */
    if (RESET != EXTI_GetITStatus(EXTI_Line9))
    {
        K2_PressFlag = 1;

        EXTI_ClearITPendingBit(EXTI_Line9);
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


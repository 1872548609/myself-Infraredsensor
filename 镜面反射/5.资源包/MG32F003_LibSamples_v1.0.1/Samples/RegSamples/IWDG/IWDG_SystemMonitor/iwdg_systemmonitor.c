/**
 * @file    iwdg_systemmonitor.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _IWDG_SYSTEMMONITOR_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "iwdg_systemmonitor.h"

/**
  * @addtogroup MG32F003_RegSamples
  * @{
  */

/**
  * @addtogroup IWDG
  * @{
  */

/**
  * @addtogroup IWDG_SystemMonitor
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
void IWDG_Configure(uint32_t Timeout)
{
    uint16_t Reload = LSI_VALUE / 1000 * Timeout / 32;

    /* Enable LSI Clock */
    SET_BIT(RCC->CSR, RCC_CSR_LSION);

    /* Wait LSI Ready */
    while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == 0)
    {
    }

    /* Wait Watchdog Prescaler Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU));
    /* Disable The Protection And Enable Access To Other Configuration Registers */
    WRITE_REG(IWDG->KR, 0x5555);
    /* IWDG Prescaler Divider */
    MODIFY_REG(IWDG->PR, IWDG_PR_PR, 0x03U << IWDG_PR_PR_Pos);
    /* Wait Watchdog Prescaler Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU));

    /* Wait Watchdog Counter Reload Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_RVU));
    /* Disable The Protection And Enable Access To Other Configuration Registers */
    WRITE_REG(IWDG->KR, 0x5555);
    /* Watchdog Counter Reload Value */
    MODIFY_REG(IWDG->RLR, IWDG_RLR_RL, Reload << IWDG_RLR_RL_Pos);
    /* Wait Watchdog Counter Reload Value Update Completed */
    while(READ_BIT(IWDG->SR, IWDG_SR_PVU));

    /* Feed The Dog */
    WRITE_REG(IWDG->KR, 0xAAAA);

    /* Start The Watchdog */
    WRITE_REG(IWDG->KR, 0xCCCC);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void IWDG_SystemMonitor_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    IWDG_Configure(500);

    while (1)
    {
        PLATFORM_LED_Toggle(LED1);
        PLATFORM_DelayMS(100);

        /* Feed The Dog */
        WRITE_REG(IWDG->KR, 0xAAAA);
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


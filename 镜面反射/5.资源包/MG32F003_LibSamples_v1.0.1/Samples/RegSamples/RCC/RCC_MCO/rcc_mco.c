/**
 * @file    rcc_mco.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _RCC_MCO_C_

/* Files include */
#include <stdio.h>
#include "platform.h"
#include "rcc_mco.h"

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
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void RCC_Configure(void)
{
    /* Enable GPIOA Clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOA);

    /* Config PA10 AF3 */
    MODIFY_REG(GPIOA->AFRH, GPIO_AFRH_AFR10, 0x03U << GPIO_AFRH_AFR10_Pos);

    /* Config PA10 Alternate Function Output Push-Pull */
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE10, 0x01U << GPIO_CRH_MODE10_Pos);
    MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF10,  0x02U << GPIO_CRH_CNF10_Pos);

    /* Micro Controller Clock Output : LSI */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, 0x02U << RCC_CFGR_MCO_Pos);
}

/***********************************************************************************************************************
  * @brief
  * @note   none
  * @param  none
  * @retval none
  *********************************************************************************************************************/
void RCC_MCO_Sample(void)
{
    printf("\r\nTest %s", __FUNCTION__);

    RCC_Configure();

    while (1)
    {
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


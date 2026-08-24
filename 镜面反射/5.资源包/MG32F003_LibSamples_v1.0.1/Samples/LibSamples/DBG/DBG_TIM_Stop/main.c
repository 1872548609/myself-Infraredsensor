/**
 * @file    main.c
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    17-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#define _MAIN_C_

/* Files include */
#include "platform.h"
#include "dbg_tim_stop.h"
#include "main.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup DBG
  * @{
  */

/**
  * @addtogroup DBG_TIM_Stop
  * @{
  */

/* Private typedef ****************************************************************************************************/

/* Private define *****************************************************************************************************/

/* Private macro ******************************************************************************************************/

/* Private variables **************************************************************************************************/

/* Private functions **************************************************************************************************/

/***********************************************************************************************************************
  * @brief  This function is main entrance
  * @note   main
  * @param  none
  * @retval none
  *********************************************************************************************************************/
int main(void)
{
    PLATFORM_Init();

    DBG_TIM_Stop_Sample();

    while (1)
    {
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


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
#include "tim3_timebase.h"
#include "main.h"

/**
  * @addtogroup MG32F003_LibSamples
  * @{
  */

/**
  * @addtogroup TIM3
  * @{
  */

/**
  * @addtogroup TIM3_TimeBase
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

    TIM3_TimeBase_Sample();

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


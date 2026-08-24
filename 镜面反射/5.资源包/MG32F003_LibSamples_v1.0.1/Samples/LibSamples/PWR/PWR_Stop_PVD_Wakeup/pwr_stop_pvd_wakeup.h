/**
 * @file    pwr_stop_pvd_wakeup.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _PWR_STOP_PVD_WAKEUP_H_
#define _PWR_STOP_PVD_WAKEUP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32_conf.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/

/* Exported functions *************************************************************************************************/
void SystemInit(void);
void PWR_Stop_PVD_Wakeup_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _PWR_STOP_PVD_WAKEUP_H_ */


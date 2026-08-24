/**
 * @file    iwdg_systemmonitor.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _IWDG_SYSTEMMONITOR_H_
#define _IWDG_SYSTEMMONITOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32f003_device.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/

/* Exported functions *************************************************************************************************/
void IWDG_SystemMonitor_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _IWDG_SYSTEMMONITOR_H_ */


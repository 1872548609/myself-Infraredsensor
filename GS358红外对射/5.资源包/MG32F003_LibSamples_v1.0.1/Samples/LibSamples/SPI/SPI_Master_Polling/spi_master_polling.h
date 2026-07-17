/**
 * @file    spi_master_polling.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _SPI_MASTER_POLLING_H_
#define _SPI_MASTER_POLLING_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32_conf.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _SPI_MASTER_POLLING_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint8_t K3_PressFlag;
EXTERN volatile uint8_t K4_PressFlag;

/* Exported functions *************************************************************************************************/
void SPI_Master_Polling_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_MASTER_POLLING_H_ */


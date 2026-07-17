/**
 * @file    spi_slave_interrupt.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _SPI_SLAVE_INTERRUPT_H_
#define _SPI_SLAVE_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32_conf.h"

/* Exported types *****************************************************************************************************/
typedef struct
{
    uint8_t Buffer[255];
    uint8_t Length;
    uint8_t CurrentCount;
    uint8_t CompleteFlag;
} SPI_RxTx_TypeDef;

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _SPI_SLAVE_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile SPI_RxTx_TypeDef SPI_RxStruct;
EXTERN volatile SPI_RxTx_TypeDef SPI_TxStruct;

/* Exported functions *************************************************************************************************/
void SPI_Slave_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_SLAVE_INTERRUPT_H_ */


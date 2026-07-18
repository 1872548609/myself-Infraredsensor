/**
 * @file    i2c_master_eeprom_interrupt.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _I2C_MASTER_EEPROM_INTERRUPT_H_
#define _I2C_MASTER_EEPROM_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32f003_device.h"

/* Exported types *****************************************************************************************************/
typedef struct
{
    uint8_t Buffer[255];
    uint8_t Length;
    uint8_t CurrentCount;
    uint8_t CompleteFlag;
} I2C_RxTx_TypeDef;

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _I2C_MASTER_EEPROM_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile I2C_RxTx_TypeDef I2C_RxStruct;
EXTERN volatile I2C_RxTx_TypeDef I2C_TxStruct;

/* Exported functions *************************************************************************************************/
void I2C_Master_EEPROM_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _I2C_MASTER_EEPROM_INTERRUPT_H_ */


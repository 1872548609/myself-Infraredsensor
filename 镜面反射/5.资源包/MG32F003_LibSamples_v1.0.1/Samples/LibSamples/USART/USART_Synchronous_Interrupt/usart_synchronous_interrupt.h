/**
 * @file    usart_synchronous_interrupt.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    14-Nov-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _USART_SYNCHRONOUS_INTERRUPT_H_
#define _USART_SYNCHRONOUS_INTERRUPT_H_

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
} USART_RxTx_TypeDef;

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _USART_SYNCHRONOUS_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile USART_RxTx_TypeDef USART_RxStruct;
EXTERN volatile USART_RxTx_TypeDef USART_TxStruct;

/* Exported functions *************************************************************************************************/
void USART_Synchronous_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _USART_SYNCHRONOUS_INTERRUPT_H_ */


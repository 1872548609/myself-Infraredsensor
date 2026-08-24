/**
 * @file    usart_simulatewithgpio.h
 * @author  MegawinTech Application Team
 * @version V1.0.1
 * @date    18-Apr-2023
 * @brief   This file contains all the system functions
 */

/* Define to prevent recursive inclusion */
#ifndef _USART_SIMULATEWITHGPIO_H_
#define _USART_SIMULATEWITHGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Files include */
#include "mg32f003_device.h"

/* Exported types *****************************************************************************************************/

/* Exported constants *************************************************************************************************/

/* Exported macro *****************************************************************************************************/
#define sUSART_BAUDRATE_DELTA   (2)
#define sUSART_BAUTRATE_1200    (833)
#define sUSART_BAUTRATE_2400    (417)
#define sUSART_BAUTRATE_4800    (208)
#define sUSART_BAUTRATE_9600    (104)
#define sUSART_BAUTRATE_14400   (69)
#define sUSART_BAUTRATE_19200   (52)
#define sUSART_BAUTRATE_DELAY   (sUSART_BAUTRATE_9600 - sUSART_BAUDRATE_DELTA)

#define sUSART_RX_BIT_START     0
#define sUSART_RX_BIT_0         1
#define sUSART_RX_BIT_1         2
#define sUSART_RX_BIT_2         3
#define sUSART_RX_BIT_3         4
#define sUSART_RX_BIT_4         5
#define sUSART_RX_BIT_5         6
#define sUSART_RX_BIT_6         7
#define sUSART_RX_BIT_7         8
#define sUSART_RX_BIT_STOP      9

/* Exported variables *************************************************************************************************/
#undef EXTERN

#ifdef _USART_SIMULATEWITHGPIO_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile uint8_t SimulateUSART_RxData;
EXTERN volatile uint8_t SimulateUSART_RxFlag;
EXTERN volatile uint8_t SimulateUSART_RxStep;

/* Exported functions *************************************************************************************************/
void SimulateUSART_DelayUS(uint32_t Tick);
void SimulateUSART_SendData(uint8_t Data);
void USART_SimulateWithGPIO_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _USART_SIMULATEWITHGPIO_H_ */


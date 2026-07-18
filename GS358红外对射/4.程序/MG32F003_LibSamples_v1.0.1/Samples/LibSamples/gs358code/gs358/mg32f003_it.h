/**
 * @file    mg32f003_it.h
 * @brief   GS358 红外对射接收板中断声明
 */

#ifndef _MG32F003_IT_H_
#define _MG32F003_IT_H_

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void EXTI4_15_IRQHandler(void);
void ADC1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _MG32F003_IT_H_ */

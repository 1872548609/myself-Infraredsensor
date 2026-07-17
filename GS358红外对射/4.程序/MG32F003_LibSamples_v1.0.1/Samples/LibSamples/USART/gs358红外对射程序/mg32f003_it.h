/**
 * @file    mg32f003_it.h
 * @brief   中断服务函数声明
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
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _MG32F003_IT_H_ */

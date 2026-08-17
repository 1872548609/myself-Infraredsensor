/**
 * @file mg32f003_it.c
 * @brief GS358 红外对射接收板中断服务函数
 *
 * TIM3提供100 us周期预测、窗口打开和窗口关闭中断；
 * TIM1在当前窗口算法中不再使用。
 */
#define _MG32F003_IT_C_

#include "platform.h"
#include "gs358_app.h"
#include "mg32f003_it.h"

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    if (PLATFORM_DelayTick != 0U)
    {
        PLATFORM_DelayTick--;
    }
}

void EXTI4_15_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        GS358_ComparatorFallingIRQHandler();
    }
}

void TIM3_IRQHandler(void)
{
    GS358_WindowTimerIRQHandler();
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    GS358_LightLostTimerIRQHandler();
}


void ADC_IRQHandler(void)
{
    GS358_ADC_EOCIRQHandler();
}

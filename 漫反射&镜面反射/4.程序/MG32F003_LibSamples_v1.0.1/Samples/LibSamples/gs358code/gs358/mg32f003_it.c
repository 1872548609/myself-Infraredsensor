/**
 * @file mg32f003_it.c
 * @brief GS358 红外对射接收板中断服务函数
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
    /*
     * SysTick 只保留原平台 1 ms 阻塞延时计数。
     * 丢光超时判断已经改由 TIM3 独立完成。
     */
    if (PLATFORM_DelayTick != 0U)
    {
        PLATFORM_DelayTick--;
    }
}

void EXTI4_15_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        /*
         * 先清挂起位，再执行应用处理，
         * 避免处理期间再次进入同一挂起状态。
         */
        EXTI_ClearITPendingBit(EXTI_Line7);

        GS358_ComparatorFallingIRQHandler();
    }
}

void TIM3_IRQHandler(void)
{
    GS358_LightLostTimerIRQHandler();
}

void ADC_IRQHandler(void)
{
    GS358_ADC_EOCIRQHandler();
}

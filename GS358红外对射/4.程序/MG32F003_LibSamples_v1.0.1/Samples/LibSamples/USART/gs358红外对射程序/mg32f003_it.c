/**
 * @file    mg32f003_it.c
 * @brief   中断服务函数
 */

#define _MG32F003_IT_C_

#include "platform.h"
#include "usart_interrupt.h"
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

/***********************************************************************************************************************
 * @brief  USART1：接收 TOF 回传帧
 *
 * @note   RXNE 逐字节装入缓冲区；IDLE 表示当前帧结束。
 *         不在本中断内格式化或发送 RS485 文本，避免长中断导致下一字节丢失。
 ***********************************************************************************************************************/
void USART1_IRQHandler(void)
{
    uint8_t RxData;

    if ((USART_GetITStatus(USART1, USART_IT_PE) != RESET) ||
        (USART_GetITStatus(USART1, USART_IT_ERR) != RESET))
    {
        (void)USART_ReceiveData(USART1);
    }

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        RxData = USART_ReceiveData(USART1);

        if (USART_RxStruct.CompleteFlag == 0U)
        {
            if (USART_RxStruct.CurrentCount < USART_RxStruct.Length)
            {
                USART_RxStruct.Buffer[USART_RxStruct.CurrentCount] = RxData;
                USART_RxStruct.CurrentCount++;
            }
            else
            {
                /* 当前帧长度超过上限，冻结此帧，交给主循环丢弃。 */
                USART_RxStruct.OverflowFlag = 1U;
                USART_RxStruct.CompleteFlag = 1U;

                USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
                USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
            }
        }
    }

    /*
     * 清 IDLE：读取状态寄存器后读取数据寄存器。
     * USART_GetITStatus() 已经访问状态，下面读取 DR 完成清除。
     */
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        (void)USART_ReceiveData(USART1);

        if ((USART_RxStruct.CompleteFlag == 0U) &&
            (USART_RxStruct.CurrentCount > 0U))
        {
            USART_RxStruct.CompleteFlag = 1U;

            /*
             * 冻结完整帧，主循环复制完并重新启动后，才继续接收下一帧。
             */
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
            USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
        }
    }
}

/***********************************************************************************************************************
 * @brief  USART2：RS485 调试文本发送
 ***********************************************************************************************************************/
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
    {
        if (USART_TxStruct.CurrentCount < USART_TxStruct.Length)
        {
            USART_SendData(USART2, USART_TxStruct.Buffer[USART_TxStruct.CurrentCount]);
            USART_TxStruct.CurrentCount++;
        }

        if (USART_TxStruct.CurrentCount >= USART_TxStruct.Length)
        {
            /* 最后一个字节只是写入数据寄存器，仍需等待移位寄存器真正发送完毕。 */
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
            USART_ITConfig(USART2, USART_IT_TC, ENABLE);
        }
    }

    if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
        /* TC 置位说明最后一个停止位已离开总线，此时才允许 MAX485 回到接收。 */
        USART_ClearITPendingBit(USART2, USART_IT_TC);
        USART_ITConfig(USART2, USART_IT_TC, DISABLE);
        GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
        USART_TxStruct.CompleteFlag = 1U;
    }
}

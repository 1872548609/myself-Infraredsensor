/**
 * @file    usart_interrupt.h
 * @brief   USART1 接收并解析 YC02 TOF 帧；USART2 经 RS485 回传解析结果
 */
#ifndef _USART_INTERRUPT_H_
#define _USART_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mg32_conf.h"

/* YC02 固定回传帧：5A 8E 17 + 23 字节字段 + 1 字节累加和校验。 */
#define TOF_FRAME_LENGTH        27U
#define TOF_FRAME_MAX_LENGTH    32U

/*
 * USART1 原始接收和 USART2 文本发送共用该结构。
 * 255 字节可容纳一整条完整的解析文本。
 */
#define USART_RX_BUFFER_SIZE    255U

typedef struct
{
    uint8_t  Buffer[USART_RX_BUFFER_SIZE];
    uint16_t Length;          /* 本轮允许接收/发送的最大字节数 */
    uint16_t CurrentCount;    /* 当前实际已经收/发的字节数 */
    uint8_t  CompleteFlag;    /* 1：当前接收帧冻结，或当前发送已完成 */
    uint8_t  OverflowFlag;    /* 1：当前接收长度超过 Length */
} USART_RxTx_TypeDef;

/* YC02 的已解析数据结构。 */
typedef struct
{
    uint16_t norm_tof;
    uint16_t norm_peak;
    uint32_t norm_noise;
    uint16_t multshot;
    uint16_t atten_peak;
    uint32_t atten_noise;
    uint16_t ref_tof;
    int32_t  temperature_centi; /* 0.01 C，例如 4011 = 40.11 C */
    int16_t  distance_mm;       /* cal_tof，单位 mm */
    uint8_t  confidence;
} TOF_Frame_t;

#undef EXTERN
#ifdef _USART_INTERRUPT_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN volatile USART_RxTx_TypeDef USART_RxStruct;
EXTERN volatile USART_RxTx_TypeDef USART_TxStruct;

void USART_Interrupt_Sample(void);

#ifdef __cplusplus
}
#endif

#endif /* _USART_INTERRUPT_H_ */

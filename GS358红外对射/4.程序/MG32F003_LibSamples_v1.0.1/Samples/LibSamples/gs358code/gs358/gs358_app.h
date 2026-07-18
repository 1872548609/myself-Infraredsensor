/**
 * @file    gs358_app.h
 * @brief   GS358 红外对射接收板应用层
 *
 * 硬件映射：
 *   PA7  - 比较器 COMP_SIGN，下降沿外部中断
 *   PA8  - 常闭判断输出 IO_OUTPUT_NC
 *   PA9  - 常开判断输出 IO_OUTPUT_NO
 *   PA10 - 红色指示灯，默认高电平点亮
 *   PA11 - PWM_OUT，当前仅预留并保持低电平
 *
 * ADC 扫描顺序：
 *   result[0] = ADC_IN0 / ADC1_VIN0 / PB1
 *   result[1] = ADC_IN1 / ADC1_VIN1 / PB0
 *   result[2] = ADC_IN2 / ADC1_VIN2 / PA3
 *   result[3] = ADC_IN3 / ADC1_VIN3 / PA12
 *   result[4] = ADC_IN5 / ADC1_VIN5 / PA2
 *
 * ADC_IN4 对应 PA11，与 PWM_OUT 复用，因此本版本不扫描 ADC_IN4。
 */

#ifndef _GS358_APP_H_
#define _GS358_APP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================== 可调参数 =========================== */

/**
 * 连续接收到多少个比较器下降沿后确认“有光”。
 *
 * 发射周期为 1 ms 时，默认 3 次约需 3 ms。
 * 实际使用时应结合发射周期和干扰情况调整。
 */
#define GS358_EDGE_CONFIRM_COUNT            3U

/**
 * 超过该时间没有收到比较器下降沿，确认“遮光/无光”。
 *
 * 必须明显大于正常发射周期。
 */
#define GS358_LIGHT_LOST_TIMEOUT_MS         1U

/* 原理图中 PA8、PA9 经电阻驱动 NPN，默认高电平表示通道动作。 */
#define GS358_NO_OUTPUT_ACTIVE_HIGH         1U
#define GS358_NC_OUTPUT_ACTIVE_HIGH         1U

/* 原理图中红灯串联电阻后接地，默认 PA10 高电平点亮。 */
#define GS358_RED_LED_ACTIVE_HIGH           1U

/* ADC 一帧包含 0、1、2、3、5 共五个通道。 */
#define GS358_ADC_CHANNEL_COUNT             5U

/* =========================== 类型定义 =========================== */

typedef enum
{
    GS358_LED_BLOCKED_ON = 0,   /* 遮光/无光时亮 */
    GS358_LED_LIGHT_ON   = 1    /* 有光时亮 */
} GS358_LedMode;

/* =========================== 状态变量 =========================== */

/**
 * ADC 数据数组。
 *
 * 索引顺序固定为 ADC_IN0、ADC_IN1、ADC_IN2、ADC_IN3、ADC_IN5。
 * 在完成一整帧五通道扫描后一起更新。
 */
extern volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT];

/* 已完成的 ADC 五通道帧数。 */
extern volatile uint32_t g_gs358_adc_frame_count;

/* PA7 捕获到的比较器下降沿累计数。 */
extern volatile uint32_t g_gs358_compare_edge_total;

/* 1=已确认有光，0=遮光/无光。 */
extern volatile uint8_t g_gs358_light_present;

/* =========================== 接口函数 =========================== */

void GS358_AppInit(void);
void GS358_AppProcess(void);

/* 运行时切换红灯逻辑。 */
void GS358_SetLedMode(GS358_LedMode mode);
GS358_LedMode GS358_GetLedMode(void);

/* 中断入口调用，用户代码不要直接调用。 */
void GS358_ComparatorFallingIRQHandler(void);
void GS358_ADC_EOCIRQHandler(void);
void GS358_1msIRQHandler(void);

/**
 * ADC 五通道一帧完成后的中断内回调。
 *
 * 本函数已经在 gs358_app.c 中留出 USER CODE 区域。
 * 回调运行在 ADC 中断上下文中，应保持短小；复杂处理建议只置标志，
 * 再放到 GS358_AppProcess() 中执行。
 */
void GS358_ADC_ScanCompleteIRQHook(void);

/**
 * PA11 PWM 预留接口。
 *
 * 当前版本不会启用定时器，只把 PA11 配置为推挽输出并保持低电平。
 * 后续需要发射信号时，可在本函数 USER CODE 区域配置 TIM14_CH1。
 */
void GS358_PWM_ConfigureReserved(uint32_t frequency_hz,
                                 uint16_t duty_permille);

#ifdef __cplusplus
}
#endif

#endif /* _GS358_APP_H_ */

/**
 * @file gs358_app.h
 * @brief GS358 红外对射接收板应用层
 *
 * 硬件映射：
 * PA7  - 比较器 COMP_SIGN，下降沿外部中断
 * PA8  - 常闭判断输出 IO_OUTPUT_NC
 * PA9  - 常开判断输出 IO_OUTPUT_NO
 * PA10 - 红色指示灯，默认高电平点亮
 * PA11 - TIM14_CH1 发射 PWM 输出
 *
 * ADC 扫描顺序：
 * result[0] = ADC_IN0 / ADC1_VIN0 / PB1
 * result[1] = ADC_IN1 / ADC1_VIN1 / PB0
 * result[2] = ADC_IN2 / ADC1_VIN2 / PA3
 * result[3] = ADC_IN3 / ADC1_VIN3 / PA12
 * result[4] = ADC_IN5 / ADC1_VIN5 / PA2
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
 * 发射周期约为 1 ms 时，默认 3 次约需 3 ms。
 */
#define GS358_EDGE_CONFIRM_COUNT             3U

/**
 * 从最后一次比较器下降沿开始计算的丢光超时时间。
 *
 * 当前设置：
 * 1500 us = 1.5 ms
 *
 * 每次 PA7 比较器下降沿都会将 TIM3 清零并重新启动；
 * 连续超过该时间没有下降沿，TIM3 才产生更新中断并判定无光。
 */
#define GS358_LIGHT_LOST_TIMEOUT_US          5000U

/**
 * TIM3 计数频率。
 *
 * 设置为 1 MHz 后，每个计数对应 1 us。
 */
#define GS358_TIMEOUT_TIMER_TICK_HZ          1000000UL

/**
 * 是否启用 PA11 发射 PWM。
 *
 * 1：上电初始化后自动输出 PWM
 * 0：PA11 保持低电平，不启动 TIM14
 */
#define GS358_PWM_ENABLE                     1U

/**
 * TIM14 的内部计数频率。
 *
 * 默认 1 MHz，因此每个计数为 1 us。
 */
#define GS358_PWM_TIMER_TICK_HZ              1000000UL

/**
 * 发射 PWM 周期，单位 us。
 *
 * 默认 1000 us，即输出频率为 1 kHz。
 */
#define GS358_PWM_PERIOD_US                  100UL

/**
 * 发射 PWM 占空比，单位为千分比。
 *
 * 50 / 1000 = 0.05 = 5%，默认高电平脉宽为 50 us。
 * 可设置范围：0～1000。
 */
#define GS358_PWM_DUTY_PERMILLE              50UL

/**
 * PWM 有效电平。
 *
 * 1：高电平为有效脉冲
 * 0：低电平为有效脉冲
 */
#define GS358_PWM_ACTIVE_HIGH                1U

/* 原理图中 PA8、PA9 经电阻驱动 NPN，默认高电平表示通道动作。 */
#define GS358_NO_OUTPUT_ACTIVE_HIGH          1U
#define GS358_NC_OUTPUT_ACTIVE_HIGH          1U

/* 原理图中红灯串联电阻后接地，默认 PA10 高电平点亮。 */
#define GS358_RED_LED_ACTIVE_HIGH            1U

/* ADC 一帧包含 0、1、2、3、5 共五个通道。 */
#define GS358_ADC_CHANNEL_COUNT              5U

/* =========================== 类型定义 =========================== */

typedef enum
{
    GS358_LED_BLOCKED_ON = 0, /* 遮光/无光时亮 */
    GS358_LED_LIGHT_ON   = 1  /* 有光时亮 */
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
void GS358_LightLostTimerIRQHandler(void);
void GS358_ADC_EOCIRQHandler(void);

/**
 * ADC 五通道一帧完成后的中断内回调。
 *
 * 本函数已经在 gs358_app.c 中留出 USER CODE 区域。
 * 回调运行在 ADC 中断上下文中，应保持短小；复杂处理建议只置标志，
 * 再放到 GS358_AppProcess() 中执行。
 */
void GS358_ADC_ScanCompleteIRQHook(void);

/**
 * 立即启动或停止 PA11 的 TIM14_CH1 PWM 输出。
 *
 * 初始化参数由 GS358_PWM_* 宏决定。
 */
void GS358_PWM_Start(void);
void GS358_PWM_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* _GS358_APP_H_ */

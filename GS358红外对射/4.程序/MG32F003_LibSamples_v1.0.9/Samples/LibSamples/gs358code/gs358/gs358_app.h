/**
 * @file gs358_app.h
 * @brief GS358 红外对射接收板应用层
 *
 * 硬件映射：
 * PA7  - 比较器 COMP_SIGN，下降沿外部中断
 * PA8  - 常闭判断输出 IO_OUTPUT_NC
 * PA9  - 常开判断输出 IO_OUTPUT_NO
 * PA10 - 红色指示灯
 * PA11 - PWM_OUT
 *
 * ADC扫描顺序：
 * result[0] = ADC_IN0 / ADC1_VIN0 / PB1
 * result[1] = ADC_IN1 / ADC1_VIN1 / PB0
 * result[2] = ADC_IN2 / ADC1_VIN2 / PA3
 * result[3] = ADC_IN3 / ADC1_VIN3 / PA12
 * result[4] = ADC_IN5 / ADC1_VIN5 / PA2
 *
 * ADC_IN4对应PA11，与PWM_OUT复用，因此不扫描ADC_IN4。
 */

#ifndef GS358_APP_H
#define GS358_APP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================== 可调参数 =========================== */

/* 发射信号目标周期，单位us。 */
#define GS358_SIGNAL_PERIOD_US                     100U

/* 预测边沿前、后各打开多少us。当前窗口为预测点前后各8us。 */
#define GS358_WINDOW_EARLY_US                       16U
#define GS358_WINDOW_LATE_US                        8U

/* 连续多少个“恰好1个边沿”的窗口后确认有光。 */
#define GS358_LIGHT_CONFIRM_WINDOWS                 6U

/* 已确认有光后，连续多少个无效窗口后确认无光并解除同步。 */
#define GS358_LIGHT_LOST_WINDOWS                    4U

/* 尚未确认有光时，连续多少个无效窗口后解除错误同步。 */
#define GS358_SYNC_LOST_WINDOWS                     3U

/* 单窗口达到此边沿数，记录为严重噪声窗口。 */
#define GS358_SEVERE_NOISE_EDGE_COUNT               3U

/*
 * 窗口测试输出：
 * 1：PA8暂时作为窗口测试口；窗口打开时动作、关闭时释放。
 *    示波器同时观察PA7和PA8，即可确认边沿是否位于窗口中。
 *    PA9和LED仍按正常逻辑工作。
 * 0：PA8恢复正常NC输出。
 */
#define GS358_WINDOW_TEST_ENABLE                    0U

/*
 * 定时器分配：
 * TIM3：100us周期预测及检测窗口；
 * TIM1：本方案不再用于丢光判断，保留给其他功能；
 * TIM14：PA11 PWM。
 */
#define GS358_PERIOD_TIMER                         TIM3

/* TIM3使用1 MHz时基。 */
#define GS358_TIMER_TICK_HZ                  1000000UL

/* 原理图中PA8、PA9经电阻驱动NPN，高电平表示通道动作。 */
#define GS358_NO_OUTPUT_ACTIVE_HIGH                 1U
#define GS358_NC_OUTPUT_ACTIVE_HIGH                 1U

/* 红灯有效电平。 */
#define GS358_RED_LED_ACTIVE_HIGH                   0U

/* ADC一帧包含0、1、2、3、5共五个通道。 */
#define GS358_ADC_CHANNEL_COUNT                     5U

/* 独立看门狗功能开关。 */
#define GS358_WATCHDOG_ENABLE                       1U

/*
 * IWDG：LSI约40 kHz，32分频，Reload=2499，
 * 标称超时时间约2秒。
 */
#define GS358_WATCHDOG_RELOAD_VALUE              2499U

#if ((GS358_WATCHDOG_ENABLE != 0U) && \
     (GS358_WATCHDOG_ENABLE != 1U))
#error "GS358_WATCHDOG_ENABLE must be 0 or 1"
#endif

#if ((GS358_WINDOW_TEST_ENABLE != 0U) && \
     (GS358_WINDOW_TEST_ENABLE != 1U))
#error "GS358_WINDOW_TEST_ENABLE must be 0 or 1"
#endif

#if (GS358_SIGNAL_PERIOD_US < 2U)
#error "GS358_SIGNAL_PERIOD_US must be >= 2"
#endif

#if (GS358_SIGNAL_PERIOD_US > 65536UL)
#error "GS358_SIGNAL_PERIOD_US must be <= 65536"
#endif

#if ((GS358_WINDOW_EARLY_US + GS358_WINDOW_LATE_US) >= \
     GS358_SIGNAL_PERIOD_US)
#error "Window width must be smaller than signal period"
#endif

#if ((GS358_WINDOW_EARLY_US == 0U) || \
     (GS358_WINDOW_LATE_US == 0U))
#error "Window early/late time must be non-zero"
#endif

#if ((GS358_LIGHT_CONFIRM_WINDOWS == 0U) || \
     (GS358_LIGHT_CONFIRM_WINDOWS > 255U) || \
     (GS358_LIGHT_LOST_WINDOWS == 0U) || \
     (GS358_LIGHT_LOST_WINDOWS > 255U) || \
     (GS358_SYNC_LOST_WINDOWS == 0U) || \
     (GS358_SYNC_LOST_WINDOWS > 255U))
#error "Window decision counts must be in range 1..255"
#endif

#if ((GS358_SEVERE_NOISE_EDGE_COUNT < 2U) || \
     (GS358_SEVERE_NOISE_EDGE_COUNT > 255U))
#error "GS358_SEVERE_NOISE_EDGE_COUNT must be in range 2..255"
#endif

#if (GS358_WATCHDOG_RELOAD_VALUE > 0x0FFFU)
#error "GS358_WATCHDOG_RELOAD_VALUE must be <= 0x0FFF"
#endif


/* =========================== 类型定义 =========================== */

typedef enum
{
    GS358_LED_BLOCKED_ON = 0,
    GS358_LED_LIGHT_ON   = 1
} GS358_LedMode;

/* =========================== 状态变量 =========================== */

extern volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT];
extern volatile uint32_t g_gs358_adc_frame_count;
extern volatile uint32_t g_gs358_compare_edge_total;
extern volatile uint8_t  g_gs358_light_present;

extern volatile uint16_t g_gs358_last_period_us;
extern volatile uint32_t g_gs358_period_valid_total;
extern volatile uint32_t g_gs358_period_invalid_total;
extern volatile uint8_t  g_gs358_last_period_valid;
extern volatile uint32_t g_gs358_period_miss_total;

extern volatile uint16_t g_gs358_last_raw_period_us;
/* 为兼容原有Keil Watch保留；窗口模式下记录标称周期，carry恒为0。 */
extern volatile uint16_t g_gs358_last_adjusted_period_us;
extern volatile uint16_t g_gs358_last_period_carry_us;

/* 新窗口算法的Keil Watch观察变量。 */
extern volatile uint8_t  g_gs358_window_open;
extern volatile uint8_t  g_gs358_window_edge_count;
extern volatile uint8_t  g_gs358_last_window_edge_count;
extern volatile uint8_t  g_gs358_last_window_valid;
extern volatile uint8_t  g_gs358_sync_locked;
extern volatile uint8_t  g_gs358_valid_window_count;
extern volatile uint8_t  g_gs358_invalid_window_count;
extern volatile uint32_t g_gs358_window_total;
extern volatile uint32_t g_gs358_window_valid_total;
extern volatile uint32_t g_gs358_window_missing_total;
extern volatile uint32_t g_gs358_window_noise_total;
extern volatile uint32_t g_gs358_window_severe_noise_total;



/* =========================== 接口函数 =========================== */

void GS358_AppInit(void);
void GS358_AppProcess(void);

void GS358_SetLedMode(GS358_LedMode mode);
GS358_LedMode GS358_GetLedMode(void);

void GS358_ComparatorFallingIRQHandler(void);
void GS358_WindowTimerIRQHandler(void);
void GS358_LightLostTimerIRQHandler(void);
void GS358_ADC_EOCIRQHandler(void);
void GS358_1msIRQHandler(void);

void GS358_ADC_ScanCompleteIRQHook(void);

void GS358_PWM_ConfigureReserved(uint32_t frequency_hz,
                                 uint16_t duty_permille);

#ifdef __cplusplus
}
#endif

#endif /* GS358_APP_H */

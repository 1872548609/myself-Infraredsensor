/**
 * @file gs358_app.h
 * @brief GS358 红外对射接收板应用层
 *
 * 硬件映射：
 * PA7  - 比较器输出，下降沿外部中断
 * PA8  - 常闭判断输出
 * PA9  - 常开判断输出
 * PA10 - 红色指示灯
 * PA11 - PWM 预留输出，当前保持低电平
 *
 * ADC 扫描顺序：
 * result[0] = ADC_IN0 / PB1
 * result[1] = ADC_IN1 / PB0
 * result[2] = ADC_IN2 / PA3
 * result[3] = ADC_IN3 / PA12
 * result[4] = ADC_IN5 / PA2
 */

#ifndef _GS358_APP_H_
#define _GS358_APP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================== 可调参数 =========================== */

#define GS358_SIGNAL_PERIOD_US                  70U
#define GS358_SIGNAL_PERIOD_TOLERANCE_US          5U
#define GS358_EDGE_CONFIRM_COUNT                 10U
#define GS358_PERIOD_ERROR_RESET_CONFIRM          1U
#define GS358_LIGHT_LOST_TIMEOUT_US            1500U

#define GS358_PERIOD_TIMER                     TIM1
#define GS358_TIMEOUT_TIMER                    TIM3
#define GS358_TIMER_TICK_HZ              1000000UL

#define GS358_NO_OUTPUT_ACTIVE_HIGH             1U
#define GS358_NC_OUTPUT_ACTIVE_HIGH             1U
#define GS358_RED_LED_ACTIVE_HIGH               0U

#define GS358_ADC_CHANNEL_COUNT                 5U

#define GS358_WATCHDOG_ENABLE                   1U
#define GS358_WATCHDOG_RELOAD_VALUE          2499U

/* =========================== 编译期参数检查 =========================== */

#if ((GS358_WATCHDOG_ENABLE != 0U) && \
     (GS358_WATCHDOG_ENABLE != 1U))
#error "GS358_WATCHDOG_ENABLE must be 0 or 1"
#endif

#if (GS358_WATCHDOG_RELOAD_VALUE > 0x0FFFU)
#error "GS358_WATCHDOG_RELOAD_VALUE must be <= 0x0FFF"
#endif

#if (GS358_SIGNAL_PERIOD_US == 0U)
#error "GS358_SIGNAL_PERIOD_US must be greater than 0"
#endif

#if (GS358_SIGNAL_PERIOD_TOLERANCE_US >= GS358_SIGNAL_PERIOD_US)
#error "GS358_SIGNAL_PERIOD_TOLERANCE_US must be less than GS358_SIGNAL_PERIOD_US"
#endif

#if (GS358_EDGE_CONFIRM_COUNT == 0U)
#error "GS358_EDGE_CONFIRM_COUNT must be greater than 0"
#endif

#if ((GS358_PERIOD_ERROR_RESET_CONFIRM != 0U) && \
     (GS358_PERIOD_ERROR_RESET_CONFIRM != 1U))
#error "GS358_PERIOD_ERROR_RESET_CONFIRM must be 0 or 1"
#endif

#if (GS358_LIGHT_LOST_TIMEOUT_US == 0U)
#error "GS358_LIGHT_LOST_TIMEOUT_US must be greater than 0"
#endif

#if (GS358_LIGHT_LOST_TIMEOUT_US > 65536UL)
#error "GS358_LIGHT_LOST_TIMEOUT_US must be <= 65536"
#endif

/* =========================== 类型定义 =========================== */

typedef enum
{
    GS358_LED_BLOCKED_ON = 0,
    GS358_LED_LIGHT_ON   = 1
} GS358_LedMode;

/* =========================== 状态变量 =========================== */

extern volatile uint16_t
    g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT];

extern volatile uint32_t g_gs358_adc_frame_count;
extern volatile uint32_t g_gs358_compare_edge_total;
extern volatile uint8_t  g_gs358_light_present;

extern volatile uint16_t g_gs358_last_period_us;
extern volatile uint32_t g_gs358_period_valid_total;
extern volatile uint32_t g_gs358_period_invalid_total;
extern volatile uint8_t  g_gs358_last_period_valid;

/* =========================== 接口函数 =========================== */

void GS358_AppInit(void);
void GS358_AppProcess(void);

void GS358_SetLedMode(GS358_LedMode mode);
GS358_LedMode GS358_GetLedMode(void);

void GS358_ComparatorFallingIRQHandler(void);
void GS358_LightLostTimerIRQHandler(void);
void GS358_ADC_EOCIRQHandler(void);

void GS358_ADC_ScanCompleteIRQHook(void);

void GS358_PWM_ConfigureReserved(uint32_t frequency_hz,
                                 uint16_t duty_permille);

#ifdef __cplusplus
}
#endif

#endif /* _GS358_APP_H_ */

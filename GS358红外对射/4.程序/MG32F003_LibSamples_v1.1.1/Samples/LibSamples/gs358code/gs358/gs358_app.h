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
#define GS358_SIGNAL_PERIOD_US                    100U

/* 周期允许容差。当前有效范围为 995~1005 us。 */
#define GS358_SIGNAL_PERIOD_TOLERANCE_US            15U


/*
 * 10次间隔累计总时长允许的误差。
 *
 * 例如单周期100us、统计10次：
 * 目标总时长为1000us，允许范围为900～1100us。
 */
#define GS358_TOTAL_PERIOD_TOLERANCE_US    100U

/*
 * 有光确认采用 N 选 M：
 * 每10个周期中，至少7个有效周期即确认有光。
 */
#define GS358_PERIOD_CHECK_COUNT       10U
#define GS358_EDGE_CONFIRM_COUNT        8U

/*
 * 周期错误时是否清零连续有效次数：
 * 1：错误一次即重新累计；
 * 0：错误周期不增加计数，但不清零。
 */
#define GS358_PERIOD_ERROR_RESET_CONFIRM            1U

/*
 * 比较器中断测试输出：
 * 1：PA8暂时作为测试输出。进入比较器下降沿中断时动作，
 *    相邻边沿周期判断有效时释放；PA9和LED仍正常工作。
 * 0：关闭测试功能，PA8恢复正常NC输出。
 */
#define GS358_COMPARE_IRQ_TEST_ENABLE                0U

/* 丢光超时，单位us。 */
#define GS358_LIGHT_LOST_TIMEOUT_US                 400U

/*
 * 定时器分配：
 * TIM3：周期检测；
 * TIM1：丢光超时；
 * TIM14：PA11 PWM。
 */
#define GS358_PERIOD_TIMER                         TIM3
#define GS358_TIMEOUT_TIMER                        TIM1

/* TIM1、TIM3均使用1 MHz时基。 */
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

#if ((GS358_COMPARE_IRQ_TEST_ENABLE != 0U) && \
     (GS358_COMPARE_IRQ_TEST_ENABLE != 1U))
#error "GS358_COMPARE_IRQ_TEST_ENABLE must be 0 or 1"
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
/* 为兼容原有 Keil Watch 保留：简化后 adjusted 等于 raw，carry 恒为 0。 */
extern volatile uint16_t g_gs358_last_adjusted_period_us;
extern volatile uint16_t g_gs358_last_period_carry_us;



/* =========================== 接口函数 =========================== */

void GS358_AppInit(void);
void GS358_AppProcess(void);

void GS358_SetLedMode(GS358_LedMode mode);
GS358_LedMode GS358_GetLedMode(void);

void GS358_ComparatorFallingIRQHandler(void);
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

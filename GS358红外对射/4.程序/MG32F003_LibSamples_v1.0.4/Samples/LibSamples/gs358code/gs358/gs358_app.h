/**
 * @file gs358_app.h
 * @brief GS358 红外对射接收板应用层
 *
 * 硬件映射：
 * PA7  - 比较器 COMP_SIGN，下降沿外部中断
 * PA8  - 常闭判断输出 IO_OUTPUT_NC
 * PA9  - 常开判断输出 IO_OUTPUT_NO
 * PA10 - 红色指示灯，默认高电平点亮
 * PA11 - PWM_OUT，当前仅预留并保持低电平
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
 * 发射信号目标周期，单位：us。
 *
 * 示例：
 *  500 Hz -> 2000 us
 * 1000 Hz -> 1000 us
 * 2000 Hz ->  500 us
 */
#define GS358_SIGNAL_PERIOD_US                   70U

/**
 * 周期允许容差，单位：us。
 *
 * 当前有效范围：
 * 1000 us ± 100 us，即 900~1100 us。
 */
#define GS358_SIGNAL_PERIOD_TOLERANCE_US          5U

/**
 * 连续得到多少个正确周期后确认“有光”。
 *
 * 第一个下降沿只建立时间基准，不计入正确周期。
 */
#define GS358_EDGE_CONFIRM_COUNT                    10U

/**
 * 周期错误时是否清零连续有效次数。
 *
 * 1：错误一次即重新累计，抗干扰更强。
 * 0：错误周期不增加计数，但不清零。
 */
#define GS358_PERIOD_ERROR_RESET_CONFIRM             0U


/* 丢光超时改回 1500 us。 */
#define GS358_LIGHT_LOST_TIMEOUT_US            1500U


#define GS358_PERIOD_TIMER          TIM1
#define GS358_TIMEOUT_TIMER         TIM3

/* TIM1、TIM3都使用1 MHz时基。 */
#define GS358_TIMER_TICK_HZ                   1000000UL


/* 原理图中PA8、PA9经电阻驱动NPN，高电平表示通道动作。 */
#define GS358_NO_OUTPUT_ACTIVE_HIGH                  1U
#define GS358_NC_OUTPUT_ACTIVE_HIGH                  1U

/* 红灯高电平点亮。输出有效灯设置 */
#define GS358_RED_LED_ACTIVE_HIGH                    0U

/* ADC一帧包含0、1、2、3、5共五个通道。 */
#define GS358_ADC_CHANNEL_COUNT                      5U


/**
 * 独立看门狗功能开关。
 *
 * 1：开启独立看门狗
 * 0：关闭独立看门狗
 */
#define GS358_WATCHDOG_ENABLE               1U

/**
 * 独立看门狗重装载值。
 *
 * 当前配置：
 * LSI              = 40 kHz
 * IWDG 预分频       = 32
 * Reload           = 2499
 *
 * 标称超时时间：
 *
 * (2499 + 1) × 32 / 40000
 * = 2 秒
 *
 * 注意：LSI 本身存在频率误差，因此实际超时时间会有偏差。
 */
#define GS358_WATCHDOG_RELOAD_VALUE          2499U

#if ((GS358_WATCHDOG_ENABLE != 0U) && \
     (GS358_WATCHDOG_ENABLE != 1U))
#error "GS358_WATCHDOG_ENABLE must be 0 or 1"
#endif

#if (GS358_WATCHDOG_RELOAD_VALUE > 0x0FFFU)
#error "GS358_WATCHDOG_RELOAD_VALUE must be <= 0x0FFF"
#endif

/*
 * 一个判断窗口由有效周期和漏周期共同组成。
 *
 * valid > 7 时认为有光，也就是至少需要8个有效周期。
 */
#define GS358_DETECT_WINDOW_COUNT       10U
#define GS358_DETECT_VALID_THRESHOLD     7U
/* =========================== 类型定义 =========================== */

typedef enum
{
    GS358_LED_BLOCKED_ON = 0,
    GS358_LED_LIGHT_ON   = 1
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

/* 最近一次测得的相邻下降沿周期，单位us。 */
extern volatile uint16_t g_gs358_last_period_us;

/* 周期判断统计。 */
extern volatile uint32_t g_gs358_period_valid_total;
extern volatile uint32_t g_gs358_period_invalid_total;

/* 1=最近一次周期正确；0=错误或尚未形成完整周期。 */
extern volatile uint8_t g_gs358_last_period_valid;

/* 累计大于有效周期的迟到或漏周期次数。 */
extern volatile uint32_t g_gs358_period_miss_total;

/*TIM1直接读取的原始周期。*/
extern volatile uint16_t g_gs358_last_raw_period_us;

/*加入补偿后的周期*/
extern volatile uint16_t g_gs358_last_adjusted_period_us;

/*准备带到下一条边沿的相位余量*/
extern volatile uint16_t g_gs358_last_period_carry_us;


extern volatile uint8_t g_gs358_last_window_valid_count;

extern volatile uint8_t g_gs358_last_window_miss_count;

extern volatile uint8_t g_gs358_last_window_light_present;

extern volatile uint32_t g_gs358_window_complete_total;

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

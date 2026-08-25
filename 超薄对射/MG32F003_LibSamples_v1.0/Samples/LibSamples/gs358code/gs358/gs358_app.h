/**
 * @file gs358_app.h
 * @brief GS358 红外对射接收板应用层
 *
 * 硬件映射：
 * PA7  - 预留（本版本不使用比较器外部中断）
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
#define GS358_SIGNAL_PERIOD_US                     70U

/* 周期允许容差；当前有效范围为70±12，即58~82 us。 */
#define GS358_SIGNAL_PERIOD_TOLERANCE_US            12U

/*
 * 一个确认组需要的有效周期数量。10个周期需要11个ADC阈值下穿事件。
 */
#define GS358_EDGE_CONFIRM_COUNT                    10U

/*
 * 10个有效周期的累计总时长必须落在
 * [GS358_CONFIRM_TOTAL_US - GS358_CONFIRM_TOTAL_TOLERANCE_US,
 *  GS358_CONFIRM_TOTAL_US + GS358_CONFIRM_TOTAL_TOLERANCE_US] 内。
 */
#define GS358_CONFIRM_TOTAL_US                     700U
#define GS358_CONFIRM_TOTAL_TOLERANCE_US            80U

/* TIM3的单次计时上限；必须覆盖累计总时长允许的最大值。 */
#define GS358_DETECTION_WINDOW_US \
    (GS358_CONFIRM_TOTAL_US + GS358_CONFIRM_TOTAL_TOLERANCE_US)

/*
 * ADC阈值事件来源。
 * ADC_TRIGGER_CHANNEL_INDEX是g_gs358_adc_values[]的下标：
 * 0=VIN0(PB1)，1=VIN1(PB0)，2=VIN2(PA3)，3=VIN3(PA12)，4=VIN5(PA2)。
 * 本板接收信号为低有效：无光静态值约为2042，有光脉冲向下跌落。
 * ADC值下穿触发阈值时生成一次事件；必须重新上升到释放阈值后，
 * 才允许下一次下穿事件。
 */
#define GS358_ADC_TRIGGER_CHANNEL_INDEX             3U
#define GS358_ADC_TRIGGER_THRESHOLD              1000U
#define GS358_ADC_TRIGGER_HYSTERESIS               100U

/*
 * ADC原始波形抓取测试。
 * 1：ADC只转换检测通道，最大化采样速度；连续循环写入数组，
 *    同时暂停阈值/周期/输出判定，供Keil Watch观察10us高电平。
 * 0：恢复五通道扫描和正常检测。
 */
#define GS358_ADC_CAPTURE_TEST_ENABLE                0U
#define GS358_ADC_CAPTURE_BUFFER_SIZE              256U

/*
 * 正常检测的ADC通道数量。
 * 1：只连续采集GS358_ADC_TRIGGER_CHANNEL_INDEX，最大化下穿时间分辨率；
 * 0：恢复0、1、2、3、5五通道扫描。
 * 此开关不暂停周期判断，与GS358_ADC_CAPTURE_TEST_ENABLE相互独立。
 */
#define GS358_ADC_SINGLE_CHANNEL_ENABLE              1U

/*
 * ADC下穿事件调试记录。
 * 0：生产模式，记录代码及数组完全不参与编译；
 * 1：调试模式，记录每次真实下穿，供Keil Watch检查连续性。
 */
#define GS358_FALL_RECORD_ENABLE                      1U

#if (GS358_FALL_RECORD_ENABLE != 0U)
/* 数组为环形缓冲区，write_index指向下一次写入位置。 */
#define GS358_FALL_RECORD_BUFFER_SIZE               32U

#define GS358_FALL_RECORD_FIRST                      0U
#define GS358_FALL_RECORD_PERIOD_VALID               1U
#define GS358_FALL_RECORD_EARLY_NOISE                2U
#define GS358_FALL_RECORD_PERIOD_INVALID             3U
#endif

/* 将g_gs358_adc_values[]下标映射为ADC硬件通道号。 */
#if (GS358_ADC_TRIGGER_CHANNEL_INDEX == 0U)
#define GS358_ADC_TRIGGER_ADC_CHANNEL       ADC_Channel_0
#elif (GS358_ADC_TRIGGER_CHANNEL_INDEX == 1U)
#define GS358_ADC_TRIGGER_ADC_CHANNEL       ADC_Channel_1
#elif (GS358_ADC_TRIGGER_CHANNEL_INDEX == 2U)
#define GS358_ADC_TRIGGER_ADC_CHANNEL       ADC_Channel_2
#elif (GS358_ADC_TRIGGER_CHANNEL_INDEX == 3U)
#define GS358_ADC_TRIGGER_ADC_CHANNEL       ADC_Channel_3
#else
#define GS358_ADC_TRIGGER_ADC_CHANNEL       ADC_Channel_5
#endif

/*
 * 比较器中断测试输出：
 * 1：PA8暂时作为测试输出。进入比较器下降沿中断时动作，
 *    相邻边沿周期判断有效时释放；PA9和LED仍正常工作。
 * 0：关闭测试功能，PA8恢复正常NC输出。
 */
#define GS358_COMPARE_IRQ_TEST_ENABLE               0U

#define GS358_PERIOD_RECORD_ENABLE                  0U


/* 丢光超时，单位us。 */
#define GS358_LIGHT_LOST_TIMEOUT_US                 500U

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

#if (GS358_ADC_TRIGGER_CHANNEL_INDEX >= GS358_ADC_CHANNEL_COUNT)
#error "GS358_ADC_TRIGGER_CHANNEL_INDEX must be 0..4"
#endif

#if ((GS358_ADC_CAPTURE_TEST_ENABLE != 0U) && \
     (GS358_ADC_CAPTURE_TEST_ENABLE != 1U))
#error "GS358_ADC_CAPTURE_TEST_ENABLE must be 0 or 1"
#endif

#if ((GS358_ADC_SINGLE_CHANNEL_ENABLE != 0U) && \
     (GS358_ADC_SINGLE_CHANNEL_ENABLE != 1U))
#error "GS358_ADC_SINGLE_CHANNEL_ENABLE must be 0 or 1"
#endif

#if ((GS358_FALL_RECORD_ENABLE != 0U) && \
     (GS358_FALL_RECORD_ENABLE != 1U))
#error "GS358_FALL_RECORD_ENABLE must be 0 or 1"
#endif

#if ((GS358_FALL_RECORD_ENABLE != 0U) && \
     ((GS358_FALL_RECORD_BUFFER_SIZE == 0U) || \
      (GS358_FALL_RECORD_BUFFER_SIZE > 255U)))
#error "GS358_FALL_RECORD_BUFFER_SIZE must be 1..255"
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

/* 原始ADC环形采样数组：测试开关为1时有效。 */
extern volatile uint16_t g_gs358_adc_capture_buffer[GS358_ADC_CAPTURE_BUFFER_SIZE];
extern volatile uint16_t g_gs358_adc_capture_write_index;
extern volatile uint32_t g_gs358_adc_capture_total;
extern volatile uint16_t g_gs358_adc_capture_min;
extern volatile uint16_t g_gs358_adc_capture_max;

/* ADC阈值触发调试变量：可直接加入Keil Watch。 */
extern volatile uint16_t g_gs358_adc_trigger_value;
extern volatile int16_t  g_gs358_adc_trigger_delta;
extern volatile uint16_t g_gs358_adc_trigger_low_threshold;
extern volatile uint16_t g_gs358_adc_trigger_high_threshold;
extern volatile uint16_t g_gs358_adc_trigger_event_value;
extern volatile uint16_t g_gs358_adc_trigger_event_time_us;
/* fall_total：下穿事件数；rise_total：回升到释放阈值的次数。 */
extern volatile uint32_t g_gs358_adc_trigger_fall_total;
extern volatile uint32_t g_gs358_adc_trigger_rise_total;
extern volatile uint16_t g_gs358_adc_trigger_high_frame_count;
extern volatile uint16_t g_gs358_adc_trigger_last_high_frame_count;
extern volatile uint16_t g_gs358_adc_trigger_max_high_frame_count;
/* 1表示当前正在低电平脉冲内，0表示已回升并重新布防。 */
extern volatile uint8_t  g_gs358_adc_trigger_high_state;

#if (GS358_FALL_RECORD_ENABLE != 0U)
/*
 * 真实ADC下穿记录。time_us为相对此组首个下穿的时间位置；
 * interval_us为相对“上一个真实下穿”的间隔，不跳过提前毛刺。
 * type取GS358_FALL_RECORD_*；group_id用于从环形数组中识别同一组。
 */
extern volatile uint16_t g_gs358_fall_record_adc[GS358_FALL_RECORD_BUFFER_SIZE];
extern volatile uint16_t g_gs358_fall_record_time_us[GS358_FALL_RECORD_BUFFER_SIZE];
extern volatile uint16_t g_gs358_fall_record_interval_us[GS358_FALL_RECORD_BUFFER_SIZE];
extern volatile uint16_t g_gs358_fall_record_group_id[GS358_FALL_RECORD_BUFFER_SIZE];
extern volatile uint8_t  g_gs358_fall_record_type[GS358_FALL_RECORD_BUFFER_SIZE];
extern volatile uint8_t  g_gs358_fall_record_write_index;
extern volatile uint32_t g_gs358_fall_record_total;
extern volatile uint16_t g_gs358_fall_record_current_group_id;
extern volatile uint8_t  g_gs358_fall_record_current_group_count;
#endif

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

extern volatile uint32_t g_gs358_last_settlement_period_us;
extern volatile uint8_t  g_gs358_last_settlement_sample_count;
extern volatile uint8_t  g_gs358_last_settlement_valid_count;
extern volatile uint8_t  g_gs358_last_settlement_valid;


/* =========================== 接口函数 =========================== */

void GS358_AppInit(void);
void GS358_AppProcess(void);

void GS358_SetLedMode(GS358_LedMode mode);
GS358_LedMode GS358_GetLedMode(void);

void GS358_ComparatorFallingIRQHandler(void);
void GS358_PeriodWindowTimerIRQHandler(void);
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

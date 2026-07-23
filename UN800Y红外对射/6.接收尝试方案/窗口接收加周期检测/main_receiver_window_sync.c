/***********************************************************************
 * 文件名称：main.c
 * 工程用途：红外对射传感器接收端
 * 芯片平台：UM800Y / UM8004
 * 主频配置：24MHz
 *
 * 信号条件：
 * 1. 发射端红外信号周期约 1ms。
 * 2. 有效脉冲宽度约 25us，占空比约 2.5%。
 *
 * 接收策略：
 * 1. 上电先高速搜索有效脉冲。
 * 2. 找到两个间隔约 1ms 的脉冲后，进入同步跟踪。
 * 3. 同步后每 1ms 打开一个约 100us 的采样窗口。
 * 4. 窗口内高速读取 ADC，取峰值。
 * 5. 用 峰值 - 基线 得到真实脉冲幅度，避免环境光/偏置影响。
 * 6. 连续 2 个有效脉冲确认有光，连续 3 个无效窗口确认遮光。
 *
 * 直接使用方法：
 * 把本文件整体替换旧工程 Example/gpio/demo/source/main.c。
 ***********************************************************************/

#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"

/*======================================================================
 * 一、ADC 通道映射
 *====================================================================*/

/*
 * 接收信号 ADC 通道。
 * 旧版接收板：P1.5 / U3-1 为 LM358 输出，旧工程中按 ADC_CHANNEL_1 使用。
 * 如果烧录后发现接收 ADC 不变，而电位器会影响接收值，请和 IR_ADJ_ADC_CHANNEL 对调。
 */
#define IR_SIGNAL_ADC_CHANNEL       ADC_CHANNEL_1

/*
 * 电位器 ADJ ADC 通道。
 * 原理图中 ADJ 已移动到 P2.0；UM800Y 中 P2.0 复用 AIN2。
 */
#define IR_ADJ_ADC_CHANNEL          ADC_CHANNEL_2

/*======================================================================
 * 二、GPIO 引脚映射
 *====================================================================*/

#define IR_OUT_NO_PIN               P1_0    /* NO 常开输出 */
#define IR_OUT_NC_PIN               P1_2    /* NC 常闭输出 */
#define IR_LED_PIN                  P1_3    /* 红色指示灯 */
#define IR_DEBUG_WINDOW_PIN         P2_7    /* 调试脚：窗口打开时翻转，用示波器看对齐 */

/*======================================================================
 * 三、输出极性配置
 *====================================================================*/

#define OUT_ACTIVE_LEVEL            GPIO_HIGH
#define OUT_INACTIVE_LEVEL          GPIO_LOW

#define LED_ACTIVE_LEVEL            GPIO_HIGH
#define LED_INACTIVE_LEVEL          GPIO_LOW

/*
 * SENSOR_DARK_ON = 0：亮通，收到光时输出动作。
 * SENSOR_DARK_ON = 1：暗通，遮光时输出动作。
 */
#define SENSOR_DARK_ON              0U

/*
 * RX_SIGNAL_ACTIVE_HIGH = 1：ADC 变大表示收到红外脉冲。
 * RX_SIGNAL_ACTIVE_HIGH = 0：ADC 变小表示收到红外脉冲。
 */
#define RX_SIGNAL_ACTIVE_HIGH       1U

/*======================================================================
 * 四、1ms/25us 脉冲同步采样参数
 *====================================================================*/

#define IR_PERIOD_US                1000U
#define IR_PULSE_WIDTH_US           25U

/* 搜索阶段：两个脉冲间隔在这个范围内，认为是同一个 1ms 信号源。 */
#define IR_SEARCH_PERIOD_MIN_US     850U
#define IR_SEARCH_PERIOD_MAX_US     1150U

/* 跟踪阶段：预计脉冲中心前 40us 到后 60us 开窗，总窗口 100us。 */
#define IR_WINDOW_PRE_US            40U
#define IR_WINDOW_POST_US           60U
#define IR_WINDOW_WIDTH_US          (IR_WINDOW_PRE_US + IR_WINDOW_POST_US)

/* 响应速度：2ms 确认有光，3ms 确认遮光，8ms 丢同步重搜。 */
#define IR_LIGHT_ON_CONFIRM_COUNT   2U
#define IR_LIGHT_OFF_MISS_COUNT     3U
#define IR_LOST_SYNC_MISS_COUNT     8U
#define IR_SEARCH_LOCK_COUNT        2U

/* 单周期最大相位修正，防止被噪声拖跑。 */
#define IR_PHASE_ADJUST_LIMIT_US    5

/*======================================================================
 * 五、阈值/滤波参数
 *====================================================================*/

#define ADC_MAX_VALUE               4095U

/*
 * 注意：这里的阈值是“脉冲幅度阈值”，不是原始 ADC 绝对值。
 * 因为程序会先做 amp = peak - baseline。
 *
 * 距离不够：降低 IR_TH_MIN_ADC / IR_TH_MAX_ADC。
 * 误触发多：升高 IR_TH_MIN_ADC / IR_TH_MAX_ADC。
 */
#define IR_TH_MIN_ADC               80U
#define IR_TH_MAX_ADC               1800U
#define IR_TH_DEFAULT_ADC           600U
#define IR_TH_HYS_MIN_ADC           40U

#define IR_BASELINE_SHIFT           5U      /* 基线慢速跟随：1/32 */
#define IR_FAST_FILTER_SHIFT        1U      /* 输出快速幅度滤波：1/2 */
#define IR_DISPLAY_FILTER_SHIFT     3U      /* 显示慢速幅度滤波：1/8 */
#define IR_ADJ_FILTER_SHIFT         3U      /* 电位器阈值滤波：1/8 */
#define IR_ADJ_UPDATE_US            10000U  /* 10ms 更新一次电位器 */

/*======================================================================
 * 六、Timer0 1us 自由运行配置
 *====================================================================*/

/*
 * 24MHz / (23 + 1) = 1MHz，所以 Timer0 每 1us 加 1。
 * 自动重装 0xFFFF，自然 65.536ms 回绕；本程序所有时间差都按 uint16_t 回绕计算。
 */
#define IR_TIMER0_1US_RELOAD        0xFFFFU
#define IR_TIMER0_1US_PRESCALER     23U

/*======================================================================
 * 七、状态定义和调试变量
 *====================================================================*/

typedef enum
{
    IR_STATE_SEARCH = 0,
    IR_STATE_TRACKING = 1
} ir_state_t;

static volatile ir_state_t g_ir_state = IR_STATE_SEARCH;

/* 这些变量建议保留，方便 Keil 仿真/串口调试观察。 */
static volatile uint8_t  g_ir_light_ok = 0;
static volatile uint8_t  g_ir_synced = 0;
static volatile uint8_t  g_ir_output_state = 0;
static volatile uint16_t g_ir_adc_signal = 0;
static volatile uint16_t g_ir_adc_adj = 0;
static volatile uint16_t g_ir_threshold_on = IR_TH_DEFAULT_ADC;
static volatile uint16_t g_ir_threshold_off = (IR_TH_DEFAULT_ADC - 80U);
static volatile uint16_t g_ir_baseline = 0;
static volatile uint16_t g_ir_peak = 0;
static volatile uint16_t g_ir_amp = 0;
static volatile uint16_t g_ir_amp_fast = 0;
static volatile uint16_t g_ir_amp_display = 0;
static volatile uint16_t g_ir_next_center_us = 0;

static uint8_t  g_ir_good_count = 0;
static uint8_t  g_ir_miss_count = 0;
static uint8_t  g_ir_lock_count = 0;
static uint8_t  g_ir_in_pulse = 0;
static uint8_t  g_ir_have_last_pulse = 0;

static uint16_t g_ir_last_pulse_us = 0;
static uint16_t g_ir_search_start_us = 0;
static uint16_t g_ir_search_peak = 0;
static uint16_t g_ir_search_peak_us = 0;
static uint16_t g_ir_last_adj_update_us = 0;
static uint32_t g_ir_threshold_filter = ((uint32_t)IR_TH_DEFAULT_ADC << IR_ADJ_FILTER_SHIFT);
static uint8_t  g_ir_threshold_init = 0;

/*======================================================================
 * 八、函数声明
 *====================================================================*/

void GPIO_Init(void);
void ADC_Init(void);

static void timer0_init_1us_free_run(void);
static uint16_t time_us16(void);
static uint8_t time_after_eq_u16(uint16_t now, uint16_t target);
static uint16_t time_sub_u16(uint16_t a, uint16_t b);

static uint16_t adc_read_once(uint8_t ch);
static void ir_detector_init(void);
static void ir_detector_poll(void);
static void ir_set_search(void);
static void ir_search_sample(void);
static void ir_tracking_process(void);
static void ir_process_frame(uint16_t peak, uint16_t peak_us, uint16_t expected_center_us);

static uint16_t ir_abs_amp(uint16_t sample, uint16_t baseline);
static uint8_t ir_sample_stronger(uint16_t sample, uint16_t old_best);
static uint16_t ir_best_init_value(void);
static uint16_t ir_iir_u16(uint16_t old_v, uint16_t new_v, uint8_t shift);
static int16_t ir_limit_i16(int16_t x, int16_t min_v, int16_t max_v);
static void ir_update_baseline(uint16_t sample);
static void ir_update_threshold_from_adj(void);
static uint16_t threshold_map_from_adj(uint16_t adj);

static void output_apply(uint8_t light_ok);
static void debug_window_pin(uint8_t level);

/*======================================================================
 * 九、主函数
 *====================================================================*/

void main(void)
{
    system_init();

    GPIO_Init();
    uart_init();
    ADC_Init();

    timer0_init_1us_free_run();
    ir_detector_init();

    /* 全局关闭中断：本接收版本不依赖中断，避免其它中断影响 25us 脉冲捕捉。 */
    EA = 0;

    output_apply(0U);

    while(1)
    {
        ir_detector_poll();
    }
}

/*======================================================================
 * 十、GPIO 初始化
 *====================================================================*/

void GPIO_Init(void)
{
    /* P1.0 -> NO 常开输出。 */
    REG_P10_CFG = 0x00;
    gpio_init(IR_OUT_NO_PIN);
    gpio_dir_set(IR_OUT_NO_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NO_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);

    /* P1.2 -> NC 常闭输出。 */
    REG_P12_CFG = 0x00;
    gpio_init(IR_OUT_NC_PIN);
    gpio_dir_set(IR_OUT_NC_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NC_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);

    /* P1.3 -> 红色指示灯。 */
    REG_P13_CFG = 0x00;
    gpio_init(IR_LED_PIN);
    gpio_dir_set(IR_LED_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_LED_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);

    /* P2.7 -> 调试脚，窗口打开时置 1。 */
#ifdef REG_P27_CFG
    REG_P27_CFG = 0x00;
#endif
    gpio_init(IR_DEBUG_WINDOW_PIN);
    gpio_dir_set(IR_DEBUG_WINDOW_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_DEBUG_WINDOW_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_DEBUG_WINDOW_PIN, GPIO_LOW);

    /* P1.5 -> U3-1 接收信号，后面 adc_io_config() 会配置为 ADC 功能。 */
    REG_P15_CFG = 0x00;

    /* P2.0 -> ADJ 电位器，后面 adc_io_config() 会配置为 ADC 功能。 */
#ifdef REG_P20_CFG
    REG_P20_CFG = 0x00;
#endif
}

/*======================================================================
 * 十一、ADC 初始化
 *====================================================================*/

void ADC_Init(void)
{
    adc_clk_config(ADC_CLKSOURCE_SYSCLK,
                   ADC_VREFSOURCE_AVDD33,
                   4,
                   ADC_ENABLE);

    /* 采样时间短一些，方便窗口内尽量多采样。 */
    adc_sample_clk_config(ADC_SAMPCLK_4);

    adc_io_config(IR_SIGNAL_ADC_CHANNEL | IR_ADJ_ADC_CHANNEL);
    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);
}

/*======================================================================
 * 十二、Timer0 1us 自由运行
 *====================================================================*/

static void timer0_init_1us_free_run(void)
{
    gtimer0_count_init(IR_TIMER0_1US_RELOAD, IR_TIMER0_1US_PRESCALER);

#ifdef REG_GTIM0_IER
    REG_GTIM0_IER = 0x00;
#endif

    REG_GTIM0_SR = 0x07;
    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    gtimer0_start();
}

static uint16_t time_us16(void)
{
    uint8_t hi1;
    uint8_t hi2;
    uint8_t lo;

    do
    {
        hi1 = REG_GTIM0_CNT1;
        lo  = REG_GTIM0_CNT0;
        hi2 = REG_GTIM0_CNT1;
    } while(hi1 != hi2);

    return ((uint16_t)hi1 << 8) | (uint16_t)lo;
}

static uint16_t time_sub_u16(uint16_t a, uint16_t b)
{
    return (uint16_t)(a - b);
}

static uint8_t time_after_eq_u16(uint16_t now, uint16_t target)
{
    return (((int16_t)(now - target)) >= 0) ? 1U : 0U;
}

/*======================================================================
 * 十三、ADC 单次读取
 *====================================================================*/

static uint16_t adc_read_once(uint8_t ch)
{
    uint16_t value;

    adc_convert_start(ch);

    while((ADCGCR1 & 0x04) != 0);
    while(!(ADCCSTAT & 0x01));

    ADCCSTAT = 0x01;

    value = adc_get_value();

    if(value > ADC_MAX_VALUE)
    {
        value = ADC_MAX_VALUE;
    }

    return value;
}

/*======================================================================
 * 十四、接收检测初始化
 *====================================================================*/

static void ir_detector_init(void)
{
    uint16_t sample;

    sample = adc_read_once(IR_SIGNAL_ADC_CHANNEL);

    g_ir_state = IR_STATE_SEARCH;
    g_ir_light_ok = 0U;
    g_ir_synced = 0U;
    g_ir_output_state = 0U;

    g_ir_adc_signal = sample;
    g_ir_adc_adj = 0U;
    g_ir_threshold_on = IR_TH_DEFAULT_ADC;
    g_ir_threshold_off = (IR_TH_DEFAULT_ADC - 80U);
    g_ir_baseline = sample;
    g_ir_peak = sample;
    g_ir_amp = 0U;
    g_ir_amp_fast = 0U;
    g_ir_amp_display = 0U;
    g_ir_next_center_us = 0U;

    g_ir_good_count = 0U;
    g_ir_miss_count = 0U;
    g_ir_lock_count = 0U;
    g_ir_in_pulse = 0U;
    g_ir_have_last_pulse = 0U;
    g_ir_last_pulse_us = 0U;
    g_ir_search_start_us = 0U;
    g_ir_search_peak = sample;
    g_ir_search_peak_us = 0U;
    g_ir_last_adj_update_us = time_us16();
    g_ir_threshold_filter = ((uint32_t)IR_TH_DEFAULT_ADC << IR_ADJ_FILTER_SHIFT);
    g_ir_threshold_init = 0U;

    ir_update_threshold_from_adj();
}

static void ir_set_search(void)
{
    g_ir_state = IR_STATE_SEARCH;
    g_ir_synced = 0U;
    g_ir_good_count = 0U;
    g_ir_miss_count = 0U;
    g_ir_lock_count = 0U;
    g_ir_in_pulse = 0U;
    g_ir_have_last_pulse = 0U;
}

/*======================================================================
 * 十五、主检测轮询
 *====================================================================*/

static void ir_detector_poll(void)
{
    ir_update_threshold_from_adj();

    if(g_ir_state == IR_STATE_SEARCH)
    {
        ir_search_sample();
    }
    else
    {
        ir_tracking_process();
    }
}

/*======================================================================
 * 十六、搜索同步阶段
 *====================================================================*/

static void ir_search_sample(void)
{
    uint16_t now;
    uint16_t sample;
    uint16_t amp;
    uint16_t width_us;
    uint16_t dt;

    now = time_us16();
    sample = adc_read_once(IR_SIGNAL_ADC_CHANNEL);
    g_ir_adc_signal = sample;

    ir_update_baseline(sample);
    amp = ir_abs_amp(sample, g_ir_baseline);
    g_ir_amp = amp;

    if(g_ir_in_pulse == 0U)
    {
        if(amp > g_ir_threshold_on)
        {
            g_ir_in_pulse = 1U;
            g_ir_search_start_us = now;
            g_ir_search_peak = sample;
            g_ir_search_peak_us = now;
        }
        return;
    }

    if(ir_sample_stronger(sample, g_ir_search_peak))
    {
        g_ir_search_peak = sample;
        g_ir_search_peak_us = now;
    }

    if(amp < g_ir_threshold_off)
    {
        g_ir_in_pulse = 0U;
        width_us = time_sub_u16(now, g_ir_search_start_us);

        /* 真实脉冲约 25us，允许放宽到 120us；太宽多半是环境干扰或饱和。 */
        if(width_us > 120U)
        {
            g_ir_lock_count = 0U;
            g_ir_have_last_pulse = 0U;
            return;
        }

        if(g_ir_have_last_pulse == 0U)
        {
            g_ir_have_last_pulse = 1U;
            g_ir_last_pulse_us = g_ir_search_peak_us;
            return;
        }

        dt = time_sub_u16(g_ir_search_peak_us, g_ir_last_pulse_us);
        g_ir_last_pulse_us = g_ir_search_peak_us;

        if((dt >= IR_SEARCH_PERIOD_MIN_US) && (dt <= IR_SEARCH_PERIOD_MAX_US))
        {
            if(g_ir_lock_count < 255U)
            {
                g_ir_lock_count++;
            }
        }
        else
        {
            g_ir_lock_count = 0U;
        }

        if(g_ir_lock_count >= IR_SEARCH_LOCK_COUNT)
        {
            g_ir_state = IR_STATE_TRACKING;
            g_ir_synced = 1U;
            g_ir_good_count = 0U;
            g_ir_miss_count = 0U;
            g_ir_next_center_us = (uint16_t)(g_ir_search_peak_us + IR_PERIOD_US);
        }
    }
}

/*======================================================================
 * 十七、同步跟踪阶段：每 1ms 打窗口采样
 *====================================================================*/

static void ir_tracking_process(void)
{
    uint16_t center;
    uint16_t start;
    uint16_t end;
    uint16_t now;
    uint16_t sample;
    uint16_t best;
    uint16_t best_us;

    center = g_ir_next_center_us;
    start = (uint16_t)(center - IR_WINDOW_PRE_US);
    end = (uint16_t)(center + IR_WINDOW_POST_US);

    now = time_us16();

    if(!time_after_eq_u16(now, start))
    {
        sample = adc_read_once(IR_SIGNAL_ADC_CHANNEL);
        g_ir_adc_signal = sample;
        ir_update_baseline(sample);
        return;
    }

    if(time_after_eq_u16(now, end))
    {
        /* 主循环错过窗口，按一次丢脉冲处理。 */
        ir_process_frame(g_ir_baseline, now, center);
        return;
    }

    debug_window_pin(1U);

    best = ir_best_init_value();
    best_us = now;

    do
    {
        now = time_us16();
        sample = adc_read_once(IR_SIGNAL_ADC_CHANNEL);
        g_ir_adc_signal = sample;

        if(ir_sample_stronger(sample, best))
        {
            best = sample;
            best_us = now;
        }
    } while(!time_after_eq_u16(now, end));

    debug_window_pin(0U);

    ir_process_frame(best, best_us, center);
}

static void ir_process_frame(uint16_t peak, uint16_t peak_us, uint16_t expected_center_us)
{
    uint16_t amp;
    uint8_t valid;
    int16_t phase_error;
    int16_t adjust;

    g_ir_peak = peak;
    amp = ir_abs_amp(peak, g_ir_baseline);
    g_ir_amp = amp;

    g_ir_amp_fast = ir_iir_u16(g_ir_amp_fast, amp, IR_FAST_FILTER_SHIFT);
    g_ir_amp_display = ir_iir_u16(g_ir_amp_display, amp, IR_DISPLAY_FILTER_SHIFT);

    if(g_ir_light_ok != 0U)
    {
        valid = (g_ir_amp_fast > g_ir_threshold_off) ? 1U : 0U;
    }
    else
    {
        valid = (g_ir_amp_fast > g_ir_threshold_on) ? 1U : 0U;
    }

    if(valid != 0U)
    {
        if(g_ir_good_count < 255U)
        {
            g_ir_good_count++;
        }
        g_ir_miss_count = 0U;

        if(g_ir_good_count >= IR_LIGHT_ON_CONFIRM_COUNT)
        {
            if(g_ir_light_ok == 0U)
            {
                g_ir_light_ok = 1U;
                output_apply(1U);
            }
        }

        phase_error = (int16_t)((int16_t)peak_us - (int16_t)expected_center_us);
        adjust = ir_limit_i16((int16_t)(phase_error / 4),
                              (int16_t)-IR_PHASE_ADJUST_LIMIT_US,
                              (int16_t) IR_PHASE_ADJUST_LIMIT_US);

        g_ir_next_center_us = (uint16_t)(expected_center_us + IR_PERIOD_US + adjust);
    }
    else
    {
        g_ir_good_count = 0U;
        if(g_ir_miss_count < 255U)
        {
            g_ir_miss_count++;
        }

        g_ir_next_center_us = (uint16_t)(expected_center_us + IR_PERIOD_US);

        if(g_ir_miss_count >= IR_LIGHT_OFF_MISS_COUNT)
        {
            if(g_ir_light_ok != 0U)
            {
                g_ir_light_ok = 0U;
                output_apply(0U);
            }
        }

        if(g_ir_miss_count >= IR_LOST_SYNC_MISS_COUNT)
        {
            ir_set_search();
        }
    }
}

/*======================================================================
 * 十八、阈值、基线、滤波工具
 *====================================================================*/

static uint16_t ir_abs_amp(uint16_t sample, uint16_t baseline)
{
#if RX_SIGNAL_ACTIVE_HIGH
    return (sample > baseline) ? (uint16_t)(sample - baseline) : 0U;
#else
    return (baseline > sample) ? (uint16_t)(baseline - sample) : 0U;
#endif
}

static uint8_t ir_sample_stronger(uint16_t sample, uint16_t old_best)
{
#if RX_SIGNAL_ACTIVE_HIGH
    return (sample > old_best) ? 1U : 0U;
#else
    return (sample < old_best) ? 1U : 0U;
#endif
}

static uint16_t ir_best_init_value(void)
{
#if RX_SIGNAL_ACTIVE_HIGH
    return 0U;
#else
    return 0xFFFFU;
#endif
}

static uint16_t ir_iir_u16(uint16_t old_v, uint16_t new_v, uint8_t shift)
{
    if(new_v >= old_v)
    {
        return (uint16_t)(old_v + ((new_v - old_v) >> shift));
    }
    else
    {
        return (uint16_t)(old_v - ((old_v - new_v) >> shift));
    }
}

static int16_t ir_limit_i16(int16_t x, int16_t min_v, int16_t max_v)
{
    if(x < min_v)
    {
        return min_v;
    }

    if(x > max_v)
    {
        return max_v;
    }

    return x;
}

static void ir_update_baseline(uint16_t sample)
{
    uint16_t amp;

    amp = ir_abs_amp(sample, g_ir_baseline);

    /* 非脉冲区才允许基线慢速跟随，避免把脉冲峰值吃进基线。 */
    if(amp < g_ir_threshold_off)
    {
        g_ir_baseline = ir_iir_u16(g_ir_baseline, sample, IR_BASELINE_SHIFT);
    }
}

static void ir_update_threshold_from_adj(void)
{
    uint16_t now;
    uint16_t dt;
    uint16_t adj;
    uint16_t new_threshold;
    uint16_t hys;

    now = time_us16();
    dt = time_sub_u16(now, g_ir_last_adj_update_us);

    if((g_ir_threshold_init != 0U) && (dt < IR_ADJ_UPDATE_US))
    {
        return;
    }

    g_ir_last_adj_update_us = now;

    adj = adc_read_once(IR_ADJ_ADC_CHANNEL);
    if(adj > ADC_MAX_VALUE)
    {
        adj = ADC_MAX_VALUE;
    }
    g_ir_adc_adj = adj;

    new_threshold = threshold_map_from_adj(adj);

    if(g_ir_threshold_init == 0U)
    {
        g_ir_threshold_init = 1U;
        g_ir_threshold_filter = ((uint32_t)new_threshold << IR_ADJ_FILTER_SHIFT);
    }
    else
    {
        g_ir_threshold_filter -= (g_ir_threshold_filter >> IR_ADJ_FILTER_SHIFT);
        g_ir_threshold_filter += new_threshold;
    }

    g_ir_threshold_on = (uint16_t)(g_ir_threshold_filter >> IR_ADJ_FILTER_SHIFT);

    if(g_ir_threshold_on < IR_TH_MIN_ADC)
    {
        g_ir_threshold_on = IR_TH_MIN_ADC;
    }
    else if(g_ir_threshold_on > IR_TH_MAX_ADC)
    {
        g_ir_threshold_on = IR_TH_MAX_ADC;
    }

    hys = (uint16_t)(g_ir_threshold_on >> 3);
    if(hys < IR_TH_HYS_MIN_ADC)
    {
        hys = IR_TH_HYS_MIN_ADC;
    }

    g_ir_threshold_off = (g_ir_threshold_on > hys) ? (uint16_t)(g_ir_threshold_on - hys) : 0U;
}

static uint16_t threshold_map_from_adj(uint16_t adj)
{
    uint32_t span;
    uint16_t threshold;

    if(adj > ADC_MAX_VALUE)
    {
        adj = ADC_MAX_VALUE;
    }

    span = (uint32_t)(IR_TH_MAX_ADC - IR_TH_MIN_ADC);
    threshold = (uint16_t)(IR_TH_MIN_ADC + (((uint32_t)adj * span) / ADC_MAX_VALUE));

    return threshold;
}

/*======================================================================
 * 十九、输出控制
 *====================================================================*/

static void output_apply(uint8_t light_ok)
{
    uint8_t output_active;

#if SENSOR_DARK_ON
    output_active = (light_ok == 0U) ? 1U : 0U;
#else
    output_active = (light_ok != 0U) ? 1U : 0U;
#endif

    g_ir_output_state = output_active;

    if(output_active != 0U)
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_ACTIVE_LEVEL);
        gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);
        gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
    }

    if(light_ok != 0U)
    {
        gpio_io_set(IR_LED_PIN, LED_ACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);
    }
}

static void debug_window_pin(uint8_t level)
{
    if(level != 0U)
    {
        gpio_io_set(IR_DEBUG_WINDOW_PIN, GPIO_HIGH);
    }
    else
    {
        gpio_io_set(IR_DEBUG_WINDOW_PIN, GPIO_LOW);
    }
}

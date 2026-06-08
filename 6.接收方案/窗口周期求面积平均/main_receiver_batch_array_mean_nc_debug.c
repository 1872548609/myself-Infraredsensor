/***********************************************************************
 * 文件名称：main.c
 * 工程用途：工业级红外对射传感器接收端
 * 芯片平台：UM800Y / UM8004
 * 主频配置：24MHz
 *
 * 本版重构目标：
 *  1. 不再每读一次 ADC 就立刻判断输出。
 *  2. 改成“定时采集一帧 ADC 数组 -> 停止本帧采集 -> 统一均值化处理 -> 判断输出”。
 *  3. 对 1ms 周期、约 25us 宽度的窄脉冲，不能直接把整帧 ADC 做普通平均，
 *     否则 25us 脉冲会被 1.2ms 窗口严重平均掉。
 *  4. 本版采用：
 *        接收信号均值 = 一帧中最强的若干个 ADC 点的平均值；
 *        背景基线均值 = 一帧中最弱的若干个 ADC 点的平均值；
 *        有效幅度     = 接收信号均值 - 背景基线均值。
 *     这样既完成“均值化处理”，又不会把窄脉冲峰值抹掉。
 *
 * 工作流程：
 *  1. Timer0 配置成 1us 自由计数。
 *  2. 每一帧开始时清零 Timer0 计数器。
 *  3. 在 RX_CAPTURE_WINDOW_US 时间内连续读取接收 ADC，写入 g_adc_frame_buf[]。
 *  4. 到时间后结束本帧采集。
 *  5. 从数组里计算：最小值、最大值、整帧平均、背景均值、信号均值、脉冲幅度。
 *  6. 根据幅度阈值、有效点数、连续帧确认次数判断有光/遮光。
 *  7. 处理完成后继续下一帧采集。
 *
 * 硬件引脚对应：
 *  P1.5 / U3-1  -> LM358 输出，接收红外脉冲信号，ADC 输入
 *  P2.0 / ADJ   -> 电位器阈值调节，ADC 输入
 *  P1.0         -> NO 常开输出
 *  P1.2         -> NC 常闭输出；当 RX_NC_WINDOW_DEBUG_ENABLE=1 时改作采集窗口调试脚
 *  P1.3         -> 红色指示灯
 *
 * 重要说明：
 *  这版是“整帧数组处理”逻辑，稳定性比单点判断更好；
 *  但响应速度由 RX_CAPTURE_WINDOW_US 和确认帧数决定。
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
 * 一、编译器存储区适配
 *====================================================================*/

/*
 * UM800Y 是 8051 内核，内部 RAM 很小。
 * ADC 采样数组不要放在 idata 里，建议放到 xdata。
 * 如果你的编译器不是 Keil C51，IR_XDATA 会自动为空。
 */
#ifndef IR_XDATA
#if defined(__C51__) || defined(__CX51__)
#define IR_XDATA xdata
#else
#define IR_XDATA
#endif
#endif

/*======================================================================
 * 二、ADC 通道映射
 *====================================================================*/

/* 接收信号 ADC 通道：旧工程按 P1.5 / U3-1 / ADC_CHANNEL_1 使用。 */
#define IR_SIGNAL_ADC_CHANNEL           ADC_CHANNEL_1

/* 电位器 ADJ ADC 通道：原理图中 ADJ 在 P2.0，UM800Y 中 P2.0 复用 AIN2。 */
#define IR_ADJ_ADC_CHANNEL              ADC_CHANNEL_2

/*======================================================================
 * 三、GPIO 引脚映射
 *====================================================================*/

#define IR_OUT_NO_PIN                   P1_0    /* NO 常开输出 */
#define IR_OUT_NC_PIN                   P1_2    /* NC 常闭输出 */
#define IR_LED_PIN                      P1_3    /* 红色指示灯 */

/*======================================================================
 * 四、NC 窗口调试开关
 *====================================================================*/

/*
 * RX_NC_WINDOW_DEBUG_ENABLE = 1：
 *      NC 引脚不再作为正常 NC 输出，而是作为采集窗口调试脚。
 *      每一帧采集窗口开始时，NC 输出 OUT_ACTIVE_LEVEL；
 *      每一帧采集窗口结束时，NC 输出 OUT_INACTIVE_LEVEL。
 *      这样可以用示波器观察“本帧 ADC 数组采集时间”。
 *
 * RX_NC_WINDOW_DEBUG_ENABLE = 0：
 *      NC 引脚恢复正常 NC 输出功能。
 */
#define RX_NC_WINDOW_DEBUG_ENABLE       1U

/*======================================================================
 * 五、输出极性配置
 *====================================================================*/

#define OUT_ACTIVE_LEVEL                GPIO_HIGH
#define OUT_INACTIVE_LEVEL              GPIO_LOW

#define LED_ACTIVE_LEVEL                GPIO_HIGH
#define LED_INACTIVE_LEVEL              GPIO_LOW

/* 0=亮通：有光输出动作；1=暗通：遮光输出动作。 */
#define SENSOR_DARK_ON                  0U

/* 1=ADC 变大代表红外脉冲；0=ADC 变小代表红外脉冲。 */
#define RX_SIGNAL_ACTIVE_HIGH           1U

/*======================================================================
 * 六、Timer0 1us 采集定时参数
 *====================================================================*/

/* 24MHz / (23 + 1) = 1MHz，所以 Timer0 每 1us 加 1。 */
#define TIMER0_1US_RELOAD               0xFFFFU
#define TIMER0_1US_PRESCALER            23U

/*
 * 每帧采集窗口。
 * 发射周期约 1ms，脉冲宽度约 25us。
 *
 * 如果只采 1ms，边界处可能刚好把脉冲切开。
 * 这里默认采 1.2ms，基本能覆盖至少一个完整脉冲。
 *
 * 响应速度估算：
 *  1.2ms 一帧；
 *  连续 2 帧确认有光约 2.4ms，约 416Hz；
 *  连续 5 帧确认遮光约 6ms，约 166Hz。
 */
#define RX_CAPTURE_WINDOW_US            1200U

/* 一帧最多保存多少个 ADC 点。384 点占用约 768 字节 xdata。 */
#define RX_ADC_FRAME_MAX_SAMPLES        384U

/* 如果一帧采样点数太少，说明 ADC 或定时器异常，本帧直接判无效。 */
#define RX_FRAME_MIN_SAMPLES            16U

/*======================================================================
 * 七、数组均值化处理参数
 *====================================================================*/

/*
 * 信号均值使用一帧里最强的若干个点。
 * 25us 脉冲内通常能采到多个点，取最强点平均可抗单点尖峰。
 */
#define RX_SIGNAL_TOP_AVG_COUNT         6U

/*
 * 背景基线均值使用一帧里最弱的若干个点。
 * 因为一帧中绝大部分时间都没有红外脉冲，所以低端样本通常代表环境背景。
 */
#define RX_BASELINE_AVG_COUNT           24U

/* 一帧中至少要有多少个点超过幅度阈值，才认为真的有脉冲。 */
#define RX_ACTIVE_POINT_MIN             2U

/*
 * 有效点数量上限百分比。
 * 真实 25us / 1200us 约 2.1%。考虑运放拖尾和采样误差，放宽到 35%。
 * 如果一帧中超过阈值的点太多，通常不是 25us 脉冲，而是饱和、环境干扰或基线错误。
 */
#define RX_ACTIVE_POINT_MAX_PERCENT     35U

/*======================================================================
 * 八、阈值和输出确认参数
 *====================================================================*/

#define ADC_MAX_VALUE                   4095U

/*
 * 注意：本版 g_threshold 表示“脉冲幅度阈值”，不是原始 ADC 绝对阈值。
 * 即判断条件是：
 *      g_frame_amp = 信号均值 - 背景均值
 *      g_frame_amp >= g_threshold
 *
 * 你当前工程原来把 MIN/MAX 都设为 800，相当于固定阈值 800。
 * 如果希望电位器真正调节灵敏度，可以把 MAX 改大，例如 1800 或 2500。
 */
#define THRESHOLD_MIN_ADC               800U
#define THRESHOLD_MAX_ADC               800U
#define THRESHOLD_DEFAULT_ADC           800U

/* 输出回差，避免临界距离附近一帧有光一帧无光。 */
#define RX_AMP_HYS_ADC                  100U

/* 连续多少帧有效后确认有光。 */
#define RX_FRAME_ON_CONFIRM_COUNT       2U

/* 连续多少帧无效后确认遮光。 */
#define RX_FRAME_OFF_CONFIRM_COUNT      5U

/* ADJ 更新周期。单位：帧。 */
#define ADJ_UPDATE_PERIOD_MS            100U
#define ADJ_UPDATE_FRAME_COUNT          ((uint16_t)((((uint32_t)ADJ_UPDATE_PERIOD_MS * 1000UL) + RX_CAPTURE_WINDOW_US - 1UL) / RX_CAPTURE_WINDOW_US))

/* ADJ 阈值 IIR 平滑：1/8。 */
#define ADJ_FILTER_SHIFT                3U

/*======================================================================
 * 九、串口调试开关
 *====================================================================*/

#define UART_ADC_DEBUG_ENABLE           1U
#define UART_ADC_DEBUG_PERIOD_MS        100U
#define UART_ADC_DEBUG_FRAME_COUNT      ((uint16_t)((((uint32_t)UART_ADC_DEBUG_PERIOD_MS * 1000UL) + RX_CAPTURE_WINDOW_US - 1UL) / RX_CAPTURE_WINDOW_US))

/*======================================================================
 * 十、全局变量
 *====================================================================*/

/* ADC 原始采样数组。 */
static uint16_t IR_XDATA g_adc_frame_buf[RX_ADC_FRAME_MAX_SAMPLES];

/* 用于均值化处理的小数组。 */
static uint16_t g_proc_signal_buf[RX_SIGNAL_TOP_AVG_COUNT];
static uint16_t g_proc_base_buf[RX_BASELINE_AVG_COUNT];

/* 调试观察变量。 */
static volatile uint16_t g_adc_signal = 0;
static volatile uint16_t g_adc_adj = 0;
static volatile uint16_t g_threshold = THRESHOLD_DEFAULT_ADC;

static volatile uint16_t g_frame_sample_count = 0;
static volatile uint8_t  g_frame_overflow = 0;
static volatile uint16_t g_frame_min = 0;
static volatile uint16_t g_frame_max = 0;
static volatile uint16_t g_frame_mean = 0;
static volatile uint16_t g_frame_baseline_mean = 0;
static volatile uint16_t g_frame_signal_mean = 0;
static volatile uint16_t g_frame_amp = 0;
static volatile uint16_t g_frame_active_points = 0;
static volatile uint8_t  g_frame_valid = 0;

/* 当前是否确认收到光。 */
static uint8_t g_light_ok = 0;

/* 当前输出动作状态，方便调试。 */
static uint8_t g_output_state = 0;

/* 连续有效/无效帧计数。 */
static uint8_t g_valid_frame_count = 0;
static uint8_t g_invalid_frame_count = 0;

/* 阈值滤波变量。 */
static uint32_t g_threshold_filter = ((uint32_t)THRESHOLD_DEFAULT_ADC << ADJ_FILTER_SHIFT);
static uint8_t  g_threshold_init = 0;
static uint16_t g_adj_update_frame_count = 0;

#if UART_ADC_DEBUG_ENABLE
static uint16_t g_uart_debug_frame_count = 0;
#endif

/*======================================================================
 * 十一、函数声明
 *====================================================================*/

void GPIO_Init(void);
void ADC_Init(void);

static void timer0_init_1us_capture(void);
static void capture_timer_start(void);
static void capture_timer_close(void);
static uint16_t capture_timer_us(void);

static uint16_t adc_read_once(uint8_t ch);

static void rx_capture_frame(void);
static void rx_process_frame(void);
static void rx_update_output_state(void);

static void threshold_update_process(void);
static uint16_t threshold_map_from_adj(uint16_t adj);

static uint16_t mean_top_high_samples(uint16_t *buf, uint16_t count, uint8_t top_n);
static uint16_t mean_low_samples(uint16_t *buf, uint16_t count, uint8_t low_n);
static uint16_t mean_top_low_samples(uint16_t *buf, uint16_t count, uint8_t low_n);
static uint16_t mean_high_baseline_samples(uint16_t *buf, uint16_t count, uint8_t high_n);
static uint16_t count_active_points(uint16_t *buf, uint16_t count, uint16_t baseline, uint16_t threshold);
static uint16_t calc_threshold_off(void);

static void output_apply(uint8_t light_ok);
static void nc_window_debug_set(uint8_t on);

#if UART_ADC_DEBUG_ENABLE
static void uart_adc_debug_process(void);
#endif

/*======================================================================
 * 十二、主函数
 *====================================================================*/

void main(void)
{
    system_init();

    GPIO_Init();

#if UART_ADC_DEBUG_ENABLE
    uart_init();
#endif

    ADC_Init();
    timer0_init_1us_capture();

    threshold_update_process();

    /* 本版不使用中断，Timer0 只做轮询计时。 */
    EA = 0;

    output_apply(0U);

    while(1)
    {
        /*
         * 1. 打开一帧采集。
         * 2. 在固定时间内连续读取 ADC 并写入数组。
         * 3. 到时间后结束本帧采集。
         */
        rx_capture_frame();

        /*
         * 4. 对数组里的 ADC 值做均值化处理。
         * 5. 根据处理结果判断有光/遮光。
         */
        rx_process_frame();
        rx_update_output_state();

        /*
         * ADJ 不在采集窗口中读取，避免抢占接收 ADC 时间。
         * 每若干帧更新一次即可。
         */
        g_adj_update_frame_count++;
        if(g_adj_update_frame_count >= ADJ_UPDATE_FRAME_COUNT)
        {
            g_adj_update_frame_count = 0U;
            threshold_update_process();
        }

#if UART_ADC_DEBUG_ENABLE
        uart_adc_debug_process();
#endif
    }
}

/*======================================================================
 * 十三、GPIO 初始化
 *====================================================================*/

void GPIO_Init(void)
{
    REG_P10_CFG = 0x00;
    gpio_init(IR_OUT_NO_PIN);
    gpio_dir_set(IR_OUT_NO_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NO_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);

    REG_P12_CFG = 0x00;
    gpio_init(IR_OUT_NC_PIN);
    gpio_dir_set(IR_OUT_NC_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NC_PIN, GPIO_SR_HIGH);
#if RX_NC_WINDOW_DEBUG_ENABLE
    /* 调试模式下，NC 先保持关闭，采集窗口开始时再打开。 */
    gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
#else
    /* 正常模式下，NC 作为常闭输出，初始为闭合/动作状态。 */
    gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
#endif

    REG_P13_CFG = 0x00;
    gpio_init(IR_LED_PIN);
    gpio_dir_set(IR_LED_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_LED_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);

    /* P1.5 接收信号，后续 adc_io_config() 配置为 ADC。 */
    REG_P15_CFG = 0x00;

#ifdef REG_P20_CFG
    /* P2.0 ADJ，后续 adc_io_config() 配置为 ADC。 */
    REG_P20_CFG = 0x00;
#endif
}

/*======================================================================
 * 十四、ADC 初始化
 *====================================================================*/

void ADC_Init(void)
{
    adc_clk_config(ADC_CLKSOURCE_SYSCLK,
                   ADC_VREFSOURCE_AVDD33,
                   4,
                   ADC_ENABLE);

    /* 保持较短采样时间，保证一帧内能采到足够多的点。 */
    adc_sample_clk_config(ADC_SAMPCLK_6);

    adc_io_config(IR_SIGNAL_ADC_CHANNEL | IR_ADJ_ADC_CHANNEL);
    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);
}

/*======================================================================
 * 十五、Timer0 1us 采集计时
 *====================================================================*/

static void timer0_init_1us_capture(void)
{
    gtimer0_count_init(TIMER0_1US_RELOAD, TIMER0_1US_PRESCALER);

#ifdef REG_GTIM0_IER
    REG_GTIM0_IER = 0x00;
#endif

    REG_GTIM0_SR = 0x07;
    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    gtimer0_start();
}

static void capture_timer_start(void)
{
    /*
     * 每一帧重新从 0 开始计时。
     * 这里相当于打开本帧采集定时。
     */
    REG_GTIM0_SR = 0x07;
    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;
    gtimer0_start();
}

static void capture_timer_close(void)
{
    /*
     * 旧工程库里只使用 gtimer0_start()，没有统一暴露 stop 接口。
     * 所以这里做“逻辑关闭”：本帧结束后不再读取 Timer0 判断采集时间，
     * 下一帧开始时重新清零计数器。
     */
    REG_GTIM0_SR = 0x07;
}

static uint16_t capture_timer_us(void)
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

/*======================================================================
 * 十六、ADC 单次读取
 *====================================================================*/

static uint16_t adc_read_once(uint8_t ch)
{
    uint16_t value;

    adc_convert_start(ch);

    while((ADCGCR1 & 0x04) != 0);
    while(!(ADCCSTAT & 0x01));

    ADCCSTAT = 0x01;

    value = (((uint16_t)(ADCDR1 & 0x0F)) << 8) | ADCDR0;

    if(value > ADC_MAX_VALUE)
    {
        value = ADC_MAX_VALUE;
    }

    return value;
}

/*======================================================================
 * 十七、采集一帧 ADC 数组
 *====================================================================*/

static void rx_capture_frame(void)
{
    uint16_t sample;
    uint16_t count;

    count = 0U;
    g_frame_overflow = 0U;

    capture_timer_start();

    /*
     * 调试模式下：从采集窗口真正开始时打开 NC。
     * 示波器看到的 NC 高/低电平宽度，就是本帧 ADC 数组采集时间。
     */
    nc_window_debug_set(1U);

    while(capture_timer_us() < RX_CAPTURE_WINDOW_US)
    {
        sample = adc_read_once(IR_SIGNAL_ADC_CHANNEL);
        g_adc_signal = sample;

        if(count < RX_ADC_FRAME_MAX_SAMPLES)
        {
            g_adc_frame_buf[count] = sample;
            count++;
        }
        else
        {
            /* 数组满了仍然等到本帧时间结束，但后续样本不再写入，避免越界。 */
            g_frame_overflow = 1U;
        }
    }

    capture_timer_close();

    /* 调试模式下：窗口读取结束，立刻关闭 NC。 */
    nc_window_debug_set(0U);

    g_frame_sample_count = count;
}

/*======================================================================
 * 十八、处理一帧 ADC 数组
 *====================================================================*/

static void rx_process_frame(void)
{
    uint32_t sum;
    uint16_t i;
    uint16_t count;
    uint16_t baseline;
    uint16_t signal_mean;
    uint16_t threshold_on;
    uint16_t threshold_off;
    uint16_t active_points;
    uint16_t active_max;
    uint8_t valid;

    count = g_frame_sample_count;
    valid = 0U;

    if(count < RX_FRAME_MIN_SAMPLES)
    {
        g_frame_valid = 0U;
        g_frame_amp = 0U;
        return;
    }

    g_frame_min = g_adc_frame_buf[0];
    g_frame_max = g_adc_frame_buf[0];
    sum = 0UL;

    for(i = 0U; i < count; i++)
    {
        if(g_adc_frame_buf[i] < g_frame_min)
        {
            g_frame_min = g_adc_frame_buf[i];
        }

        if(g_adc_frame_buf[i] > g_frame_max)
        {
            g_frame_max = g_adc_frame_buf[i];
        }

        sum += g_adc_frame_buf[i];
    }

    g_frame_mean = (uint16_t)(sum / count);

#if RX_SIGNAL_ACTIVE_HIGH
    /* 高脉冲有效：强信号取高端均值，背景取低端均值。 */
    signal_mean = mean_top_high_samples(g_adc_frame_buf, count, RX_SIGNAL_TOP_AVG_COUNT);
    baseline = mean_low_samples(g_adc_frame_buf, count, RX_BASELINE_AVG_COUNT);

    if(signal_mean > baseline)
    {
        g_frame_amp = (uint16_t)(signal_mean - baseline);
    }
    else
    {
        g_frame_amp = 0U;
    }
#else
    /* 低脉冲有效：强信号取低端均值，背景取高端均值。 */
    signal_mean = mean_top_low_samples(g_adc_frame_buf, count, RX_SIGNAL_TOP_AVG_COUNT);
    baseline = mean_high_baseline_samples(g_adc_frame_buf, count, RX_BASELINE_AVG_COUNT);

    if(baseline > signal_mean)
    {
        g_frame_amp = (uint16_t)(baseline - signal_mean);
    }
    else
    {
        g_frame_amp = 0U;
    }
#endif

    g_frame_baseline_mean = baseline;
    g_frame_signal_mean = signal_mean;

    threshold_on = g_threshold;
    threshold_off = calc_threshold_off();

    if(g_light_ok != 0U)
    {
        active_points = count_active_points(g_adc_frame_buf, count, baseline, threshold_off);
        if(g_frame_amp >= threshold_off)
        {
            valid = 1U;
        }
    }
    else
    {
        active_points = count_active_points(g_adc_frame_buf, count, baseline, threshold_on);
        if(g_frame_amp >= threshold_on)
        {
            valid = 1U;
        }
    }

    g_frame_active_points = active_points;

    active_max = (uint16_t)(((uint32_t)count * RX_ACTIVE_POINT_MAX_PERCENT) / 100UL);
    if(active_max < RX_ACTIVE_POINT_MIN)
    {
        active_max = RX_ACTIVE_POINT_MIN;
    }

    /* 幅度满足，但有效点太少，可能是单点毛刺。 */
    if(active_points < RX_ACTIVE_POINT_MIN)
    {
        valid = 0U;
    }

    /* 有效点太多，不像 25us 窄脉冲，可能是饱和或环境干扰。 */
    if(active_points > active_max)
    {
        valid = 0U;
    }

    g_frame_valid = valid;
}

/*======================================================================
 * 十九、根据连续帧结果更新输出
 *====================================================================*/

static void rx_update_output_state(void)
{
    if(g_frame_valid != 0U)
    {
        g_invalid_frame_count = 0U;
        if(g_valid_frame_count < 255U)
        {
            g_valid_frame_count++;
        }

        if(g_valid_frame_count >= RX_FRAME_ON_CONFIRM_COUNT)
        {
            if(g_light_ok == 0U)
            {
                g_light_ok = 1U;
                output_apply(1U);
            }
        }
    }
    else
    {
        g_valid_frame_count = 0U;
        if(g_invalid_frame_count < 255U)
        {
            g_invalid_frame_count++;
        }

        if(g_invalid_frame_count >= RX_FRAME_OFF_CONFIRM_COUNT)
        {
            if(g_light_ok != 0U)
            {
                g_light_ok = 0U;
                output_apply(0U);
            }
        }
    }
}

/*======================================================================
 * 二十、均值化工具函数
 *====================================================================*/

static uint16_t mean_top_high_samples(uint16_t *buf, uint16_t count, uint8_t top_n)
{
    uint16_t i;
    uint8_t j;
    uint8_t used;
    uint8_t min_index;
    uint32_t sum;

    used = 0U;

    for(i = 0U; i < count; i++)
    {
        if(used < top_n)
        {
            g_proc_signal_buf[used] = buf[i];
            used++;
        }
        else
        {
            min_index = 0U;
            for(j = 1U; j < top_n; j++)
            {
                if(g_proc_signal_buf[j] < g_proc_signal_buf[min_index])
                {
                    min_index = j;
                }
            }

            if(buf[i] > g_proc_signal_buf[min_index])
            {
                g_proc_signal_buf[min_index] = buf[i];
            }
        }
    }

    if(used == 0U)
    {
        return 0U;
    }

    sum = 0UL;
    for(j = 0U; j < used; j++)
    {
        sum += g_proc_signal_buf[j];
    }

    return (uint16_t)(sum / used);
}

static uint16_t mean_low_samples(uint16_t *buf, uint16_t count, uint8_t low_n)
{
    uint16_t i;
    uint8_t j;
    uint8_t used;
    uint8_t max_index;
    uint32_t sum;

    used = 0U;

    for(i = 0U; i < count; i++)
    {
        if(used < low_n)
        {
            g_proc_base_buf[used] = buf[i];
            used++;
        }
        else
        {
            max_index = 0U;
            for(j = 1U; j < low_n; j++)
            {
                if(g_proc_base_buf[j] > g_proc_base_buf[max_index])
                {
                    max_index = j;
                }
            }

            if(buf[i] < g_proc_base_buf[max_index])
            {
                g_proc_base_buf[max_index] = buf[i];
            }
        }
    }

    if(used == 0U)
    {
        return 0U;
    }

    sum = 0UL;
    for(j = 0U; j < used; j++)
    {
        sum += g_proc_base_buf[j];
    }

    return (uint16_t)(sum / used);
}

static uint16_t mean_top_low_samples(uint16_t *buf, uint16_t count, uint8_t low_n)
{
    /* 低脉冲有效时，最强信号就是最低的若干个点。 */
    return mean_low_samples(buf, count, low_n);
}

static uint16_t mean_high_baseline_samples(uint16_t *buf, uint16_t count, uint8_t high_n)
{
    /* 低脉冲有效时，背景基线就是最高的若干个点。 */
    return mean_top_high_samples(buf, count, high_n);
}

static uint16_t count_active_points(uint16_t *buf, uint16_t count, uint16_t baseline, uint16_t threshold)
{
    uint16_t i;
    uint16_t cnt;
    uint16_t th;

    cnt = 0U;

#if RX_SIGNAL_ACTIVE_HIGH
    if((uint32_t)baseline + threshold > ADC_MAX_VALUE)
    {
        th = ADC_MAX_VALUE;
    }
    else
    {
        th = (uint16_t)(baseline + threshold);
    }

    for(i = 0U; i < count; i++)
    {
        if(buf[i] >= th)
        {
            cnt++;
        }
    }
#else
    if(baseline > threshold)
    {
        th = (uint16_t)(baseline - threshold);
    }
    else
    {
        th = 0U;
    }

    for(i = 0U; i < count; i++)
    {
        if(buf[i] <= th)
        {
            cnt++;
        }
    }
#endif

    return cnt;
}

static uint16_t calc_threshold_off(void)
{
    if(g_threshold > RX_AMP_HYS_ADC)
    {
        return (uint16_t)(g_threshold - RX_AMP_HYS_ADC);
    }

    return 0U;
}

/*======================================================================
 * 二十一、ADJ 阈值更新
 *====================================================================*/

static void threshold_update_process(void)
{
    uint16_t adj;
    uint16_t new_threshold;

    adj = adc_read_once(IR_ADJ_ADC_CHANNEL);
    if(adj > ADC_MAX_VALUE)
    {
        adj = ADC_MAX_VALUE;
    }

    g_adc_adj = adj;
    new_threshold = threshold_map_from_adj(adj);

    if(g_threshold_init == 0U)
    {
        g_threshold_init = 1U;
        g_threshold_filter = ((uint32_t)new_threshold << ADJ_FILTER_SHIFT);
        g_threshold = new_threshold;
        return;
    }

    g_threshold_filter -= (g_threshold_filter >> ADJ_FILTER_SHIFT);
    g_threshold_filter += new_threshold;

    g_threshold = (uint16_t)(g_threshold_filter >> ADJ_FILTER_SHIFT);

    if(g_threshold < THRESHOLD_MIN_ADC)
    {
        g_threshold = THRESHOLD_MIN_ADC;
    }
    else if(g_threshold > THRESHOLD_MAX_ADC)
    {
        g_threshold = THRESHOLD_MAX_ADC;
    }
}

static uint16_t threshold_map_from_adj(uint16_t adj)
{
    uint32_t span;
    uint16_t threshold;

    if(adj > ADC_MAX_VALUE)
    {
        adj = ADC_MAX_VALUE;
    }

    span = (uint32_t)(THRESHOLD_MAX_ADC - THRESHOLD_MIN_ADC);
    threshold = (uint16_t)(THRESHOLD_MIN_ADC + (((uint32_t)adj * span) / ADC_MAX_VALUE));

    return threshold;
}

/*======================================================================
 * 二十二、串口调试输出
 *====================================================================*/

#if UART_ADC_DEBUG_ENABLE
static void uart_adc_debug_process(void)
{
    g_uart_debug_frame_count++;

    if(g_uart_debug_frame_count < UART_ADC_DEBUG_FRAME_COUNT)
    {
        return;
    }

    g_uart_debug_frame_count = 0U;

    printfS("S=%u,A=%u,TH=%u,N=%u,MIN=%u,MAX=%u,MEAN=%u,BASE=%u,SIG=%u,AMP=%u,AP=%u,V=%u,ON=%u,OFF=%u,L=%u,O=%u,OV=%u\r\n",
            g_adc_signal,
            g_adc_adj,
            g_threshold,
            g_frame_sample_count,
            g_frame_min,
            g_frame_max,
            g_frame_mean,
            g_frame_baseline_mean,
            g_frame_signal_mean,
            g_frame_amp,
            g_frame_active_points,
            (uint16_t)g_frame_valid,
            (uint16_t)g_valid_frame_count,
            (uint16_t)g_invalid_frame_count,
            (uint16_t)g_light_ok,
            (uint16_t)g_output_state,
            (uint16_t)g_frame_overflow);
}
#endif

/*======================================================================
 * 二十三、输出控制
 *====================================================================*/

static void output_apply(uint8_t light_ok)
{
    uint8_t output_active;

#if SENSOR_DARK_ON
    output_active = (light_ok != 0U) ? 0U : 1U;
#else
    output_active = (light_ok != 0U) ? 1U : 0U;
#endif

    if(output_active != 0U)
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_ACTIVE_LEVEL);
#if !RX_NC_WINDOW_DEBUG_ENABLE
        gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
#endif
    }
    else
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);
#if !RX_NC_WINDOW_DEBUG_ENABLE
        gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
#endif
    }

    if(light_ok != 0U)
    {
        gpio_io_set(IR_LED_PIN, LED_ACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);
    }

    g_output_state = output_active;
}

/*======================================================================
 * 二十四、NC 采集窗口调试输出
 *====================================================================*/

static void nc_window_debug_set(uint8_t on)
{
#if RX_NC_WINDOW_DEBUG_ENABLE
    if(on != 0U)
    {
        gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
    }
#else
    (void)on;
#endif
}

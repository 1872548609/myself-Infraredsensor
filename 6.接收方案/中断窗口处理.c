/***********************************************************************
 * 工业红外对射 - 接收端
 *
 * 修正版：
 * 1. ADC 不用中断，主循环轮询 ADC。
 * 2. 主循环连续扫描到 2 次有效脉冲后，认为同步成功。
 * 3. 同步成功后启动 GTIMER0。
 * 4. GTIMER0 不在 main.c 里重写 interrupt 21。
 * 5. 按仓库原驱动方式，使用 gtimer0_irq_init() 注册回调函数。
 * 6. 定时器使用 24MHz / 24 = 1MHz，1 个计数 = 1us。
 * 7. 定时器只在两个点进入回调：
 *    - 打开采样窗口
 *    - 关闭采样窗口
 * 8. 窗口期间不频繁中断，窗口内 ADC 由主循环连续扫描。
 *
 * 引脚：
 * P1.5 / ADC_CHANNEL_1 -> 接收信号 U3-1
 * P2.0 / ADC_CHANNEL_2 -> ADJ 电位器阈值
 * P1.0 -> NO 常开输出
 * P1.2 -> NC 采样窗口观察信号
 * P1.3 -> 红色指示灯
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
 * 一、ADC 通道配置
 *====================================================================*/

#define IR_SIGNAL_ADC_CHANNEL       ADC_CHANNEL_1
#define IR_ADJ_ADC_CHANNEL          ADC_CHANNEL_2

/*======================================================================
 * 二、GPIO 引脚配置
 *====================================================================*/

#define IR_OUT_NO_PIN               P1_0
#define IR_OUT_NC_PIN               P1_2
#define IR_LED_PIN                  P1_3

#define OUT_ACTIVE_LEVEL            GPIO_HIGH
#define OUT_INACTIVE_LEVEL          GPIO_LOW

#define LED_ACTIVE_LEVEL            GPIO_HIGH
#define LED_INACTIVE_LEVEL          GPIO_LOW

/*
 * 0：亮通模式，收到光时 NO 动作
 * 1：暗通模式，遮光时 NO 动作
 */
#define SENSOR_DARK_ON              0

/*
 * 1：NC 作为采样窗口观察信号
 * 0：NC 正常作为常闭输出
 */
#define DEBUG_NC_SAMPLE_WINDOW      1

/*======================================================================
 * 三、接收信号极性
 *====================================================================*/

/*
 * 1：ADC 变大表示收到红外脉冲
 * 0：ADC 变小表示收到红外脉冲
 */
#define RX_SIGNAL_ACTIVE_HIGH       1

/*======================================================================
 * 四、定时器参数
 *====================================================================*/

/*
 * GTIMER0 24 分频：
 * 24MHz / 24 = 1MHz
 * 1 个计数 = 1us
 */
#define TIMER0_US_PRESCALER         (24U - 1U)

/*
 * 发射周期，单位 us。
 */
#define SAMPLE_INTERVAL_US          930U

/*
 * 同步成功后，第一次打开窗口的偏移。
 *
 * 第 2 个有效脉冲被主循环抓到后，
 * 下一个脉冲约在 1000us 后到来。
 */
#define SAMPLE_OFFSET_US            0U
#define SAMPLE_FIRST_DELAY_US       (SAMPLE_INTERVAL_US + SAMPLE_OFFSET_US)

/*
 * 采样窗口宽度，单位 us。
 * NC 高电平宽度就是这个值。
 */
#define SAMPLE_WINDOW_US            120U

/*
 * 窗口关闭后，到下一个窗口打开之间的时间。
 */
#define SAMPLE_REST_US              (SAMPLE_INTERVAL_US - SAMPLE_WINDOW_US)

/*
 * 每个窗口内最多采样次数。
 */
#define SAMPLE_COUNT_PER_WINDOW     8U

/*
 * 每隔多少个窗口重新同步一次。
 */
#define RESYNC_EVERY_WINDOWS        10U

/*======================================================================
 * 五、同步和判断参数
 *====================================================================*/

#define SYNC_REQUIRED_PULSES        2U

#define VALID_CONFIRM_COUNT         2U
#define MISS_CONFIRM_COUNT          5U
#define MISS_RESYNC_COUNT           8U

/*======================================================================
 * 六、阈值参数
 *====================================================================*/

#define ADC_MAX_VALUE               4095U

#define THRESHOLD_MIN_ADC           650U
#define THRESHOLD_MAX_ADC           2800U
#define THRESHOLD_DEFAULT_ADC       1300U

#define RX_ON_MARGIN_ADC            80U
#define RX_OFF_MARGIN_ADC           80U

#define ADJ_FILTER_SHIFT            3U

/*
 * 搜索阶段每扫描多少次接收 ADC 后更新一次 ADJ。
 */
#define SEARCH_ADJ_UPDATE_DIV       800U

/*
 * 锁定阶段默认不更新 ADJ，避免影响窗口。
 */
#define LOCKED_ENABLE_ADJ_UPDATE    0
#define LOCKED_ADJ_UPDATE_WINDOWS   50U

/*======================================================================
 * 七、状态机
 *====================================================================*/

#define RX_STATE_SEARCH             0U
#define RX_STATE_LOCKED             1U

#define TIMER_EVENT_NONE            0U
#define TIMER_EVENT_OPEN_WINDOW     1U
#define TIMER_EVENT_CLOSE_WINDOW    2U

static volatile uint8_t  g_rx_state = RX_STATE_SEARCH;
static volatile uint8_t  g_timer_event = TIMER_EVENT_NONE;

static volatile uint8_t  g_window_open = 0;
static volatile uint8_t  g_window_result_ready = 0;
static volatile uint8_t  g_resync_request = 0;
static volatile uint8_t  g_window_overrun = 0;

static volatile uint8_t  g_window_sample_count = 0;
static volatile uint16_t g_window_peak = 0;

static volatile uint8_t  g_last_window_sample_count = 0;
static volatile uint16_t g_last_window_peak = 0;

static volatile uint16_t g_window_count_since_sync = 0;

static uint16_t g_adc_signal = 0;
static uint16_t g_adc_adj = 0;

static uint16_t g_threshold = THRESHOLD_DEFAULT_ADC;
static uint32_t g_threshold_filter = ((uint32_t)THRESHOLD_DEFAULT_ADC << ADJ_FILTER_SHIFT);
static uint8_t  g_threshold_init = 0;

static uint8_t  g_light_ok = 0;
static uint8_t  g_output_state = 0;

static uint8_t  g_sync_pulse_count = 0;
static uint8_t  g_in_pulse = 0;

static uint8_t  g_valid_count = 0;
static uint8_t  g_miss_count = 0;

static uint16_t g_search_adc_scan_count = 0;

#if LOCKED_ENABLE_ADJ_UPDATE
static uint16_t g_locked_adj_window_count = 0;
#endif

#if DEBUG_NC_SAMPLE_WINDOW
static volatile uint8_t g_nc_window_level = 0;
#endif

/*======================================================================
 * 八、函数声明
 *====================================================================*/

void GPIO_Init(void);
void ADC_Init(void);

static void timer0_init_base(void);
static void timer0_schedule_us(uint16_t delay_us);
static void timer0_disable_window_timer(void);
static void timer0_update_callback(void);

static uint16_t adc_read_once(uint8_t ch);

static void rx_enter_search(void);
static void rx_enter_locked(void);

static void rx_search_process(void);
static void rx_locked_process(void);
static void rx_window_sample_process(void);
static void rx_window_result_process(void);

static uint8_t signal_enter_active(uint16_t adc_value);
static uint8_t signal_leave_active(uint16_t adc_value);
static uint8_t signal_is_valid(uint16_t adc_value);

static void threshold_update_process(void);
static uint16_t threshold_map_from_adj(uint16_t adj);

static void output_apply(uint8_t light_ok);

#if DEBUG_NC_SAMPLE_WINDOW
static void debug_nc_window_high(void);
static void debug_nc_window_low(void);
#endif

/*======================================================================
 * 九、主函数
 *====================================================================*/

void main(void)
{
    system_init();

    GPIO_Init();

    /*
     * 当前版本为了保证窗口时序，默认不启用串口。
     * 如果需要串口调试，可以打开，但要确认 uart_init() 不开启串口中断。
     */
    /* uart_init(); */

    ADC_Init();

    threshold_update_process();

    timer0_init_base();

    output_apply(0);

    rx_enter_search();

    EA = 1;

    while(1)
    {
        if(g_rx_state == RX_STATE_SEARCH)
        {
            rx_search_process();
        }
        else
        {
            rx_locked_process();
        }
    }
}

/*======================================================================
 * 十、GPIO 初始化
 *====================================================================*/

void GPIO_Init(void)
{
    /*
     * P1.0 -> NO
     */
    REG_P10_CFG = 0x00;
    gpio_init(IR_OUT_NO_PIN);
    gpio_dir_set(IR_OUT_NO_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NO_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);

    /*
     * P1.2 -> NC
     */
    REG_P12_CFG = 0x00;
    gpio_init(IR_OUT_NC_PIN);
    gpio_dir_set(IR_OUT_NC_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NC_PIN, GPIO_SR_HIGH);

#if DEBUG_NC_SAMPLE_WINDOW
    gpio_io_set(IR_OUT_NC_PIN, GPIO_LOW);
#else
    gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
#endif

    /*
     * P1.3 -> LED
     */
    REG_P13_CFG = 0x00;
    gpio_init(IR_LED_PIN);
    gpio_dir_set(IR_LED_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_LED_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);

    /*
     * P1.5 -> ADC 接收信号
     * P2.0 -> ADC ADJ
     */
    REG_P15_CFG = 0x00;
    REG_P20_CFG = 0x00;
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

    adc_sample_clk_config(ADC_SAMPCLK_4);

    adc_io_config(IR_SIGNAL_ADC_CHANNEL | IR_ADJ_ADC_CHANNEL);

    adc_scan_mode_config(ADC_MODE_SINGLE);

    adc_power_config(ADC_ENABLE);

    adc_controller_config(ADC_ENABLE);

    /*
     * ADC 不使用中断。
     */
    ADCIER &= (uint8_t)(~0x01);
    IEN0 &= (uint8_t)(~0x40);
}

/*======================================================================
 * 十二、GTIMER0 配置
 *====================================================================*/

static void timer0_init_base(void)
{
    /*
     * 初始化一个默认计数值。
     */
    gtimer0_stop();

    REG_GTIM0_IER &= (uint8_t)(~GTIMER0_UIE);
    REG_GTIM0_SR = 0x07;

    gtimer0_count_init(1000U, TIMER0_US_PRESCALER);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    /*
     * 关键修复：
     * 使用仓库原 gtimer0.c 的回调机制。
     * 不在 main.c 里重写 interrupt 21。
     */
    gtimer0_irq_init(GTIMER_IRQ_ENABLE,
                     GTIMER0_UIE_IRQ,
                     timer0_update_callback);

    /*
     * gtimer0_irq_init 会打开 UIE。
     * 初始化阶段先关闭，等同步成功后再打开。
     */
    REG_GTIM0_IER &= (uint8_t)(~GTIMER0_UIE);

    gtimer0_stop();
}

static void timer0_schedule_us(uint16_t delay_us)
{
    if(delay_us < 2U)
    {
        delay_us = 2U;
    }

    gtimer0_stop();

    REG_GTIM0_IER &= (uint8_t)(~GTIMER0_UIE);

    gtimer0_count_init(delay_us, TIMER0_US_PRESCALER);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    REG_GTIM0_SR = 0x07;

    REG_GTIM0_IER |= GTIMER0_UIE;

    gtimer0_start();
}

static void timer0_disable_window_timer(void)
{
    REG_GTIM0_IER &= (uint8_t)(~GTIMER0_UIE);
    REG_GTIM0_SR = 0x07;
    gtimer0_stop();

    g_timer_event = TIMER_EVENT_NONE;
}

/*======================================================================
 * 十三、ADC 单次轮询读取
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
 * 十四、进入搜索 / 进入锁定
 *====================================================================*/

static void rx_enter_search(void)
{
    EA = 0;

    timer0_disable_window_timer();

    g_rx_state = RX_STATE_SEARCH;

    g_window_open = 0;
    g_window_result_ready = 0;
    g_resync_request = 0;
    g_window_overrun = 0;

    g_window_sample_count = 0;
    g_last_window_sample_count = 0;

#if RX_SIGNAL_ACTIVE_HIGH
    g_window_peak = 0;
    g_last_window_peak = 0;
#else
    g_window_peak = ADC_MAX_VALUE;
    g_last_window_peak = ADC_MAX_VALUE;
#endif

    g_window_count_since_sync = 0;

    g_sync_pulse_count = 0;
    g_in_pulse = 0;

    g_valid_count = 0;
    g_miss_count = 0;
    
    /*
    * 失同步 / 重新搜索时，强制认为无光，立即关闭输出。
    * 否则红灯灭了，但输出会保持上一次状态。
    */
    g_light_ok = 0;
    output_apply(0);

    g_search_adc_scan_count = 0;

#if LOCKED_ENABLE_ADJ_UPDATE
    g_locked_adj_window_count = 0;
#endif

#if DEBUG_NC_SAMPLE_WINDOW
    debug_nc_window_low();
#endif

    gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);

    EA = 1;
}

static void rx_enter_locked(void)
{
    EA = 0;

    g_rx_state = RX_STATE_LOCKED;

    g_window_open = 0;
    g_window_result_ready = 0;
    g_resync_request = 0;
    g_window_overrun = 0;

    g_window_sample_count = 0;
    g_last_window_sample_count = 0;

#if RX_SIGNAL_ACTIVE_HIGH
    g_window_peak = 0;
    g_last_window_peak = 0;
#else
    g_window_peak = ADC_MAX_VALUE;
    g_last_window_peak = ADC_MAX_VALUE;
#endif

    g_window_count_since_sync = 0;

    g_valid_count = 0;
    g_miss_count = 0;

#if LOCKED_ENABLE_ADJ_UPDATE
    g_locked_adj_window_count = 0;
#endif

#if DEBUG_NC_SAMPLE_WINDOW
    debug_nc_window_low();
#endif

    /*
     * LED 亮表示同步成功，方便判断是否进入 LOCKED。
     */
    gpio_io_set(IR_LED_PIN, LED_ACTIVE_LEVEL);

    g_timer_event = TIMER_EVENT_OPEN_WINDOW;

    /*
     * 同步成功后，安排第一次开窗。
     */
    timer0_schedule_us(SAMPLE_FIRST_DELAY_US);

    EA = 1;
}

/*======================================================================
 * 十五、搜索阶段
 *====================================================================*/

static void rx_search_process(void)
{
    uint16_t signal;

    signal = adc_read_once(IR_SIGNAL_ADC_CHANNEL);
    g_adc_signal = signal;

    /*
     * 等待进入有效脉冲。
     */
    if(g_in_pulse == 0)
    {
        if(signal_enter_active(signal))
        {
            g_in_pulse = 1;

            if(g_sync_pulse_count < 255U)
            {
                g_sync_pulse_count++;
            }

            if(g_sync_pulse_count >= SYNC_REQUIRED_PULSES)
            {
                rx_enter_locked();
                return;
            }
        }
    }
    else
    {
        /*
         * 必须先离开有效区，才允许计下一次脉冲。
         */
        if(signal_leave_active(signal))
        {
            g_in_pulse = 0;
        }
    }

    /*
     * 搜索阶段偶尔更新 ADJ。
     */
    g_search_adc_scan_count++;
    if(g_search_adc_scan_count >= SEARCH_ADJ_UPDATE_DIV)
    {
        g_search_adc_scan_count = 0;
        threshold_update_process();
    }
}

/*======================================================================
 * 十六、锁定阶段主循环
 *====================================================================*/

static void rx_locked_process(void)
{
    /*
     * 定时器要求重新同步。
     */
    if(g_resync_request)
    {
        rx_enter_search();
        return;
    }

    /*
     * 窗口结果优先处理。
     */
    if(g_window_result_ready)
    {
        rx_window_result_process();
        return;
    }

    /*
     * 窗口打开期间，主循环连续扫 ADC。
     */
    if(g_window_open)
    {
        rx_window_sample_process();
        return;
    }

#if LOCKED_ENABLE_ADJ_UPDATE
    if(g_locked_adj_window_count >= LOCKED_ADJ_UPDATE_WINDOWS)
    {
        g_locked_adj_window_count = 0;
        threshold_update_process();
    }
#endif
}

/*======================================================================
 * 十七、窗口内 ADC 扫描
 *====================================================================*/

static void rx_window_sample_process(void)
{
    uint16_t signal;

    if(g_window_sample_count >= SAMPLE_COUNT_PER_WINDOW)
    {
        return;
    }

    signal = adc_read_once(IR_SIGNAL_ADC_CHANNEL);

    /*
     * 如果 ADC 读取过程中窗口被关闭，
     * 该次结果不计入本窗口。
     */
    if(g_window_open == 0)
    {
        return;
    }

    g_adc_signal = signal;

#if RX_SIGNAL_ACTIVE_HIGH
    if(signal > g_window_peak)
    {
        g_window_peak = signal;
    }
#else
    if(signal < g_window_peak)
    {
        g_window_peak = signal;
    }
#endif

    if(g_window_sample_count < 255U)
    {
        g_window_sample_count++;
    }
}

/*======================================================================
 * 十八、窗口结果处理
 *====================================================================*/

static void rx_window_result_process(void)
{
    uint16_t peak;
    uint8_t count;

    EA = 0;

    peak = g_last_window_peak;
    count = g_last_window_sample_count;

    g_window_result_ready = 0;

    EA = 1;

    g_adc_signal = peak;

    if((count > 0U) && signal_is_valid(peak))
    {
        g_miss_count = 0;

        if(g_valid_count < 255U)
        {
            g_valid_count++;
        }

        if(g_valid_count >= VALID_CONFIRM_COUNT)
        {
            if(g_light_ok == 0)
            {
                g_light_ok = 1;
                output_apply(1);
            }
        }
    }
    else
    {
        g_valid_count = 0;

        if(g_miss_count < 255U)
        {
            g_miss_count++;
        }

        if(g_miss_count >= MISS_CONFIRM_COUNT)
        {
            if(g_light_ok != 0)
            {
                g_light_ok = 0;
                output_apply(0);
            }
        }

        if(g_miss_count >= MISS_RESYNC_COUNT)
        {
            rx_enter_search();
            return;
        }
    }
}

/*======================================================================
 * 十九、阈值判断
 *====================================================================*/

static uint8_t signal_enter_active(uint16_t adc_value)
{
#if RX_SIGNAL_ACTIVE_HIGH
    if(adc_value >= (uint16_t)(g_threshold + RX_ON_MARGIN_ADC))
    {
        return 1;
    }
#else
    if(adc_value <= (uint16_t)(g_threshold - RX_ON_MARGIN_ADC))
    {
        return 1;
    }
#endif

    return 0;
}

static uint8_t signal_leave_active(uint16_t adc_value)
{
#if RX_SIGNAL_ACTIVE_HIGH
    if(adc_value <= (uint16_t)(g_threshold - RX_OFF_MARGIN_ADC))
    {
        return 1;
    }
#else
    if(adc_value >= (uint16_t)(g_threshold + RX_OFF_MARGIN_ADC))
    {
        return 1;
    }
#endif

    return 0;
}

static uint8_t signal_is_valid(uint16_t adc_value)
{
#if RX_SIGNAL_ACTIVE_HIGH
    if(adc_value >= (uint16_t)(g_threshold + RX_ON_MARGIN_ADC))
    {
        return 1;
    }
#else
    if(adc_value <= (uint16_t)(g_threshold - RX_ON_MARGIN_ADC))
    {
        return 1;
    }
#endif

    return 0;
}

/*======================================================================
 * 二十、ADJ 阈值更新
 *====================================================================*/

static void threshold_update_process(void)
{
    uint16_t adj;
    uint16_t new_threshold;

    adj = adc_read_once(IR_ADJ_ADC_CHANNEL);
    g_adc_adj = adj;

    new_threshold = threshold_map_from_adj(adj);

    if(g_threshold_init == 0)
    {
        g_threshold_init = 1;
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
    uint32_t value;

    if(adj > ADC_MAX_VALUE)
    {
        adj = ADC_MAX_VALUE;
    }

    span = (uint32_t)(THRESHOLD_MAX_ADC - THRESHOLD_MIN_ADC);

    value = (uint32_t)THRESHOLD_MIN_ADC +
            (((uint32_t)adj * span) / ADC_MAX_VALUE);

    if(value < THRESHOLD_MIN_ADC)
    {
        value = THRESHOLD_MIN_ADC;
    }
    else if(value > THRESHOLD_MAX_ADC)
    {
        value = THRESHOLD_MAX_ADC;
    }

    return (uint16_t)value;
}

/*======================================================================
 * 二十一、输出控制
 *====================================================================*/

static void output_apply(uint8_t light_ok)
{
    uint8_t action;

#if SENSOR_DARK_ON
    action = light_ok ? 0 : 1;
#else
    action = light_ok ? 1 : 0;
#endif

    g_output_state = action;

    if(action)
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_ACTIVE_LEVEL);

#if !DEBUG_NC_SAMPLE_WINDOW
        gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
#endif

        gpio_io_set(IR_LED_PIN, LED_ACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);

#if !DEBUG_NC_SAMPLE_WINDOW
        gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
#endif

        gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);
    }
}

/*======================================================================
 * 二十二、NC 窗口观察
 *====================================================================*/

#if DEBUG_NC_SAMPLE_WINDOW
static void debug_nc_window_high(void)
{
    g_nc_window_level = 1;
    gpio_io_set(IR_OUT_NC_PIN, GPIO_HIGH);
}

static void debug_nc_window_low(void)
{
    g_nc_window_level = 0;
    gpio_io_set(IR_OUT_NC_PIN, GPIO_LOW);
}
#endif

/*======================================================================
 * 二十三、GTIMER0 回调函数
 *
 * 注意：
 * 这里不是 interrupt 函数。
 * interrupt 21 已经在 Driver/src/gtimer0.c 的 GTIM0_IRQHandler 里。
 * 这里通过 gtimer0_irq_init() 注册给 gtimer0_callback[0]。
 *====================================================================*/

static void timer0_update_callback(void)
{
    if(g_rx_state != RX_STATE_LOCKED)
    {
        timer0_disable_window_timer();
        return;
    }

    if(g_timer_event == TIMER_EVENT_OPEN_WINDOW)
    {
        /*
         * 如果上一个窗口结果还没处理完，
         * 说明主循环处理超时，直接重新同步。
         */
        if(g_window_result_ready)
        {
            g_window_overrun = 1;
            g_resync_request = 1;
            timer0_disable_window_timer();

#if DEBUG_NC_SAMPLE_WINDOW
            debug_nc_window_low();
#endif
            return;
        }

        g_window_open = 1;
        g_window_sample_count = 0;

#if RX_SIGNAL_ACTIVE_HIGH
        g_window_peak = 0;
#else
        g_window_peak = ADC_MAX_VALUE;
#endif

#if DEBUG_NC_SAMPLE_WINDOW
        debug_nc_window_high();
#endif

        /*
         * 下一次回调：关闭窗口。
         */
        g_timer_event = TIMER_EVENT_CLOSE_WINDOW;
        timer0_schedule_us(SAMPLE_WINDOW_US);

        return;
    }

    if(g_timer_event == TIMER_EVENT_CLOSE_WINDOW)
    {
        /*
         * 关闭窗口。
         */
        g_window_open = 0;

#if DEBUG_NC_SAMPLE_WINDOW
        debug_nc_window_low();
#endif

        /*
         * 锁存窗口结果。
         */
        g_last_window_peak = g_window_peak;
        g_last_window_sample_count = g_window_sample_count;
        g_window_result_ready = 1;

        g_window_count_since_sync++;

#if LOCKED_ENABLE_ADJ_UPDATE
        g_locked_adj_window_count++;
#endif

        if(g_window_count_since_sync >= RESYNC_EVERY_WINDOWS)
        {
            g_resync_request = 1;
            timer0_disable_window_timer();
            return;
        }

        /*
         * 下一次回调：打开下一个窗口。
         */
        g_timer_event = TIMER_EVENT_OPEN_WINDOW;
        timer0_schedule_us(SAMPLE_REST_US);

        return;
    }

    /*
     * 异常状态。
     */
    g_resync_request = 1;
    timer0_disable_window_timer();
}
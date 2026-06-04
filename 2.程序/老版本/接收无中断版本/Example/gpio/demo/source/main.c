/***********************************************************************
 * Industrial Infrared Through-beam Receiver
 * Platform : UM800Y / 24MHz
 * Mode     : No interrupt, polling only
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"

/* ======================= 用户可调参数 ======================= */

/*
 * 硬件映射：
 * ADC_CHANNEL_1：接收光强
 * ADC_CHANNEL_2：电位器阈值 ADJ，原理图说明 ADJ/P2.0 用于调接收距离
 */
#define IR_RX_ADC_CHANNEL           ADC_CHANNEL_1
#define IR_ADJ_ADC_CHANNEL          ADC_CHANNEL_2

/*
 * P1.4：原旧代码用作接收脉冲外部中断输入。
 * 现在改为普通 GPIO 轮询上升沿。
 */
#define IR_CP_PIN                   P1_4

/*
 * 输出：
 * P1.2：NO 常开
 * P1.3：NC 常闭
 * P1.0：指示灯，若实物 LED 极性相反，改 LED_ACTIVE_LEVEL 即可。
 */
#define IR_OUT_NO_PIN               P1_2
#define IR_OUT_NC_PIN               P1_3
#define IR_LED_PIN                  P1_0

#define OUT_ACTIVE_LEVEL            GPIO_HIGH
#define OUT_INACTIVE_LEVEL          GPIO_LOW

#define LED_ACTIVE_LEVEL            GPIO_HIGH
#define LED_INACTIVE_LEVEL          GPIO_LOW

/*
 * 0：亮通输出，收到光时 NO 动作。
 * 1：暗通输出，遮光/丢光时 NO 动作。
 *
 * 原旧版逻辑是 adc_data >= adc_set 后输出动作，所以默认用亮通输出。
 */
#define SENSOR_DARK_ON              0

/*
 * 24MHz / 16 = 1.5MHz
 * 1500 个计数约 1ms。
 */
#define TIMER0_1MS_RELOAD           1500U
#define TIMER0_PRESCALER            (16U - 1U)

#ifndef GTIMER0_UIF
#define GTIMER0_UIF                 0x01
#endif

/*
 * 阈值映射范围：
 * 电位器 ADC 0~4095 映射到 700~2800。
 * 如果现场太灵敏，增大 THRESHOLD_MIN_ADC。
 * 如果最远距离不够，减小 THRESHOLD_MIN_ADC 或增大红外发射电流。
 */
#define THRESHOLD_MIN_ADC           700U
#define THRESHOLD_MAX_ADC           2800U
#define THRESHOLD_DEFAULT_ADC       1400U

/*
 * 滞回窗口：
 * ADC 高于 threshold + ON_MARGIN 判定为有效光。
 * ADC 低于 threshold - OFF_MARGIN 判定为丢光。
 */
#define RX_ON_MARGIN_ADC            80U
#define RX_OFF_MARGIN_ADC           80U

/*
 * 工业去抖：
 * 连续 2 次有效脉冲才认为收到光。
 * 连续 3 次无效脉冲才认为丢光。
 * 超过 8ms 没有收到 P1.4 脉冲，强制丢光。
 */
#define RX_ON_CONFIRM_COUNT         2U
#define RX_OFF_CONFIRM_COUNT        3U
#define RX_LOST_TIMEOUT_MS          8U

/*
 * 电位器阈值每 20ms 更新一次，并做 IIR 平滑。
 */
#define ADJ_UPDATE_PERIOD_MS        20U
#define ADJ_FILTER_SHIFT            3U

/*
 * ADC 合法范围。
 */
#define ADC_VALID_MAX               4090U

/* ======================= 全局状态 ======================= */

static volatile uint16_t g_adc_signal = 0;
static volatile uint16_t g_adc_adj = 0;
static volatile uint16_t g_threshold = THRESHOLD_DEFAULT_ADC;

static uint32_t g_threshold_filter = ((uint32_t)THRESHOLD_DEFAULT_ADC << ADJ_FILTER_SHIFT);

static uint8_t g_light_ok = 0;
static uint8_t g_on_count = 0;
static uint8_t g_off_count = 0;

static uint8_t g_last_cp_level = 0;
static uint8_t g_cp_init = 0;

static uint8_t g_adj_update_count = 0;
static uint16_t g_lost_ms = RX_LOST_TIMEOUT_MS;

/* ======================= 函数声明 ======================= */

void GPIO_Init(void);
void ADC_Init(void);

static void timer0_init_poll_1ms(void);
static void timer0_poll_process(void);

static uint16_t adc_read_once(uint8_t ch);
static uint16_t adc_read_average(uint8_t ch, uint8_t times);

static void threshold_update_process(void);
static void cp_poll_process(void);
static void rx_sample_process(uint16_t adc_value);
static void output_apply(uint8_t light_ok);
static void sensor_force_lost(void);

/* ======================= 主函数 ======================= */

void main(void)
{
    system_init();

    GPIO_Init();
    ADC_Init();
    timer0_init_poll_1ms();

    /*
     * 上电先读取一次阈值，避免默认阈值和电位器位置差太多。
     */
    threshold_update_process();

    output_apply(0);

    while(1)
    {
        timer0_poll_process();
        cp_poll_process();
    }
}

/* ======================= 初始化 ======================= */

void GPIO_Init(void)
{
    /*
     * 指示灯 P1.0
     */
    REG_P10_CFG = 0x00;
    gpio_init(IR_LED_PIN);
    gpio_dir_set(IR_LED_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_LED_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);

    /*
     * NO 输出 P1.2
     */
    REG_P12_CFG = 0x00;
    gpio_init(IR_OUT_NO_PIN);
    gpio_dir_set(IR_OUT_NO_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NO_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);

    /*
     * NC 输出 P1.3
     */
    REG_P13_CFG = 0x00;
    gpio_init(IR_OUT_NC_PIN);
    gpio_dir_set(IR_OUT_NC_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NC_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);

    /*
     * P1.5 保留为输入，兼容旧板预留脚。
     */
    REG_P15_CFG = 0x00;
    gpio_init(P1_5);
    gpio_dir_set(P1_5, GPIO_DIR_IN);
    gpio_in_enable(P1_5, IN_ENABLE);
}

void ADC_Init(void)
{
    adc_clk_config(ADC_CLKSOURCE_SYSCLK, ADC_VREFSOURCE_AVDD33, 4, ADC_ENABLE);
    adc_sample_clk_config(ADC_SAMPCLK_4);

    adc_io_config(IR_RX_ADC_CHANNEL | IR_ADJ_ADC_CHANNEL);

    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);
}

static void timer0_init_poll_1ms(void)
{
    gtimer0_count_init(TIMER0_1MS_RELOAD, TIMER0_PRESCALER);

    /*
     * 不调用 gtimer0_irq_init()。
     * 只启动计数器，然后主循环轮询 UIF。
     */
    REG_GTIM0_SR = GTIMER0_UIF;
    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    gtimer0_start();
}

/* ======================= 基础驱动 ======================= */

static uint16_t adc_read_once(uint8_t ch)
{
    uint16_t value;

    adc_convert_start(ch);

    while((ADCGCR1 & 0x04) != 0);
    while(!(ADCCSTAT & 0x01));

    ADCCSTAT = 0x01;
    value = adc_get_value();

    return value;
}

static uint16_t adc_read_average(uint8_t ch, uint8_t times)
{
    uint8_t i;
    uint16_t value;
    uint16_t min_value = 0xFFFF;
    uint16_t max_value = 0;
    uint32_t sum = 0;

    if(times == 0)
    {
        return adc_read_once(ch);
    }

    for(i = 0; i < times; i++)
    {
        value = adc_read_once(ch);
        sum += value;

        if(value < min_value)
        {
            min_value = value;
        }

        if(value > max_value)
        {
            max_value = value;
        }
    }

    /*
     * 4 点采样，去掉最大最小，提高电位器阈值抗抖。
     */
    if(times >= 4)
    {
        sum -= min_value;
        sum -= max_value;
        return (uint16_t)(sum / (times - 2));
    }

    return (uint16_t)(sum / times);
}

/* ======================= 周期轮询 ======================= */

static void timer0_poll_process(void)
{
    if(REG_GTIM0_SR & GTIMER0_UIF)
    {
        REG_GTIM0_SR = GTIMER0_UIF;

        /*
         * 丢脉冲超时。
         * P1.4 长时间没有有效边沿，强制丢光。
         */
        if(g_lost_ms < 60000U)
        {
            g_lost_ms++;
        }

        if(g_lost_ms >= RX_LOST_TIMEOUT_MS)
        {
            sensor_force_lost();
        }

        /*
         * 慢速更新电位器阈值。
         */
        g_adj_update_count++;
        if(g_adj_update_count >= ADJ_UPDATE_PERIOD_MS)
        {
            g_adj_update_count = 0;
            threshold_update_process();
        }
    }
}

static void cp_poll_process(void)
{
    uint8_t now_level;

    now_level = gpio_io_get(IR_CP_PIN);

    if(g_cp_init == 0)
    {
        g_cp_init = 1;
        g_last_cp_level = now_level;
        return;
    }

    /*
     * 默认使用上升沿。
     * 如果实测 P1.4 是下降沿有效，把判断改成：
     * if((g_last_cp_level != 0) && (now_level == 0))
     */
    if((g_last_cp_level == 0) && (now_level != 0))
    {
        g_lost_ms = 0;

        g_adc_signal = adc_read_once(IR_RX_ADC_CHANNEL);

        if(g_adc_signal < ADC_VALID_MAX)
        {
            rx_sample_process(g_adc_signal);
        }
        else
        {
            sensor_force_lost();
        }
    }

    g_last_cp_level = now_level;
}

/* ======================= 阈值与判定 ======================= */

static void threshold_update_process(void)
{
    uint16_t adj;
    uint16_t new_threshold;
    uint32_t span;

    adj = adc_read_average(IR_ADJ_ADC_CHANNEL, 4);
    g_adc_adj = adj;

    if(adj > 4095U)
    {
        adj = 4095U;
    }

    span = (uint32_t)(THRESHOLD_MAX_ADC - THRESHOLD_MIN_ADC);
    new_threshold = (uint16_t)(THRESHOLD_MIN_ADC + ((uint32_t)adj * span) / 4095U);

    /*
     * IIR 平滑：
     * threshold_filter = 7/8 old + 1/8 new
     */
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

static void rx_sample_process(uint16_t adc_value)
{
    uint16_t on_threshold;
    uint16_t off_threshold;

    on_threshold = g_threshold + RX_ON_MARGIN_ADC;

    if(g_threshold > RX_OFF_MARGIN_ADC)
    {
        off_threshold = g_threshold - RX_OFF_MARGIN_ADC;
    }
    else
    {
        off_threshold = 0;
    }

    if(adc_value >= on_threshold)
    {
        if(g_on_count < 255U)
        {
            g_on_count++;
        }

        g_off_count = 0;
    }
    else if(adc_value <= off_threshold)
    {
        if(g_off_count < 255U)
        {
            g_off_count++;
        }

        g_on_count = 0;
    }
    else
    {
        /*
         * 滞回区内不改变状态。
         * 这样可以防止阈值附近频繁抖动。
         */
    }

    if((g_light_ok == 0) && (g_on_count >= RX_ON_CONFIRM_COUNT))
    {
        g_light_ok = 1;
    }

    if((g_light_ok != 0) && (g_off_count >= RX_OFF_CONFIRM_COUNT))
    {
        g_light_ok = 0;
    }

    output_apply(g_light_ok);
}

static void sensor_force_lost(void)
{
    g_light_ok = 0;
    g_on_count = 0;

    if(g_off_count < RX_OFF_CONFIRM_COUNT)
    {
        g_off_count = RX_OFF_CONFIRM_COUNT;
    }

    output_apply(0);
}

/* ======================= 输出 ======================= */

static void output_apply(uint8_t light_ok)
{
    uint8_t output_active;

#if SENSOR_DARK_ON
    output_active = light_ok ? 0 : 1;
#else
    output_active = light_ok ? 1 : 0;
#endif

    /*
     * NO / NC 互补输出。
     * 若实物三极管驱动极性相反，只需要交换 OUT_ACTIVE_LEVEL / OUT_INACTIVE_LEVEL。
     */
    if(output_active)
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_ACTIVE_LEVEL);
        gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);
        gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
    }

    if(light_ok)
    {
        gpio_io_set(IR_LED_PIN, LED_ACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);
    }
}
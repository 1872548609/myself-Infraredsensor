/***********************************************************************
 * 文件名称：main.c（全文注释版）
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
 * 2. 找到多个连续间隔约 1ms 的合格脉冲后，进入同步跟踪。
 * 3. 同步后每 1ms 在预计脉冲附近打开一个约 200us 的采样窗口。
 * 4. 窗口内高速读取 ADC，取峰值。
 * 5. 用 峰值 - 基线 得到真实脉冲幅度，避免环境光/偏置影响。
 * 6. 连续多个有效脉冲确认有光，并对搜索阶段脉冲宽度/周期做校验，减少误触发。
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

/* 搜索阶段脉冲宽度校验：25us 脉冲允许一定误差，过窄多半是毛刺，过宽多半是环境干扰/饱和。 */
#define IR_SEARCH_PULSE_MIN_US      6U
#define IR_SEARCH_PULSE_MAX_US      90U

/* 跟踪阶段：预计脉冲中心前 80us 到后 120us 开窗，总窗口 200us。 */
#define IR_WINDOW_PRE_US            80U
#define IR_WINDOW_POST_US           120U
#define IR_WINDOW_WIDTH_US          (IR_WINDOW_PRE_US + IR_WINDOW_POST_US)

/* 响应速度：约 4ms 确认有光，约 10ms 确认遮光，连续漏采先重搜不立刻闪断。 */
#define IR_LIGHT_ON_CONFIRM_COUNT   4U
#define IR_LIGHT_OFF_MISS_COUNT     10U     /* 真遮光确认：约 10ms，约 100Hz，抗漏采更强 */
#define IR_RELOCK_MISS_COUNT        3U      /* 连续 3 个窗口未命中，先回搜索，但不立刻关输出 */
#define IR_LOST_SYNC_MISS_COUNT     20U     /* 仅用于调试统计，不再直接决定输出闪断 */
#define IR_SEARCH_LOCK_COUNT        4U      /* 搜索阶段必须连续 4 个 1ms 周期正确，才允许进入跟踪 */

/* 单周期最大相位修正，防止被噪声拖跑。 */
#define IR_PHASE_ADJUST_LIMIT_US    12

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
#define IR_TH_MIN_ADC               120U
#define IR_TH_MAX_ADC               1800U
#define IR_TH_DEFAULT_ADC           700U
#define IR_TH_HYS_MIN_ADC           80U

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
/*
 * 调试变量说明：
 * g_ir_light_ok          当前内部判定是否有光，1=有光，0=无光。
 * g_ir_synced            当前是否已经锁定 1ms 脉冲同步。
 * g_ir_output_state      最终输出是否动作，受亮通/暗通影响。
 * g_ir_adc_signal        最近一次读取的接收 ADC 原始值。
 * g_ir_adc_adj           最近一次读取的 ADJ 电位器 ADC 原始值。
 * g_ir_threshold_on      有光确认高阈值。
 * g_ir_threshold_off     有光保持低阈值，也就是回差后的阈值。
 * g_ir_baseline          当前接收通道背景基线。
 * g_ir_peak              最近一个窗口内的峰值。
 * g_ir_amp               最近一次窗口的瞬时幅度，即 peak-baseline。
 * g_ir_amp_fast          快速滤波幅度，便于观察输出响应趋势。
 * g_ir_amp_display       慢速滤波幅度，后续可用于显示距离/光强。
 * g_ir_next_center_us    下一次预计脉冲中心时间。
 * g_ir_last_light_seen_us最后一次确认看到有效红外的时间。
 */
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
static volatile uint16_t g_ir_last_light_seen_us = 0;

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

/*
 * GPIO_Init()：
 * 配置接收端用到的所有引脚。
 * P1.0：NO 常开输出。
 * P1.2：NC 常闭输出。
 * P1.3：红色指示灯。
 * P2.7：示波器调试窗口脚。
 * P1.5/P2.0：先恢复默认配置，之后由 ADC 初始化切到模拟输入。
 */
void GPIO_Init(void);
/*
 * ADC_Init()：
 * 配置 ADC 时钟、参考电压、采样时间和通道。
 * 采样时间设置较短，是为了在 200us 窗口内尽量多采样，
 * 从而更容易抓到 25us 的窄脉冲峰值。
 */
void ADC_Init(void);

/*
 * timer0_init_1us_free_run()：
 * 把 Timer0 配置为 1us 自由运行计数器。
 * 本程序不依赖 Timer0 中断，只反复读取计数值作为时间戳。
 * uint16_t 计数会在 65.536ms 回绕，程序中的 time_sub_u16() 会处理回绕。
 */
static void timer0_init_1us_free_run(void);
/*
 * time_us16()：
 * 安全读取 Timer0 的 16 位计数值。
 * 由于高/低字节分开读取，所以采用“高-低-高”方式，
 * 两次高字节一致才认为读取稳定，避免低字节溢出时读错。
 */
static uint16_t time_us16(void);
/*
 * time_after_eq_u16()：
 * 判断 now 是否已经到达或超过 target。
 * 用 int16_t 差值比较，可以兼容 uint16_t 时间回绕。
 */
static uint8_t time_after_eq_u16(uint16_t now, uint16_t target);
/*
 * time_sub_u16()：
 * 计算两个 16 位时间戳的差值。
 * 利用无符号整数自然回绕，只要时间差小于 32768us~65535us 量级，
 * 对本程序 1ms/10ms 的判断是安全的。
 */
static uint16_t time_sub_u16(uint16_t a, uint16_t b);

/*
 * adc_read_once()：
 * 对指定 ADC 通道做一次阻塞式采样。
 * 阻塞等待转换完成，返回 0~4095 的 12 位 ADC 值。
 * 由于脉冲很窄，本函数要尽量短，不建议在里面加入滤波或打印。
 */
static uint16_t adc_read_once(uint8_t ch);
/*
 * ir_detector_init()：
 * 初始化红外检测状态机和所有运行变量。
 * 上电时不知道发射脉冲相位，所以默认进入 SEARCH。
 * 初始 baseline 用当前 ADC 值，后续在非脉冲区慢速跟随。
 */
static void ir_detector_init(void);
/*
 * ir_detector_poll()：
 * 接收算法的主轮询函数。
 * 每次调用都先根据 ADJ 电位器更新阈值，
 * 然后根据当前状态执行“搜索”或“跟踪”。
 * 最后用 last_light_seen 超时判断真正遮光。
 */
static void ir_detector_poll(void);
/*
 * ir_set_search()：
 * 重新回到搜索状态。
 * 注意：这里不直接关闭输出。
 * 因为连续漏采可能只是窗口漂移，不一定是真遮光，
 * 真正关闭输出由 ir_detector_poll() 中的超时逻辑决定。
 */
static void ir_set_search(void);
/*
 * ir_search_sample()：
 * 搜索同步阶段。
 * 逻辑是连续高速读 ADC，寻找超过阈值的疑似脉冲；
 * 脉冲结束后检查宽度是否像 25us 脉冲；
 * 再检查连续脉冲间隔是否约 1ms；
 * 连续多个周期都正确后，才进入 TRACKING。
 */
static void ir_search_sample(void);
/*
 * ir_tracking_process()：
 * 同步跟踪阶段。
 * 根据 g_ir_next_center_us 计算窗口起点和终点；
 * 窗口外只更新 baseline；窗口内拉高 P2.7 并高速采样；
 * 窗口结束后把窗口峰值交给 ir_process_frame()。
 */
static void ir_tracking_process(void);
/*
 * ir_process_frame()：
 * 对一个 1ms 周期的窗口结果做判断。
 * 输入 peak 是窗口最强值，peak_us 是峰值出现时间，expected_center_us 是预计中心。
 * 函数完成：幅度计算、有效/无效判断、连续计数、输出更新、相位微调。
 */
static void ir_process_frame(uint16_t peak, uint16_t peak_us, uint16_t expected_center_us);

/*
 * ir_abs_amp()：
 * 根据接收信号极性，计算 sample 相对 baseline 的有效幅度。
 * 正脉冲：sample - baseline。
 * 负脉冲：baseline - sample。
 */
static uint16_t ir_abs_amp(uint16_t sample, uint16_t baseline);
/*
 * ir_sample_stronger()：
 * 判断当前 sample 是否比窗口内已有 best 更“强”。
 * 正脉冲找最大 ADC，负脉冲找最小 ADC。
 */
static uint8_t ir_sample_stronger(uint16_t sample, uint16_t old_best);
/*
 * ir_best_init_value()：
 * 返回窗口峰值搜索的初始值。
 * 正脉冲从 0 开始找最大值；负脉冲从 0xFFFF 开始找最小值。
 */
static uint16_t ir_best_init_value(void);
/*
 * ir_iir_u16()：
 * 无符号 16 位一阶 IIR 滤波。
 * shift=1 表示每次变化 1/2，shift=3 表示每次变化 1/8。
 * 用移位实现，避免 8051 上使用浮点。
 */
static uint16_t ir_iir_u16(uint16_t old_v, uint16_t new_v, uint8_t shift);
/*
 * ir_limit_i16()：
 * int16_t 限幅函数。
 * 这里主要用于限制每个周期的相位修正量，防止噪声把窗口拉偏。
 */
static int16_t ir_limit_i16(int16_t x, int16_t min_v, int16_t max_v);
/*
 * ir_update_baseline()：
 * 更新背景基线。
 * 只有当前幅度低于 threshold_off，说明大概率不是红外脉冲，
 * 才允许 baseline 慢速跟随 sample，避免把脉冲峰值吃进基线。
 */
static void ir_update_baseline(uint16_t sample);
/*
 * ir_update_threshold_from_adj()：
 * 周期性读取 ADJ 电位器，并映射成检测阈值。
 * 更新周期是 10ms，不会每次循环都读 ADJ，避免干扰接收采样。
 * 阈值本身也做 IIR 平滑，防止电位器抖动导致输出抖动。
 */
static void ir_update_threshold_from_adj(void);
/*
 * threshold_map_from_adj()：
 * 把电位器 ADC 0~4095 线性映射到 IR_TH_MIN_ADC~IR_TH_MAX_ADC。
 * 这个阈值代表“红外脉冲幅度”阈值，不是 ADC 原始电压阈值。
 */
static uint16_t threshold_map_from_adj(uint16_t adj);

/*
 * output_apply()：
 * 根据 light_ok 和 SENSOR_DARK_ON 计算最终输出状态。
 * 亮通：有光时输出动作。
 * 暗通：无光时输出动作。
 * NO 和 NC 始终互补，LED 按 light_ok 指示。
 */
static void output_apply(uint8_t light_ok);
/*
 * debug_window_pin()：
 * 控制 P2.7 调试窗口脚。
 * 窗口采样期间拉高，窗口结束拉低。
 * 用示波器观察它和接收模拟脉冲的相对位置，可以判断同步是否稳定。
 */
static void debug_window_pin(uint8_t level);

/*======================================================================
 * 九、主函数
 *====================================================================*/

/*
 * main() 主入口：
 * 1. 初始化系统、GPIO、串口、ADC、1us 定时器。
 * 2. 初始化红外检测状态机。
 * 3. 关闭总中断，保证主循环连续扫描 ADC，不被中断打断。
 * 4. while(1) 中只运行 ir_detector_poll()，不要插入长延时。
 */
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

/* 以下是 Timer0 1us 时间基准的具体初始化实现。 */
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

/* 以下实现安全读取 16 位时间戳。 */
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

/* 以下实现两个时间戳相减，允许 uint16_t 回绕。 */
static uint16_t time_sub_u16(uint16_t a, uint16_t b)
{
    return (uint16_t)(a - b);
}

/* 以下实现“当前时间是否到达目标时间”的判断。 */
static uint8_t time_after_eq_u16(uint16_t now, uint16_t target)
{
    return (((int16_t)(now - target)) >= 0) ? 1U : 0U;
}

/*======================================================================
 * 十三、ADC 单次读取
 *====================================================================*/

/* 以下实现指定 ADC 通道的单次阻塞采样。 */
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

/* 以下实现检测状态机的上电初始化。 */
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
    g_ir_last_light_seen_us = time_us16();

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

/* 以下实现回到搜索状态，但不立即改变输出。 */
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

/* 以下是每次主循环调用的红外检测总入口。 */
static void ir_detector_poll(void)
{
    uint16_t now;

    ir_update_threshold_from_adj();

    if(g_ir_state == IR_STATE_SEARCH)
    {
        ir_search_sample();
    }
    else
    {
        ir_tracking_process();
    }

    /*
     * 关键防闪烁逻辑：
     * 窗口没采到，不等于真的遮光，可能只是同步漂移。
     * 只有超过 IR_LIGHT_OFF_MISS_COUNT 个 1ms 周期都没有重新确认到光，
     * 才允许输出无光。
     */
    if(g_ir_light_ok != 0U)
    {
        now = time_us16();
        if(time_sub_u16(now, g_ir_last_light_seen_us) >=
           (uint16_t)(IR_LIGHT_OFF_MISS_COUNT * IR_PERIOD_US))
        {
            g_ir_light_ok = 0U;
            output_apply(0U);
        }
    }
}

/*======================================================================
 * 十六、搜索同步阶段
 *====================================================================*/

/* 以下实现搜索阶段的脉冲宽度校验和 1ms 周期校验。 */
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

    /* 当前不在脉冲中：等待 ADC 幅度越过高阈值，作为脉冲起点。 */
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

    /* 当前已经在脉冲中：等幅度跌回低阈值，认为脉冲结束。 */
    if(amp < g_ir_threshold_off)
    {
        g_ir_in_pulse = 0U;
        width_us = time_sub_u16(now, g_ir_search_start_us);

        /*
         * 真实脉冲约 25us。
         * 宽度太窄：多数是 ADC/运放尖峰毛刺。
         * 宽度太宽：多数是环境光、饱和、慢变化干扰。
         * 这一步能明显减少“偶尔误触发几个信号”。
         */
        /* 宽度校验：不符合 25us 脉冲特征的信号直接丢弃。 */
        if((width_us < IR_SEARCH_PULSE_MIN_US) ||
           (width_us > IR_SEARCH_PULSE_MAX_US))
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

        /* 周期校验：真实发射信号应接近 1000us。 */
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
            g_ir_last_light_seen_us = g_ir_search_peak_us;
        }
    }
}

/*======================================================================
 * 十七、同步跟踪阶段：每 1ms 打窗口采样
 *====================================================================*/

/* 以下实现同步后的定时窗口采样。 */
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

    /* 还没到窗口，读到的 ADC 大概率是背景值，可用于更新 baseline。 */
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

    /* 窗口内持续采样，最终只保留最强的一个点 best。 */
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

/* 以下实现单个窗口结果的有效性判断、输出确认和相位修正。 */
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

    /*
     * 注意：每一帧是否命中脉冲，必须用本窗口的瞬时 amp 判断。
     * 不能用 g_ir_amp_fast 判断，否则一次漏采后，滤波残留仍可能被当成有效脉冲，
     * 进而用错误的 peak_us 修正相位，最终造成输出周期性闪烁。
     */
    if(g_ir_light_ok != 0U)
    {
        valid = (amp > g_ir_threshold_off) ? 1U : 0U;
    }
    else
    {
        valid = (amp > g_ir_threshold_on) ? 1U : 0U;
    }

    /* 本帧有效：增加有效计数，清除漏采计数，并根据峰值时间微调相位。 */
    if(valid != 0U)
    {
        if(g_ir_good_count < 255U)
        {
            g_ir_good_count++;
        }
        g_ir_miss_count = 0U;
        g_ir_last_light_seen_us = peak_us;

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
        /* 本帧无效：清有效计数，增加漏采计数。 */
        g_ir_good_count = 0U;
        if(g_ir_miss_count < 255U)
        {
            g_ir_miss_count++;
        }

        g_ir_next_center_us = (uint16_t)(expected_center_us + IR_PERIOD_US);

        /*
         * 窗口连续漏采，优先认为是同步漂移，回到搜索重新锁相。
         * 这里不直接关输出；真正关输出由 ir_detector_poll() 的
         * IR_LIGHT_OFF_MISS_COUNT * 1ms 超时统一处理。
         */
        if(g_ir_miss_count >= IR_RELOCK_MISS_COUNT)
        {
            ir_set_search();
        }
    }
}

/*======================================================================
 * 十八、阈值、基线、滤波工具
 *====================================================================*/

/* 以下根据信号极性计算幅度。 */
static uint16_t ir_abs_amp(uint16_t sample, uint16_t baseline)
{
#if RX_SIGNAL_ACTIVE_HIGH
    return (sample > baseline) ? (uint16_t)(sample - baseline) : 0U;
#else
    return (baseline > sample) ? (uint16_t)(baseline - sample) : 0U;
#endif
}

/* 以下根据信号极性判断哪个采样点更强。 */
static uint8_t ir_sample_stronger(uint16_t sample, uint16_t old_best)
{
#if RX_SIGNAL_ACTIVE_HIGH
    return (sample > old_best) ? 1U : 0U;
#else
    return (sample < old_best) ? 1U : 0U;
#endif
}

/* 以下给窗口峰值搜索提供初始值。 */
static uint16_t ir_best_init_value(void)
{
#if RX_SIGNAL_ACTIVE_HIGH
    return 0U;
#else
    return 0xFFFFU;
#endif
}

/* 以下实现整数 IIR 滤波。 */
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

/* 以下实现 int16_t 限幅。 */
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

/* 以下实现背景基线慢速跟随。 */
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

/* 以下实现 ADJ 电位器到检测阈值的更新。 */
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

/* 以下实现电位器 ADC 到阈值范围的线性映射。 */
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

/* 以下实现 NO/NC/LED 的最终输出刷新。 */
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

/* 以下实现 P2.7 窗口调试脚控制。 */
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

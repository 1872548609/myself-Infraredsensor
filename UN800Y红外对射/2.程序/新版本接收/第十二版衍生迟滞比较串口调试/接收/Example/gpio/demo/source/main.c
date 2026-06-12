/***********************************************************************
 * UM800Y 红外对射接收端 main.c
 *
 * 当前版本功能：
 * 1. P1_4 捕获比较器输出脉冲。
 * 2. 发射周期 1ms，占空比 0.025，有效脉宽约 25us。
 * 3. GTIMER0 使用 24 分频：24MHz / 24 = 1MHz，1 tick = 1us。
 * 4. 600us 内重复边沿判定为毛刺，用于过滤比较器抖动、双边沿和振铃。
 * 5. ADC 增加迟滞判断：
 *      ADC >= adc_set                      ：ADC 状态切换为有效；
 *      ADC <= adc_set - RX_ADC_HYS_VALUE   ：ADC 状态切换为无效；
 *      中间区域保持上一次 ADC 状态。
 * 6. 连续 3 个有效 1ms 脉冲确认有光：
 *      从第 1 个有效脉冲开始约 2ms 确认；
 *      按外部遮挡释放时刻最坏相位计算，约 3ms 内确认。
 * 7. 1.5ms 无有效脉冲判定遮光，理论遮光响应约 667Hz。
 * 8. 当前参数属于“快速响应版”，满足不低于 200Hz 的响应要求，
 *    但对漏脉冲、ADC 偶发不过阈值、比较器边沿抖动更敏感。
 * 9. 串口调试输出用于观察 ADC、阈值、边沿周期、有效计数、毛刺次数和超时次数。
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
 * 一、接收算法参数
 *====================================================================*/

/*
 * 系统主频 24MHz，GTIMER0 使用 24 分频：
 *
 * 定时器频率 = 24MHz / 24 = 1MHz
 * 1 tick = 1us
 *
 * 发射周期 = 1ms
 * 发射占空比 = 0.025
 * 有效脉宽 ≈ 25us
 *
 * 当前判断策略：
 *
 * 1. P1_4 每 1ms 应收到一次比较器脉冲。
 * 2. 小于 600us 的重复边沿，不可能是下一次真实发射脉冲，
 *    直接认为是毛刺、振铃或双边沿。
 * 3. 连续 3 个合格脉冲确认有光。
 * 4. 1.5ms 内没有新的合格脉冲，判定遮光。
 *
 * 注意：
 * RX_LOST_TIMEOUT_TICK = 1500U 比较激进。
 * 它只允许 1ms 周期有约 500us 余量。
 * 如果现场 TO 频繁增加、输出仍然抖动，
 * 可以把它改成 2500U 或 3000U，仍然满足 200Hz 响应要求。
 */

#define RX_CONFIRM_COUNT          3U

#define RX_MIN_PERIOD_TICK        600U     /* 600us 内重复边沿认为是毛刺 */

/*
 * RX_MAX_PERIOD_TICK 必须小于 RX_LOST_TIMEOUT_TICK。
 *
 * 当前丢光超时是 1500us。
 * 如果这里写 1600us，那么还没进入“周期过长”判断，
 * 定时器就已经先超时了。
 */
#define RX_MAX_PERIOD_TICK        1300U    /* 大于 1.3ms 认为周期偏长 */

/*
 * 1.5ms 丢光超时。
 *
 * 优点：
 *      遮光响应非常快，约 667Hz。
 *
 * 缺点：
 *      只要漏掉一个 1ms 脉冲，或者某次 ADC 没过阈值，
 *      就可能触发丢光，抗抖余量较小。
 */
#define RX_LOST_TIMEOUT_TICK      2500U


/*======================================================================
 * 二、ADC 迟滞参数
 *====================================================================*/

/*
 * adc_set 是 ADC 有效上阈值。
 *
 * 当前默认：
 *      adc_set = 200
 *      RX_ADC_HYS_VALUE = 120
 *
 * 因此：
 *      ADC >= 200 ：ADC 状态切换为有效；
 *      ADC <= 80  ：ADC 状态切换为无效；
 *      80 ~ 200   ：保持上一次 ADC 状态。
 *
 * 这样可以明显减少 ADC 在阈值附近跳动导致的输出抖动。
 *
 * 注意：
 * RX_ADC_HYS_VALUE 越大，抗抖越强，但 ADC 状态释放越慢。
 * 如果遮光后 ADC 残余值仍然高于 80，并且 P1_4 仍然有毛刺，
 * 可能会导致输出不容易释放。
 *
 * 调试判断：
 * 1. AF 持续增加：说明 ADC 经常不过阈值，可能 TH 太高或采样时机偏晚。
 * 2. L 不释放但 S 在 80~200：说明迟滞下阈值太低，可以减小 HYS。
 * 3. S 在 TH 附近抖：可以适当增大 HYS。
 */

#define RX_ADC_HYS_ENABLE         1U
#define RX_ADC_HYS_VALUE          120U

#ifndef ADC_INVALID
#define ADC_INVALID               4095U
#endif

#ifndef ADC_MAX_VALUE
#define ADC_MAX_VALUE             4095U
#endif

#define RX_ADC_MAX_VALID          (ADC_INVALID - 2U)


/*======================================================================
 * 三、串口调试参数
 *====================================================================*/

/*
 * 1 = 打开串口调试
 * 0 = 关闭串口调试
 *
 * 注意：
 * printfS 不放在中断里执行。
 * 中断里只设置打印请求，主循环里打印。
 */

#define UART_ADC_DEBUG_ENABLE              1U

/*
 * 每多少个 P1_4 中断事件打印一次。
 *
 * 发射周期 1ms 时：
 * 100 大约是 100ms 打印一次。
 */
#define UART_ADC_DEBUG_EVENT_DIV           100U

/*
 * 有光确认 / 丢光超时时是否立即打印一次。
 */
#define UART_ADC_DEBUG_STATE_CHANGE_PRINT  1U


#define UART_DBG_REASON_BOOT               1U
#define UART_DBG_REASON_SHORT_EDGE         2U
#define UART_DBG_REASON_ADC_FAIL           3U
#define UART_DBG_REASON_VALID_PULSE        4U
#define UART_DBG_REASON_LIGHT_ON           5U
#define UART_DBG_REASON_TIMEOUT            6U


/**** 全局变量，保留原命名 ****/

volatile uint16_t adc_data  = 0;
volatile uint16_t adc_data1 = 0;

/*
 * 当前 ADC 有效阈值。
 * 现在先固定 200。
 *
 * 后续如果要用 ADJ 电位器控制阈值，可以在主循环里读取 ADC_CHANNEL_2，
 * 然后映射更新 adc_set。
 */
volatile uint16_t adc_set = 200;

uint16_t status   = 0;
uint16_t adcount  = 0;
uint16_t adcount1 = 0;


/*
 * 接收状态
 */
static volatile uint8_t rx_valid_count = 0;     /* 连续有效光脉冲计数 */
static volatile uint8_t rx_light_state = 0;     /* 0 = 遮光/无光，1 = 有光 */
static volatile uint8_t rx_seen_once   = 0;     /* 是否已经捕获过有效脉冲 */


/*
 * ADC 迟滞状态：
 * 0 = ADC 当前认为无效
 * 1 = ADC 当前认为有效
 */
static volatile uint8_t rx_adc_valid_state = 0;


/*======================================================================
 * 四、串口调试变量
 *====================================================================*/

/*
 * 这两个变量即使关闭串口调试也保留。
 *
 * 原因：
 *      GPIO_IRQHandler 和 gtimer0_UECallBack 中会直接更新它们。
 *      如果放在 #if UART_ADC_DEBUG_ENABLE 内部，
 *      当 UART_ADC_DEBUG_ENABLE = 0 时可能编译报错。
 */
static volatile uint16_t g_dbg_last_elapsed_tick = 0;
static volatile uint8_t  g_dbg_last_adc_ok       = 0;

#if UART_ADC_DEBUG_ENABLE

static volatile uint8_t  g_uart_debug_need_print = 0;
static volatile uint8_t  g_uart_debug_div_count  = 0;
static volatile uint8_t  g_uart_debug_reason     = 0;

/* 事件统计，满 65535 后保持 */
static volatile uint16_t g_dbg_irq_count         = 0;
static volatile uint16_t g_dbg_short_edge_count  = 0;
static volatile uint16_t g_dbg_adc_fail_count    = 0;
static volatile uint16_t g_dbg_valid_pulse_count = 0;
static volatile uint16_t g_dbg_timeout_count     = 0;

#endif


/*======================================================================
 * 五、函数声明
 *====================================================================*/

void gtimer0_UECallBack(void);
void gtimer1_UECallBack(void);
void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);

static uint16_t rx_timer_get_count(void);
static void rx_timer_stop_clear(void);
static void rx_timer_restart(void);
static void rx_output_light(void);
static void rx_output_block(void);
static uint16_t rx_adc_read_once(uint8_t ch);
static uint16_t rx_adc_calc_off_threshold(void);
static uint8_t rx_adc_hysteresis_check(uint16_t adc);

#if UART_ADC_DEBUG_ENABLE
static void uart_debug_inc16(volatile uint16_t *value);
static void uart_debug_request(uint8_t reason, uint8_t force);
static void uart_adc_debug_process(void);
#else
#define uart_debug_request(reason, force)     ((void)0)
#define uart_adc_debug_process()              ((void)0)
#endif


/*======================================================================
 * 六、定时器工具函数
 *====================================================================*/

static uint16_t rx_timer_get_count(void)
{
    uint16_t cnt;

    cnt  = REG_GTIM0_CNT0;
    cnt |= ((uint16_t)REG_GTIM0_CNT1 << 8);

    return cnt;
}


static void rx_timer_stop_clear(void)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;
}


static void rx_timer_restart(void)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    REG_GTIM0_CR0 |= (1 << 0);
}


/*======================================================================
 * 七、输出控制
 *====================================================================*/

static void rx_output_light(void)
{
    /*
     * 保持原来的有光输出逻辑：
     * P1_2 = HIGH
     * P1_3 = HIGH
     * P1_0 = LOW
     */
    gpio_io_set(P1_2, GPIO_HIGH);
    gpio_io_set(P1_3, GPIO_HIGH);
//    gpio_io_set(P1_0, GPIO_LOW);
}


static void rx_output_block(void)
{
    /*
     * 保持原来的遮光 / 无光输出逻辑：
     * P1_2 = LOW
     * P1_3 = LOW
     * P1_0 = HIGH
     */
    gpio_io_set(P1_2, GPIO_LOW);
    gpio_io_set(P1_3, GPIO_LOW);
//    gpio_io_set(P1_0, GPIO_HIGH);
}


/*======================================================================
 * 八、ADC 读取与迟滞判断
 *====================================================================*/

static uint16_t rx_adc_read_once(uint8_t ch)
{
    uint16_t value;

    adc_convert_start(ch);

    while ((ADCGCR1 & 0x04) != 0);
    while (!(ADCCSTAT & 0x01));

    ADCCSTAT = 0x01;

    /*
     * 这里沿用你原工程里的 adc_get_value()。
     */
    value = adc_get_value();

    if (value > ADC_MAX_VALUE)
    {
        value = ADC_MAX_VALUE;
    }

    return value;
}


static uint16_t rx_adc_calc_off_threshold(void)
{
    if (adc_set > RX_ADC_HYS_VALUE)
    {
        return (uint16_t)(adc_set - RX_ADC_HYS_VALUE);
    }

    return 0U;
}


static uint8_t rx_adc_hysteresis_check(uint16_t adc)
{
    uint16_t adc_on_th;
    uint16_t adc_off_th;

    /*
     * 异常值保护。
     */
    if (adc >= RX_ADC_MAX_VALID)
    {
        rx_adc_valid_state = 0;
        return 0;
    }

    adc_on_th  = adc_set;
    adc_off_th = rx_adc_calc_off_threshold();

    if (rx_adc_valid_state == 0)
    {
        /*
         * 当前 ADC 状态为无效。
         * 必须超过上阈值，才切换为有效。
         */
        if (adc >= adc_on_th)
        {
            rx_adc_valid_state = 1;
        }
    }
    else
    {
        /*
         * 当前 ADC 状态为有效。
         * 必须低于下阈值，才切换为无效。
         */
        if (adc <= adc_off_th)
        {
            rx_adc_valid_state = 0;
        }
    }

    return rx_adc_valid_state;
}


/*======================================================================
 * 九、串口调试函数
 *====================================================================*/

#if UART_ADC_DEBUG_ENABLE

static void uart_debug_inc16(volatile uint16_t *value)
{
    if (*value < 65535U)
    {
        (*value)++;
    }
}


static void uart_debug_request(uint8_t reason, uint8_t force)
{
    g_uart_debug_reason = reason;

    if (force != 0U)
    {
        g_uart_debug_div_count  = 0U;
        g_uart_debug_need_print = 1U;
        return;
    }

    g_uart_debug_div_count++;

    if (g_uart_debug_div_count >= UART_ADC_DEBUG_EVENT_DIV)
    {
        g_uart_debug_div_count  = 0U;
        g_uart_debug_need_print = 1U;
    }
}


static void uart_adc_debug_process(void)
{
    uint16_t adc_off_th;

    if (g_uart_debug_need_print == 0U)
    {
        return;
    }

    g_uart_debug_need_print = 0U;

    adc_off_th = rx_adc_calc_off_threshold();

    /*
     * 字段说明：
     *
     * S   ：最近一次接收信号 ADC。
     * A   ：预留 ADJ ADC，目前未主动读取。
     * TH  ：ADC 有效上阈值 adc_set。
     * HYS ：ADC 迟滞宽度。
     * OFF ：ADC 无效下阈值。
     * EL  ：最近一次边沿间隔，24 分频下单位就是 us。
     * VC  ：连续有效脉冲计数。
     * L   ：当前有光确认状态，1 有光，0 遮光。
     * AS  ：ADC 迟滞状态，1 ADC 有效，0 ADC 无效。
     * OK  ：最近一次 ADC 判断结果。
     * IR  ：P1_4 中断总次数。
     * SH  ：小于 600us 被过滤的短边沿次数。
     * AF  ：ADC 不满足阈值次数。
     * VP  ：有效脉冲次数。
     * TO  ：丢光超时次数。
     * R   ：打印原因码。
     *
     * R：
     * 1 = 上电
     * 2 = 短边沿毛刺
     * 3 = ADC 不满足
     * 4 = 有效脉冲
     * 5 = 有光确认
     * 6 = 丢光超时
     *
     * 调试判断：
     * 1. 正常有光：
     *      EL ≈ 1000，VC = 3，L = 1，AS = 1，OK = 1。
     *
     * 2. SH 持续增加：
     *      P1_4 比较器输出有毛刺、双边沿或振铃。
     *
     * 3. AF 持续增加：
     *      ADC 经常不过阈值，可能 TH 太高、光强不够或采样时机偏晚。
     *
     * 4. TO 有光时持续增加：
     *      1.5ms 内没有稳定收到有效脉冲。
     *      优先考虑把 RX_LOST_TIMEOUT_TICK 改成 2500U 或 3000U。
     */
    printfS("S=%u,A=%u,TH=%u,HYS=%u,OFF=%u,EL=%u,VC=%u,L=%u,AS=%u,OK=%u,IR=%u,SH=%u,AF=%u,VP=%u,TO=%u,R=%u\r\n",
            adc_data,
            adc_data1,
            adc_set,
            (uint16_t)RX_ADC_HYS_VALUE,
            adc_off_th,
            g_dbg_last_elapsed_tick,
            (uint16_t)rx_valid_count,
            (uint16_t)rx_light_state,
            (uint16_t)rx_adc_valid_state,
            (uint16_t)g_dbg_last_adc_ok,
            g_dbg_irq_count,
            g_dbg_short_edge_count,
            g_dbg_adc_fail_count,
            g_dbg_valid_pulse_count,
            g_dbg_timeout_count,
            (uint16_t)g_uart_debug_reason);
}

#endif


/*======================================================================
 * 十、主函数
 *====================================================================*/

void main(void)
{
    system_init();

    GPIO_Init();

    uart_init();

    /*
     * GTIMER0 作为丢光超时定时器。
     *
     * 24MHz / 24 = 1MHz
     * 1 tick = 1us
     *
     * RX_LOST_TIMEOUT_TICK = 1500
     * 即 1.5ms 丢光超时。
     *
     * 响应速度：
     *      1 / 1.5ms ≈ 667Hz
     *
     * 注意：
     *      该参数响应很快，但抗漏脉冲能力较弱。
     *      如果现场 TO 频繁增加、输出仍然抖动，
     *      优先把 RX_LOST_TIMEOUT_TICK 调到 2500U 或 3000U。
     */
    gtimer0_count_init(RX_LOST_TIMEOUT_TICK, 24 - 1);
    gtimer0_irq_init(GTIMER_IRQ_ENABLE, GTIMER0_UIE_IRQ, gtimer0_UECallBack);

    /*
     * 上电先不启动定时器。
     * 等第一次有效 P1_4 脉冲来了再启动。
     */
    rx_timer_stop_clear();

    /*
     * ADC 初始化。
     *
     * ADC_CHANNEL_1：接收信号 ADC。
     * ADC_CHANNEL_2：预留 ADJ 电位器。
     */
    adc_clk_config(ADC_CLKSOURCE_SYSCLK, ADC_VREFSOURCE_AVDD33, 4, ADC_ENABLE);
    adc_sample_clk_config(ADC_SAMPCLK_4);
    adc_io_config(ADC_CHANNEL_1 | ADC_CHANNEL_2);
    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);

    /*
     * 上电默认遮光 / 无光输出。
     */
    rx_output_block();

    /*
     * 上电打印一次，确认串口调试打开。
     */
    uart_debug_request(UART_DBG_REASON_BOOT, 1U);

    while (1)
    {
        /*
         * 串口调试只在主循环打印，不在中断里打印。
         */
        uart_adc_debug_process();
    }
}


/*======================================================================
 * 十一、丢光超时回调
 *====================================================================*/

void gtimer0_UECallBack(void)
{
    rx_valid_count = 0;
    rx_seen_once   = 0;

    /*
     * 丢光后 ADC 迟滞状态清零。
     *
     * 作用：
     *      再次来光时，ADC 必须重新超过 adc_set，
     *      避免上一次 ADC 有效状态残留导致误确认有光。
     */
    rx_adc_valid_state = 0;
    g_dbg_last_adc_ok  = 0;

#if UART_ADC_DEBUG_ENABLE
    uart_debug_inc16(&g_dbg_timeout_count);
#endif

    if (rx_light_state)
    {
        rx_light_state = 0;
        rx_output_block();

#if UART_ADC_DEBUG_STATE_CHANGE_PRINT
        uart_debug_request(UART_DBG_REASON_TIMEOUT, 1U);
#else
        uart_debug_request(UART_DBG_REASON_TIMEOUT, 0U);
#endif
    }
    else
    {
        rx_output_block();
        uart_debug_request(UART_DBG_REASON_TIMEOUT, 0U);
    }

    /*
     * 无光后停止定时器。
     * 下一次有效 P1_4 脉冲来了再开启。
     */
    rx_timer_stop_clear();
}


/*======================================================================
 * 十二、P1_4 比较器中断
 *====================================================================*/

void GPIO_IRQHandler(void) interrupt 0
{
    uint16_t elapsed;
    uint8_t adc_ok;
    
    if (gpio_irq_get(P1_4))
    {
        /*
         * 必须先清中断标志。
         *
         * 原因：
         *      后面 ADC 可能判断失败并 return。
         *      如果这里不先清标志，ADC 失败时 P1_4 中断标志会一直挂着，
         *      CPU 可能反复进入同一个中断，导致 AF 快速增加、统计异常、输出抖动。
         *
         * 清标志只占很短时间，相比 ADC 转换时间可以忽略。
         */
        gpio_irq_clr(P1_4);

#if UART_ADC_DEBUG_ENABLE
        uart_debug_inc16(&g_dbg_irq_count);
#endif

#if RX_ADC_HYS_ENABLE
        /*
         * ADC 采样尽量提前。
         *
         * 当前发射有效脉宽约 25us，ADC 读取越晚越容易错过脉冲。
         * 所以在清掉中断标志后，第一时间启动 ADC 转换。
         */
        adc_data = rx_adc_read_once(ADC_CHANNEL_1);
        
        gpio_io_set(P1_0, GPIO_LOW);

        adc_ok = rx_adc_hysteresis_check(adc_data);
        g_dbg_last_adc_ok = adc_ok;

        if (!adc_ok)
        {
#if UART_ADC_DEBUG_ENABLE
            uart_debug_inc16(&g_dbg_adc_fail_count);
#endif
            uart_debug_request(UART_DBG_REASON_ADC_FAIL, 0U);
            return;
        }
#else
        adc_ok = 1U;
        g_dbg_last_adc_ok = 1U;
#endif

        /*
         * ADC 已经通过，再读取距离上一次有效脉冲的时间。
         *
         * 注意：
         *      因为 ADC 现在提前读取，EL 会包含一点 ADC 转换耗时。
         *      也就是说 EL 会比真实边沿间隔略大一点。
         *      这不影响判断，只要正常有光时 EL 仍然接近 1000us 即可。
         */
        elapsed = rx_timer_get_count();
        g_dbg_last_elapsed_tick = elapsed;

        /*
         * 发射周期是 1ms。
         *
         * 如果两次有效事件间隔小于 600us：
         *      不可能是下一次真实发射脉冲；
         *      认为是比较器毛刺、双边沿或振铃。
         *
         * 这里虽然 ADC 已经读过一次，但仍然不把它算作有效光脉冲。
         */
        if (rx_seen_once && (elapsed < RX_MIN_PERIOD_TICK))
        {
#if UART_ADC_DEBUG_ENABLE
            uart_debug_inc16(&g_dbg_short_edge_count);
#endif
            uart_debug_request(UART_DBG_REASON_SHORT_EDGE, 0U);
            return;
        }

        /*
         * 到这里说明：
         * 1. P1_4 有比较器中断；
         * 2. ADC 幅值通过迟滞判断；
         * 3. 不是 600us 内重复毛刺。
         *
         * 这才算一次真正有效的光脉冲。
         */
        if (rx_seen_once && (elapsed > RX_MAX_PERIOD_TICK))
        {
            /*
             * 间隔超过 RX_MAX_PERIOD_TICK，说明周期偏长。
             *
             * 未确认有光时：
             *      重新同步，从 1 个有效脉冲开始计数。
             *
             * 已确认有光时：
             *      允许偶发漏一个周期，保持有光确认状态。
             */
            if (rx_light_state)
            {
                rx_valid_count = RX_CONFIRM_COUNT;
            }
            else
            {
                rx_valid_count = 1U;
            }
        }
        else
        {
            if (rx_valid_count < RX_CONFIRM_COUNT)
            {
                rx_valid_count++;
            }
        }

        rx_seen_once = 1U;

        /*
         * 只有真正有效光脉冲才刷新丢光定时器。
         */
        rx_timer_restart();

#if UART_ADC_DEBUG_ENABLE
        uart_debug_inc16(&g_dbg_valid_pulse_count);
#endif

        /*
         * 连续 RX_CONFIRM_COUNT 个有效 1ms 周期确认有光。
         *
         * 当前 RX_CONFIRM_COUNT = 3：
         *      从第 1 个有效脉冲开始，约 2ms 后确认；
         *      按最坏相位计算，约 3ms 内确认。
         */
        if ((!rx_light_state) && (rx_valid_count >= RX_CONFIRM_COUNT))
        {
            rx_light_state = 1U;
            rx_output_light();

#if UART_ADC_DEBUG_STATE_CHANGE_PRINT
            uart_debug_request(UART_DBG_REASON_LIGHT_ON, 1U);
#else
            uart_debug_request(UART_DBG_REASON_LIGHT_ON, 0U);
#endif
        }
        else
        {
            uart_debug_request(UART_DBG_REASON_VALID_PULSE, 0U);
        }
    }
    
    gpio_io_set(P1_0, GPIO_HIGH);
}


void gpio_UECallBack(void)
{
    /*
     * 不用二次回调。
     */
}


/*======================================================================
 * 十三、GPIO 初始化
 *====================================================================*/

void GPIO_Init(void)
{
    /*
     * P1_0 输出
     */
    REG_P10_CFG = 0x00;

    gpio_init(P1_0);
    gpio_dir_set(P1_0, GPIO_DIR_OUT);
    gpio_dr_set(P1_0, GPIO_SR_HIGH);
    gpio_io_set(P1_0, GPIO_HIGH);

    /*
     * P1_2 输出
     */
    REG_P12_CFG = 0x00;

    gpio_init(P1_2);
    gpio_dir_set(P1_2, GPIO_DIR_OUT);
    gpio_dr_set(P1_2, GPIO_SR_HIGH);
    gpio_io_set(P1_2, GPIO_LOW);

    /*
     * P1_3 输出
     */
    REG_P13_CFG = 0x00;

    gpio_init(P1_3);
    gpio_dir_set(P1_3, GPIO_DIR_OUT);
    gpio_dr_set(P1_3, GPIO_SR_HIGH);
    gpio_io_set(P1_3, GPIO_LOW);

    /*
     * P1_4 输入中断。
     *
     * P1_4 用于捕获比较器输出脉冲。
     * ADC_CHANNEL_1 用于读取接收信号 ADC 幅值。
     *
     * 软件判断方式：
     *      比较器脉冲负责“有没有边沿”；
     *      ADC 迟滞负责“光强是否足够”；
     *      GTIMER0 负责“多久没有有效脉冲则判定遮光”。
     */
    gpio_init(P1_4);
    gpio_dir_set(P1_4, GPIO_DIR_IN);
    gpio_dr_set(P1_4, GPIO_SR_HIGH);
    gpio_in_enable(P1_4, IN_ENABLE);
    gpio_irq_set(P1_4, GPIO_IRQ_ENABLE, gpio_UECallBack);

    /*
     * 保留你原来的 P1_4 特殊配置。
     *
     * 如果串口 SH 持续增加，或者示波器看到一个 25us 脉冲触发多次中断，
     * 需要重点确认这里到底是单边沿触发还是双边沿触发。
     */
    P1AH &= ~(0x02);
    P1AH |=  (0x01);

    REG_P14_CFG = 0x20;

    /*
     * P1_5 输入，保留原配置。
     */
    REG_P15_CFG = 0x00;

    gpio_init(P1_5);
    gpio_dir_set(P1_5, GPIO_DIR_IN);
    gpio_in_enable(P1_5, IN_ENABLE);
}
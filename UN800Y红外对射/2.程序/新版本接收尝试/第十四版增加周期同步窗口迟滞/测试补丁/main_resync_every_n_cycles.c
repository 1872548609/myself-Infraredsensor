/***********************************************************************
 * UM800Y 红外对射接收端 main.c
 *
 * 本版本逻辑：
 * 1. P1_4 只作为“首次同步边沿”。捕获到一次比较器脉冲后，认为已经对齐发射周期。
 * 2. 同步后关闭 P1_4 同步中断，启动 GTIMER0，但第一次溢出使用可调起点间隔。
 * 3. 起点间隔由 RX_FIRST_WINDOW_DELAY_TICK 设置，用于补偿同步中断带来的相位延迟。
 * 4. 第一次窗口触发后，后续仍然按 RX_WINDOW_INTERVAL_TICK 周期触发，默认每 1ms 一次。
 * 5. ADC 扫描窗口宽度由 RX_ADC_WINDOW_WIDTH_TICK 设置，窗口内连续读取 ADC，做积分平均。
 * 6. 最终使用“窗口平均 ADC”进入原来的 ADC 迟滞判断。
 * 7. 连续 RX_CONFIRM_COUNT 个有效窗口确认有光；连续 RX_LOST_WINDOW_COUNT 个无效窗口判定遮光。
 * 8. 遮光后停止 GTIMER0，重新打开 P1_4 同步中断，等待下一次来光重新同步。
 *
 * 重要参数：
 * - RX_FIRST_WINDOW_DELAY_TICK：同步中断后的第一次窗口起点间隔。
 * - RX_WINDOW_INTERVAL_TICK：第一次窗口之后的窗口间隔周期，默认 1000us。
 * - RX_ADC_WINDOW_WIDTH_TICK：ADC 连续扫描窗口宽度，默认 40us。
 * - RX_CONFIRM_COUNT：连续有效窗口确认有光次数。
 * - RX_LOST_WINDOW_COUNT：连续无效窗口判定遮光次数。
 *
 * 注意：
 * 本方案不再在 P1_4 中断里读 ADC，避免 ADC 采样时机被外部中断抖动影响。
 * P1_4 只负责第一次同步；真正的 ADC 判断由定时器窗口完成。
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
 * 24MHz / 24 = 1MHz，1 tick = 1us。
 */

#define RX_CONFIRM_COUNT              3U

/*
 * 定时器窗口间隔周期。
 * 默认 1000us，也就是同步后每隔 1ms 打开一次 ADC 扫描窗口。
 * 如果发射端周期不是 1ms，只需要改这里。
 */
#define RX_WINDOW_INTERVAL_TICK       1000U

/*
 * 同步中断后的第一次窗口起点间隔。
 *
 * 这个参数只影响“同步后的第一次 ADC 窗口”。
 * 第一次窗口之后，后续窗口仍然保持 RX_WINDOW_INTERVAL_TICK，也就是 1000us。
 *
 * 调试方法：
 * - 如果示波器看到 ADC 窗口整体偏后，就把这里调小，例如 950、900、850。
 * - 如果示波器看到 ADC 窗口整体偏前，就把这里调大，最大不要超过 RX_WINDOW_INTERVAL_TICK。
 *
 * 例：
 * - 1000U：等同旧版，同步后 1000us 才打开第一次窗口。
 * - 900U ：同步后 900us 打开第一次窗口，相当于后续相位整体提前 100us。
 */
#define RX_FIRST_WINDOW_DELAY_TICK    900U

/*
 * ADC 连续扫描窗口宽度。
 * 默认 40us。
 *
 * 发射有效脉宽约 25us 时：
 * - 窗口太小：容易因为相位偏差漏掉脉冲；
 * - 窗口太大：平均值会被窗口内的低电平拉低，需要适当降低 adc_set；
 * - 建议先用 30~60us 调试。
 */
#define RX_ADC_WINDOW_WIDTH_TICK      100U

/*
 * 连续多少个 ADC 窗口无效后判定遮光。
 * 默认 3 个窗口，即约 3ms 遮光确认，响应约 333Hz。
 * 如果想更快，可以改 2；如果现场抖动明显，可以改 4 或 5。
 */
#define RX_LOST_WINDOW_COUNT          3U

/*
 * 周期性强制重新同步功能。
 *
 * 作用：
 * 同步后虽然 GTIMER0 每 1ms 周期本身稳定，但接收端本地定时器和发射端周期
 * 之间可能有微小频差，累计一段时间后 ADC 窗口会相对发射脉冲慢慢漂移。
 * 所以每处理 RX_RESYNC_WINDOW_COUNT 个窗口后，主动退出当前 1ms 周期采样，
 * 重新打开 P1_4 同步中断，等下一次发射脉冲边沿重新校准相位。
 *
 * 注意：
 * 强制重同步不会把输出拉成遮光，只是暂停周期窗口并等待下一次同步边沿。
 * 如果确实已经遮光，则仍然由 RX_LOST_WINDOW_COUNT 个无效窗口走遮光逻辑。
 *
 * 默认 100 个周期，即约每 100ms 重新同步一次。
 * 如果漂移很快，可以改 50 或 20；如果现场很稳，可以改 200。
 */
#define RX_RESYNC_ENABLE              1U
#define RX_RESYNC_WINDOW_COUNT        100U

/*
 * 是否在同步边沿后立刻补采一个窗口。
 * 0：只从下一个 1ms 周期开始采样，时序更干净。
 * 1：同步后马上采一次，来光确认更快，但可能刚好处于脉冲尾部。
 */
#define RX_FIRST_WINDOW_IMMEDIATE     0U

/*
 * 如果你的 gpio_irq_set() 库没有定义 GPIO_IRQ_DISABLE，通常 0 就是关闭。
 */
#ifndef GPIO_IRQ_DISABLE
#define GPIO_IRQ_DISABLE              0U
#endif


/*======================================================================
 * 二、ADC 迟滞参数
 *====================================================================*/

#define RX_ADC_HYS_ENABLE             1U
#define RX_ADC_HYS_VALUE              120U

#ifndef ADC_INVALID
#define ADC_INVALID                   4095U
#endif

#ifndef ADC_MAX_VALUE
#define ADC_MAX_VALUE                 4095U
#endif

#define RX_ADC_MAX_VALID              (ADC_INVALID - 2U)


/*======================================================================
 * 三、串口调试参数
 *====================================================================*/

#define UART_ADC_DEBUG_ENABLE              0U
#define UART_ADC_DEBUG_EVENT_DIV           100U
#define UART_ADC_DEBUG_STATE_CHANGE_PRINT  1U

#define UART_DBG_REASON_BOOT               1U
#define UART_DBG_REASON_SYNC               2U
#define UART_DBG_REASON_ADC_FAIL           3U
#define UART_DBG_REASON_VALID_WINDOW       4U
#define UART_DBG_REASON_LIGHT_ON           5U
#define UART_DBG_REASON_TIMEOUT            6U
#define UART_DBG_REASON_WINDOW_OVERRUN     7U
#define UART_DBG_REASON_RESYNC             8U


/**** 全局变量，保留原命名 ****/

volatile uint16_t adc_data  = 0;      /* 最近一次 ADC 窗口平均值 */
volatile uint16_t adc_data1 = 0;      /* 最近一次 ADC 窗口采样次数，方便调试 */

/* 当前 ADC 有效阈值。窗口平均后幅值会低于峰值，必要时需要重新标定这个值。 */
volatile uint16_t adc_set = 200;

uint16_t status   = 0;
uint16_t adcount  = 0;
uint16_t adcount1 = 0;


/* 接收状态 */
static volatile uint8_t rx_valid_count       = 0;     /* 连续有效窗口计数 */
static volatile uint8_t rx_lost_window_count = 0;     /* 连续无效窗口计数 */
static volatile uint8_t rx_light_state       = 0;     /* 0 = 遮光/无光，1 = 有光 */
static volatile uint8_t rx_seen_once         = 0;     /* 0 = 等待同步，1 = 已同步并运行窗口采样 */

/* 定时器中断置位，主循环执行 ADC 窗口扫描 */
static volatile uint8_t rx_window_pending    = 0;

/* GPIO_IRQHandler only sets this flag; main loop performs sync start to avoid calling gpio_irq_set() inside ISR. */
static volatile uint8_t rx_sync_pending      = 0;

/* 周期性重同步请求：由主循环处理，避免在中断上下文调用 gpio_irq_set()。 */
static volatile uint8_t rx_resync_pending    = 0;

/* 已同步后累计处理的 ADC 窗口个数，用于按周期数强制重新同步。 */
static volatile uint16_t rx_resync_window_count = 0;

/* ADC 迟滞状态：0 = ADC 当前认为无效，1 = ADC 当前认为有效 */
static volatile uint8_t rx_adc_valid_state   = 0;

/* 1 = 下一次 GTIMER0 溢出是同步后的第一次起点间隔窗口 */
static volatile uint8_t rx_first_interval_pending = 0;

/* 最近一次触发窗口的实际间隔，单位 us；第一次可能不是 1000us */
static volatile uint16_t rx_last_trigger_interval_tick = 0;

/* Cached first-window delay, written in main context and read by GTIMER0 ISR. */
static volatile uint16_t rx_first_delay_cached_tick = RX_FIRST_WINDOW_DELAY_TICK;


/*======================================================================
 * 四、串口调试变量
 *====================================================================*/

static volatile uint16_t g_dbg_last_elapsed_tick = 0;  /* 当前窗口间隔，单位 us */
static volatile uint8_t  g_dbg_last_adc_ok       = 0;

#if UART_ADC_DEBUG_ENABLE

static volatile uint8_t  g_uart_debug_need_print = 0;
static volatile uint8_t  g_uart_debug_div_count  = 0;
static volatile uint8_t  g_uart_debug_reason     = 0;

static volatile uint16_t g_dbg_irq_count         = 0;  /* 同步中断次数 */
static volatile uint16_t g_dbg_short_edge_count  = 0;  /* 这里改成窗口丢失/积压次数 */
static volatile uint16_t g_dbg_adc_fail_count    = 0;  /* ADC 窗口无效次数 */
static volatile uint16_t g_dbg_valid_pulse_count = 0;  /* ADC 窗口有效次数 */
static volatile uint16_t g_dbg_timeout_count     = 0;  /* 遮光次数 */

#endif


/*======================================================================
 * 五、函数声明
 *====================================================================*/

void gtimer0_UECallBack(void);
void gtimer1_UECallBack(void);
void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);
void gpio_UECallBack(void);

static uint16_t rx_timer_get_count(void);
static uint16_t rx_timer_elapsed_from(uint16_t start_tick);
static uint16_t rx_first_delay_get(void);
static uint16_t rx_first_delay_to_timer_preload(uint16_t first_delay_tick);
static void rx_timer_stop_clear(void);
static void rx_timer_restart_with_preload(uint16_t preload_tick);
static void rx_timer_restart_with_first_delay(void);
static void rx_sync_irq_enable(void);
static void rx_sync_irq_disable(void);
static void rx_output_light(void);
static void rx_output_block(void);
static uint16_t rx_adc_read_once(uint8_t ch);
static uint16_t rx_adc_scan_window_average(uint8_t ch, uint16_t window_tick, uint16_t *sample_count);
static uint16_t rx_adc_calc_off_threshold(void);
static uint8_t rx_adc_hysteresis_check(uint16_t adc);
static void rx_start_periodic_window_from_sync(void);
static void rx_enter_block_state(uint8_t debug_reason);
static void rx_force_resync_from_main(void);
static void rx_resync_count_after_window(uint8_t adc_ok);
static void rx_process_adc_window(void);

#if UART_ADC_DEBUG_ENABLE
static void uart_debug_inc16(volatile uint16_t *value);
static void uart_debug_request(uint8_t reason, uint8_t force);
static void uart_adc_debug_process(void);
#else
#define uart_debug_request(reason, force)     ((void)0)
#define uart_adc_debug_process()              ((void)0)
#endif


/*======================================================================
 * 六、定时器与同步中断工具函数
 *====================================================================*/

static uint16_t rx_timer_get_count(void)
{
    uint16_t cnt;

    cnt  = REG_GTIM0_CNT0;
    cnt |= ((uint16_t)REG_GTIM0_CNT1 << 8);

    return cnt;
}


static uint16_t rx_timer_elapsed_from(uint16_t start_tick)
{
    uint16_t now_tick;
    uint16_t elapsed;

    now_tick = rx_timer_get_count();

    if (now_tick >= start_tick)
    {
        elapsed = (uint16_t)(now_tick - start_tick);
    }
    else
    {
        /* GTIMER0 在 RX_WINDOW_INTERVAL_TICK 周期回卷。 */
        elapsed = (uint16_t)((RX_WINDOW_INTERVAL_TICK - start_tick) + now_tick);
    }

    return elapsed;
}


static uint16_t rx_first_delay_get(void)
{
    uint16_t first_delay_tick;

    first_delay_tick = RX_FIRST_WINDOW_DELAY_TICK;

    /*
     * 0us 容易造成定时器重装异常。
     * 如果确实要同步后马上补采，使用 RX_FIRST_WINDOW_IMMEDIATE。
     */
    if (first_delay_tick == 0U)
    {
        first_delay_tick = 1U;
    }

    /*
     * 这里做保护：起点间隔最大不超过后续周期。
     * 本需求主要用于“前移一点”，所以通常应小于 1000us。
     */
    if (first_delay_tick > RX_WINDOW_INTERVAL_TICK)
    {
        first_delay_tick = RX_WINDOW_INTERVAL_TICK;
    }

    return first_delay_tick;
}


static uint16_t rx_first_delay_to_timer_preload(uint16_t first_delay_tick)
{
    /*
     * GTIMER0 的自动重装周期仍然保持 RX_WINDOW_INTERVAL_TICK。
     * 想让第一次溢出提前到 first_delay_tick，
     * 只需要在启动定时器时把 CNT 预装到：
     *
     *     RX_WINDOW_INTERVAL_TICK - first_delay_tick
     *
     * 这样第一次溢出后，硬件自然回到 0，
     * 后续就继续每 RX_WINDOW_INTERVAL_TICK 溢出一次。
     */
    if (first_delay_tick >= RX_WINDOW_INTERVAL_TICK)
    {
        return 0U;
    }

    return (uint16_t)(RX_WINDOW_INTERVAL_TICK - first_delay_tick);
}


static void rx_timer_stop_clear(void)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;
}



static void rx_timer_restart_with_preload(uint16_t preload_tick)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = (uint8_t)(preload_tick & 0x00FFU);
    REG_GTIM0_CNT1 = (uint8_t)((preload_tick >> 8) & 0x00FFU);

    REG_GTIM0_CR0 |= (1 << 0);
}


static void rx_timer_restart_with_first_delay(void)
{
    uint16_t first_delay_tick;
    uint16_t preload_tick;

    first_delay_tick = rx_first_delay_get();
    preload_tick     = rx_first_delay_to_timer_preload(first_delay_tick);

    rx_first_delay_cached_tick = first_delay_tick;
    rx_last_trigger_interval_tick = 0U;

    rx_timer_restart_with_preload(preload_tick);
}


static void rx_sync_irq_enable(void)
{
    gpio_irq_clr(P1_4);
    gpio_irq_set(P1_4, GPIO_IRQ_ENABLE, gpio_UECallBack);
}


static void rx_sync_irq_disable(void)
{
    gpio_irq_clr(P1_4);
    gpio_irq_set(P1_4, GPIO_IRQ_DISABLE, gpio_UECallBack);
}


/*======================================================================
 * 七、输出控制
 *====================================================================*/

static void rx_output_light(void)
{
    gpio_io_set(P1_2, GPIO_HIGH);
    gpio_io_set(P1_3, GPIO_HIGH);
//    gpio_io_set(P1_0, GPIO_LOW);
}


static void rx_output_block(void)
{
    gpio_io_set(P1_2, GPIO_LOW);
    gpio_io_set(P1_3, GPIO_LOW);
//    gpio_io_set(P1_0, GPIO_HIGH);
}


/*======================================================================
 * 八、ADC 读取、窗口积分平均与迟滞判断
 *====================================================================*/

static uint16_t rx_adc_read_once(uint8_t ch)
{
    uint16_t value;

    adc_convert_start(ch);

    while ((ADCGCR1 & 0x04) != 0);
    while (!(ADCCSTAT & 0x01));

    ADCCSTAT = 0x01;

    value = adc_get_value();

    if (value > ADC_MAX_VALUE)
    {
        value = ADC_MAX_VALUE;
    }

    return value;
}


static uint16_t rx_adc_scan_window_average(uint8_t ch, uint16_t window_tick, uint16_t *sample_count)
{
    uint16_t start_tick;
    uint16_t value;
    uint16_t count;
    uint32_t sum;

    sum   = 0U;
    count = 0U;

    start_tick = rx_timer_get_count();

    /* P1_0 拉低用于示波器观察 ADC 扫描窗口。 */
    gpio_io_set(P1_0, GPIO_LOW);

    do
    {
        value = rx_adc_read_once(ch);

        if (value < RX_ADC_MAX_VALID)
        {
            sum += value;
            count++;
        }
    }
    while (rx_timer_elapsed_from(start_tick) < window_tick);

    gpio_io_set(P1_0, GPIO_HIGH);

    if (sample_count != 0)
    {
        *sample_count = count;
    }

    if (count == 0U)
    {
        return ADC_INVALID;
    }

    return (uint16_t)(sum / count);
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

    if (adc >= RX_ADC_MAX_VALID)
    {
        rx_adc_valid_state = 0;
        return 0;
    }

    adc_on_th  = adc_set;
    adc_off_th = rx_adc_calc_off_threshold();

    if (rx_adc_valid_state == 0)
    {
        if (adc >= adc_on_th)
        {
            rx_adc_valid_state = 1;
        }
    }
    else
    {
        if (adc <= adc_off_th)
        {
            rx_adc_valid_state = 0;
        }
    }

    return rx_adc_valid_state;
}


/*======================================================================
 * 九、接收状态机
 *====================================================================*/

static void rx_start_periodic_window_from_sync(void)
{
    rx_seen_once         = 1U;
    rx_window_pending    = 0U;
    rx_sync_pending      = 0U;
    rx_resync_pending    = 0U;
    rx_resync_window_count = 0U;
    rx_valid_count       = 0U;
    rx_lost_window_count = 0U;
    rx_adc_valid_state   = 0U;
    rx_first_interval_pending = 1U;
    rx_last_trigger_interval_tick = 0U;
    g_dbg_last_adc_ok    = 0U;

    /* 同步完成后，P1_4 不再反复进中断，后续由 GTIMER0 周期窗口采样。 */
    rx_sync_irq_disable();

#if RX_FIRST_WINDOW_IMMEDIATE
    /*
     * 这个立即补采窗口不影响后续定时器相位。
     * 后续仍然先等 RX_FIRST_WINDOW_DELAY_TICK，再进入 1ms 周期。
     */
    rx_window_pending = 1U;
#endif

    /*
     * 关键修改：
     * GTIMER0 的周期仍然是 1000us，只是启动时预装 CNT，
     * 让“第一次溢出”发生在 RX_FIRST_WINDOW_DELAY_TICK 后。
     * 第一次溢出以后，硬件自动回到 1ms 周期，不需要在中断里改周期。
     */
    rx_timer_restart_with_first_delay();

    uart_debug_request(UART_DBG_REASON_SYNC, 1U);
}


static void rx_enter_block_state(uint8_t debug_reason)
{
    rx_valid_count       = 0U;
    rx_lost_window_count = 0U;
    rx_seen_once         = 0U;
    rx_window_pending    = 0U;
    rx_sync_pending      = 0U;
    rx_resync_pending    = 0U;
    rx_resync_window_count = 0U;
    rx_adc_valid_state   = 0U;
    rx_first_interval_pending = 0U;
    rx_last_trigger_interval_tick = 0U;
    g_dbg_last_adc_ok    = 0U;

    rx_timer_stop_clear();
    rx_output_block();

    if (rx_light_state)
    {
        rx_light_state = 0U;
    }

    /* 遮光后重新打开 P1_4 中断，等待下一次来光重新同步。 */
    rx_sync_irq_enable();

    uart_debug_request(debug_reason, 1U);
}


/*
 * 主循环上下文里的强制重同步。
 *
 * 这个函数不能放在 GTIMER0 中断或 GPIO 中断里调用，原因是里面会重新打开
 * P1_4 同步中断，最终会调用 gpio_irq_set()，C51 会对 ISR 和主流程双入口
 * 调用库函数报 L15 警告。
 *
 * 强制重同步只重新校准采样相位，不改变当前输出状态：
 * - 当前已经有光：输出继续保持有光，等待下一次 P1_4 边沿后恢复 1ms 窗口。
 * - 当前实际遮光：因为没有新的 P1_4 边沿，后续不会误确认有光；遮光正常由
 *   无效窗口逻辑处理，所以这里不主动拉低输出。
 */
static void rx_force_resync_from_main(void)
{
    rx_resync_pending = 0U;
    rx_resync_window_count = 0U;

    rx_seen_once = 0U;
    rx_window_pending = 0U;
    rx_sync_pending = 0U;
    rx_adc_valid_state = 0U;
    rx_first_interval_pending = 0U;
    rx_last_trigger_interval_tick = 0U;

    rx_timer_stop_clear();

    /* 重新打开 P1_4，等待下一次发射脉冲边沿重新同步。 */
    rx_sync_irq_enable();

    uart_debug_request(UART_DBG_REASON_RESYNC, 1U);
}


/*
 * 每处理一个 ADC 窗口后累计一次。
 *
 * 为了避免真实遮光时刚好碰到重同步周期，导致输出保持有光等待同步，
 * 这里只在“本窗口仍然有效 adc_ok=1”时触发强制重同步。
 * 如果已经开始连续无效，优先走 RX_LOST_WINDOW_COUNT 遮光判定。
 */
static void rx_resync_count_after_window(uint8_t adc_ok)
{
#if RX_RESYNC_ENABLE
    if (RX_RESYNC_WINDOW_COUNT == 0U)
    {
        return;
    }

    if (rx_seen_once == 0U)
    {
        rx_resync_window_count = 0U;
        return;
    }

    if (rx_resync_window_count < 65535U)
    {
        rx_resync_window_count++;
    }

    if ((rx_resync_window_count >= RX_RESYNC_WINDOW_COUNT) && (adc_ok != 0U))
    {
        rx_resync_pending = 1U;
    }
#else
    (void)adc_ok;
#endif
}


static void rx_process_adc_window(void)
{
    uint16_t avg_adc;
    uint16_t sample_count;
    uint8_t adc_ok;

    if (rx_seen_once == 0U)
    {
        return;
    }

    sample_count = 0U;
    avg_adc = rx_adc_scan_window_average(ADC_CHANNEL_1,
                                         RX_ADC_WINDOW_WIDTH_TICK,
                                         &sample_count);

    adc_data  = avg_adc;
    adc_data1 = sample_count;
    adcount   = sample_count;
    g_dbg_last_elapsed_tick = rx_last_trigger_interval_tick;

#if RX_ADC_HYS_ENABLE
    adc_ok = rx_adc_hysteresis_check(avg_adc);
#else
    if ((avg_adc < RX_ADC_MAX_VALID) && (avg_adc >= adc_set))
    {
        adc_ok = 1U;
    }
    else
    {
        adc_ok = 0U;
    }
#endif

    g_dbg_last_adc_ok = adc_ok;

    if (adc_ok)
    {
        rx_lost_window_count = 0U;

        if (rx_valid_count < RX_CONFIRM_COUNT)
        {
            rx_valid_count++;
        }

#if UART_ADC_DEBUG_ENABLE
        uart_debug_inc16(&g_dbg_valid_pulse_count);
#endif

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
            uart_debug_request(UART_DBG_REASON_VALID_WINDOW, 0U);
        }
    }
    else
    {
        rx_valid_count = 0U;

        if (rx_lost_window_count < RX_LOST_WINDOW_COUNT)
        {
            rx_lost_window_count++;
        }

#if UART_ADC_DEBUG_ENABLE
        uart_debug_inc16(&g_dbg_adc_fail_count);
#endif

        if (rx_lost_window_count >= RX_LOST_WINDOW_COUNT)
        {
#if UART_ADC_DEBUG_ENABLE
            uart_debug_inc16(&g_dbg_timeout_count);
#endif
            rx_enter_block_state(UART_DBG_REASON_TIMEOUT);
        }
        else
        {
            uart_debug_request(UART_DBG_REASON_ADC_FAIL, 0U);
        }
    }

    /* 如果本窗口没有触发遮光状态机退出，则按窗口周期累计重同步。 */
    if (rx_seen_once != 0U)
    {
        rx_resync_count_after_window(adc_ok);
    }
}


/*======================================================================
 * 十、串口调试函数
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
     * S   ：最近一次 ADC 窗口平均值。
     * A   ：最近一次窗口内 ADC 采样次数。
     * TH  ：ADC 有效上阈值。
     * HYS ：ADC 迟滞宽度。
     * OFF ：ADC 无效下阈值。
     * EL  ：最近一次窗口触发间隔；第一次可能是起点间隔，后续默认 1000us。
     * VC  ：连续有效窗口计数。
     * L   ：当前有光状态。
     * AS  ：ADC 迟滞状态。
     * OK  ：最近一次窗口判断结果。
     * IR  ：P1_4 同步中断次数。
     * SH  ：窗口积压/丢失次数。
     * AF  ：ADC 窗口无效次数。
     * VP  ：ADC 窗口有效次数。
     * TO  ：遮光次数。
     * R   ：打印原因码。
     *
     * R：
     * 1 = 上电
     * 2 = 首次同步
     * 3 = ADC 窗口无效
     * 4 = ADC 窗口有效
     * 5 = 有光确认
     * 6 = 遮光
     * 7 = 窗口积压
     * 8 = 周期性强制重同步
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
 * 十一、主函数
 *====================================================================*/

void main(void)
{
    system_init();

    GPIO_Init();

    uart_init();

    /*
     * GTIMER0 作为 ADC 扫描窗口周期定时器。
     * 24MHz / 24 = 1MHz，1 tick = 1us。
     * RX_WINDOW_INTERVAL_TICK = 1000 表示后续每 1ms 产生一次窗口触发。
     * 同步后的第一次触发时间由 RX_FIRST_WINDOW_DELAY_TICK 通过 CNT 预装实现。
     */
    gtimer0_count_init(RX_WINDOW_INTERVAL_TICK, 24 - 1);
    gtimer0_irq_init(GTIMER_IRQ_ENABLE, GTIMER0_UIE_IRQ, gtimer0_UECallBack);

    /* 上电先不启动定时器，等待第一次 P1_4 同步边沿。 */
    rx_timer_stop_clear();

    /* ADC_CHANNEL_1：接收信号 ADC；ADC_CHANNEL_2：预留 ADJ 电位器。 */
    adc_clk_config(ADC_CLKSOURCE_SYSCLK, ADC_VREFSOURCE_AVDD33, 4, ADC_ENABLE);
    adc_sample_clk_config(ADC_SAMPCLK_4);
    adc_io_config(ADC_CHANNEL_1 | ADC_CHANNEL_2);
    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);

    rx_output_block();
    rx_sync_irq_enable();

    uart_debug_request(UART_DBG_REASON_BOOT, 1U);

    while (1)
    {
        /*
         * P1_4 interrupt only sets rx_sync_pending.
         * Do the actual sync start here so gpio_irq_set() is never called from GPIO_IRQHandler.
         */
        if (rx_sync_pending != 0U)
        {
            rx_sync_pending = 0U;

            if (rx_seen_once == 0U)
            {
                rx_start_periodic_window_from_sync();
            }
        }

        /*
         * 定时器中断只置位，不在中断里做 ADC 连续扫描。
         * 这样可以避免 40us 级窗口扫描长时间占用中断。
         */
        if (rx_window_pending != 0U)
        {
            rx_window_pending = 0U;
            rx_process_adc_window();
        }

        /*
         * 周期性重同步也在主循环处理。
         * 这样 rx_sync_irq_enable()/gpio_irq_set() 不会从中断上下文调用，避免 L15。
         */
        if (rx_resync_pending != 0U)
        {
            rx_force_resync_from_main();
        }

        uart_adc_debug_process();
    }
}


/*======================================================================
 * 十二、GTIMER0 周期窗口触发回调
 *====================================================================*/

void gtimer0_UECallBack(void)
{
    if (rx_seen_once == 0U)
    {
        rx_first_interval_pending = 0U;
        REG_GTIM0_CR0 &= ~(1 << 0);
        REG_GTIM0_CNT0 = 0x00;
        REG_GTIM0_CNT1 = 0x00;
        return;
    }

    if (rx_first_interval_pending != 0U)
    {
        /*
         * 这是同步后的第一次窗口。
         * 它的触发间隔不是 1000us，而是 RX_FIRST_WINDOW_DELAY_TICK。
         * 第一次溢出之后，GTIMER0 自动从 0 重新计数，
         * 后续自然恢复为每 1000us 触发一次。
         */
        rx_first_interval_pending = 0U;
        rx_last_trigger_interval_tick = rx_first_delay_cached_tick;
    }
    else
    {
        rx_last_trigger_interval_tick = RX_WINDOW_INTERVAL_TICK;
    }

    if (rx_window_pending != 0U)
    {
#if UART_ADC_DEBUG_ENABLE
        uart_debug_inc16(&g_dbg_short_edge_count);
#endif
        uart_debug_request(UART_DBG_REASON_WINDOW_OVERRUN, 0U);
    }
    else
    {
        rx_window_pending = 1U;
    }
}


/*======================================================================
 * 十三、P1_4 比较器同步中断
 *====================================================================*/

void GPIO_IRQHandler(void) interrupt 0
{
    if (gpio_irq_get(P1_4))
    {
        gpio_irq_clr(P1_4);

#if UART_ADC_DEBUG_ENABLE
        uart_debug_inc16(&g_dbg_irq_count);
#endif

        /* 已同步运行时不再用 P1_4 参与判断。 */
        if ((rx_seen_once != 0U) || (rx_sync_pending != 0U))
        {
            return;
        }

        rx_sync_pending = 1U;
    }
}


void gpio_UECallBack(void)
{
    /* 不用二次回调。 */
}


/*======================================================================
 * 十四、GPIO 初始化
 *====================================================================*/

void GPIO_Init(void)
{
    /* P1_0 输出：用作 ADC 窗口示波器观测脚，窗口期间拉低。 */
    REG_P10_CFG = 0x00;

    gpio_init(P1_0);
    gpio_dir_set(P1_0, GPIO_DIR_OUT);
    gpio_dr_set(P1_0, GPIO_SR_HIGH);
    gpio_io_set(P1_0, GPIO_HIGH);

    /* P1_2 输出 */
    REG_P12_CFG = 0x00;

    gpio_init(P1_2);
    gpio_dir_set(P1_2, GPIO_DIR_OUT);
    gpio_dr_set(P1_2, GPIO_SR_HIGH);
    gpio_io_set(P1_2, GPIO_LOW);

    /* P1_3 输出 */
    REG_P13_CFG = 0x00;

    gpio_init(P1_3);
    gpio_dir_set(P1_3, GPIO_DIR_OUT);
    gpio_dr_set(P1_3, GPIO_SR_HIGH);
    gpio_io_set(P1_3, GPIO_LOW);

    /* P1_4 输入中断：只用于第一次同步。 */
    gpio_init(P1_4);
    gpio_dir_set(P1_4, GPIO_DIR_IN);
    gpio_dr_set(P1_4, GPIO_SR_HIGH);
    gpio_in_enable(P1_4, IN_ENABLE);
    gpio_irq_set(P1_4, GPIO_IRQ_ENABLE, gpio_UECallBack);

    /* 保留原来的 P1_4 特殊配置。 */
    P1AH &= ~(0x02);
    P1AH |=  (0x01);

    REG_P14_CFG = 0x20;

    /* P1_5 输入，保留原配置。 */
    REG_P15_CFG = 0x00;

    gpio_init(P1_5);
    gpio_dir_set(P1_5, GPIO_DIR_IN);
    gpio_in_enable(P1_5, IN_ENABLE);
}

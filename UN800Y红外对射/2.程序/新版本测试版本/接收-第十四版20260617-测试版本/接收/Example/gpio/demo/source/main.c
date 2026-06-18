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

/*
 * 【有光确认次数】
 * 连续 3 个 ADC 窗口有效才确认有光。
 * 作用：防止单个窗口偶然采到噪声就立即输出有光。
 * 影响：越大，来光响应越慢；越小，越容易误亮。
 */
#define RX_CONFIRM_COUNT              1U

/*
 * 定时器窗口间隔周期。
 * 默认 1000us，也就是同步后每隔 1ms 打开一次 ADC 扫描窗口。
 * 如果发射端周期不是 1ms，只需要改这里。
 */
/*
 * 【窗口周期】
 * GTIMER0 后续每 1000us 触发一次 ADC 扫描窗口。
 * 注意：这里只是“窗口起点周期”，不是 ADC 窗口宽度。
 * 如果发射端有效脉冲周期不是 1ms，这里必须跟着改。
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
/*
 * 【第一次窗口延迟】
 * 收到 P1_4 同步边沿后，不是立刻采 ADC，而是等待 880us 再打开第一次窗口。
 * 这个值决定采样窗口相对于发射脉冲的位置。
 *
 * 排查点：
 * - 如果这个值偏差太大，窗口可能扫不到脉冲，导致误遮光。
 * - 如果窗口扫到脉冲尾部，平均值会偏低，可能刚好低于阈值。
 */
#define RX_FIRST_WINDOW_DELAY_TICK    880U

/*
 * ADC 连续扫描窗口宽度。
 * 默认 40us。
 *
 * 发射有效脉宽约 25us 时：
 * - 窗口太小：容易因为相位偏差漏掉脉冲；
 * - 窗口太大：平均值会被窗口内的低电平拉低，需要适当降低 adc_set；
 * - 建议先用 30~60us 调试。
 */
/*
 * 【ADC 窗口宽度】
 * 每次窗口持续 100us，在这 100us 内连续读 ADC 并求平均。
 *
 * 排查点：
 * - 发射有效脉宽如果只有约 25us，100us 求平均会把峰值稀释。
 * - 例如 25us 有效 + 75us 无信号，平均值大约只有峰值的 1/4。
 * - 所以 adc_set 必须按“窗口平均值”标定，不是按峰值标定。
 */
#define RX_ADC_WINDOW_WIDTH_TICK      100U

/*
 * 连续多少个 ADC 窗口无效后判定遮光。
 * 默认 3 个窗口，即约 3ms 遮光确认，响应约 333Hz。
 * 如果想更快，可以改 2；如果现场抖动明显，可以改 4 或 5。
 */
//#define RX_LOST_WINDOW_COUNT          3U

/* 
 *每组 ADC 判断窗口数。
 */
#define RX_DECISION_GROUP_WINDOW_COUNT   1U

#if (RX_CONFIRM_COUNT > RX_DECISION_GROUP_WINDOW_COUNT)
#error "RX_CONFIRM_COUNT must not exceed RX_DECISION_GROUP_WINDOW_COUNT"
#endif

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
/*
 * 【强制重同步窗口数】
 * 当前为 3U，表示每 3 个有效窗口就强制重同步一次。
 *
 * 重要排查点：
 * - 这个值和上方注释里“默认 100 个周期”不一致。
 * - 3U 太频繁，大约每 3ms 停一次 GTIMER0 ADC 窗口。
 * - 停止 ADC 窗口后，遮光不再走 rx_lost_window_count，
 *   而是依赖 GTIMER1 的 2ms 同步等待超时。
 * - 如果 GTIMER1 超时没有被主循环及时处理，输出可能保持有光。
 *
 * 建议调试：先改 100U 或关闭 RX_RESYNC_ENABLE 验证。
 */
#define RX_RESYNC_WINDOW_COUNT        1U

/*
 * 强制重同步等待超时。
 * 周期性强制重同步时，接收端会停止 GTIMER0 窗口采样并重新打开 P1_4。
 * 如果 2ms 内没有收到新的 P1_4 同步边沿，就认为当前已经遮光。
 * 这个超时只用于“已经有光后的强制重同步等待期”，上电遮光等待不启动。
 */
#define RX_SYNC_WAIT_TIMEOUT_TICK     1500U

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
/*
 * 【ADC 迟滞宽度】
 * adc_set 是打开阈值，adc_set - 30 是关闭阈值。
 * 例如 adc_set=200，则：
 *   从无效到有效：avg_adc >= 200
 *   从有效到无效：avg_adc <= 170
 *
 * 排查点：
 * 遮光后如果窗口平均值仍然在 170 以上，迟滞会继续认为有效。
 */
#define RX_ADC_HYS_VALUE              25U

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

/*
 * 串口调试只打印窗口平均 ADC 和当前阈值。
 *
 * UART_ADC_DEBUG_EVENT_DIV = 1 ：每个 ADC 窗口打印一次。
 * 如果窗口周期仍是 1ms，普通 115200 串口仍可能拖慢主循环；
 * 现场只看趋势时建议改成 5、10、20。
 */
/*
 * 【串口调试开关】
 * 当前关闭。
 * 若要观察遮光问题，建议不要高频打印全状态，只打印关键量：
 * adc_set, adc_data, adc_ok, rx_lost_window_count, rx_seen_once, rx_light_state。
 */
#define UART_ADC_DEBUG_ENABLE              0U
#define UART_ADC_DEBUG_EVENT_DIV           10U
#define UART_ADC_DEBUG_STATE_CHANGE_PRINT  0U

#define UART_DBG_REASON_BOOT               1U
#define UART_DBG_REASON_SYNC               2U
#define UART_DBG_REASON_ADC_FAIL           3U
#define UART_DBG_REASON_VALID_WINDOW       4U
#define UART_DBG_REASON_LIGHT_ON           5U
#define UART_DBG_REASON_TIMEOUT            6U
#define UART_DBG_REASON_WINDOW_OVERRUN     7U
#define UART_DBG_REASON_RESYNC             8U
#define UART_DBG_REASON_SYNC_TIMEOUT       9U


/**** 全局变量，保留原命名 ****/

/*
 * 【变量分组理解】
 *
 * adc_data / adc_data1：
 *   用于观察最近一次窗口平均 ADC 和采样次数。
 *
 * rx_valid_count / rx_lost_window_count：
 *   真正决定“有光确认”和“遮光确认”的连续计数器。
 *
 * rx_light_state：
 *   当前输出状态。1 表示输出保持有光；0 表示输出遮光。
 *
 * rx_seen_once：
 *   当前是否处于“已经同步并且 GTIMER0 窗口采样中”。
 *   这是排查遮光不判断的关键变量：
 *   - rx_seen_once = 1：遮光靠 ADC 无效窗口计数判断。
 *   - rx_seen_once = 0：ADC 窗口停止，只能等待 P1_4 同步或 GTIMER1 超时。
 *
 * rx_window_pending：
 *   GTIMER0 中断置位，主循环清除并处理 ADC 窗口。
 *   如果主循环被串口或 ADC 阻塞太久，可能出现窗口积压。
 *
 * rx_resync_pending：
 *   强制重同步请求。置位后主循环会停止 GTIMER0 并打开 P1_4。
 *
 * rx_sync_wait_timeout_pending：
 *   GTIMER1 超时标志。只有强制重同步等待超时时才会置位。
 */

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

/*
 * 强制重同步等待 P1_4 边沿期间的 2ms 超时标志。
 * GTIMER1 中断只置位，主循环里真正执行遮光处理，避免在中断里调用 GPIO 库函数。
 */
static volatile uint8_t rx_sync_wait_timeout_pending = 0;
static volatile uint8_t rx_sync_wait_timer_running   = 0;

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

/* 当前同步周期内已处理的窗口数，3 个窗口为一组。 */
static volatile uint8_t rx_group_window_count = 0U;


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
static void rx_sync_timeout_timer_stop_clear(void);
static void rx_sync_timeout_timer_start(void);
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
static void rx_handle_sync_wait_timeout_from_main(void);
static void rx_resync_count_after_window(void);
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

/*
 * 读取 GTIMER0 当前计数值。
 * 低 8 位来自 CNT0，高 8 位来自 CNT1。
 *
 * 用途：
 * - ADC 扫描窗口开始时记录 start_tick。
 * - 循环读 ADC 时不断计算已经扫描了多少 us。
 */
static uint16_t rx_timer_get_count(void)
{
    uint16_t cnt;

    cnt  = REG_GTIM0_CNT0;
    cnt |= ((uint16_t)REG_GTIM0_CNT1 << 8);

    return cnt;
}


/*
 * 计算从 start_tick 到当前 GTIMER0 计数的经过时间。
 *
 * 注意：
 * GTIMER0 周期是 RX_WINDOW_INTERVAL_TICK，计数会回卷。
 * 这里做了回卷处理。
 *
 * 风险点：
 * 如果 ADC 窗口宽度接近或超过定时器周期，计算会不可靠。
 * 当前 100us 远小于 1000us，所以正常。
 */
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


/*
 * 获取并保护第一次窗口延迟。
 *
 * 作用：
 * RX_FIRST_WINDOW_DELAY_TICK 是宏配置，理论上用户可能配成 0 或大于周期。
 * 这里限制到 1~RX_WINDOW_INTERVAL_TICK 范围内。
 */
static uint16_t rx_first_delay_get(void)
{
    uint16_t first_delay_tick;

    first_delay_tick = RX_FIRST_WINDOW_DELAY_TICK;//== 窗口等待时间

    /*
     * 0us 容易造成定时器重装异常。
     * 如果确实要同步后马上补采，使用 RX_FIRST_WINDOW_IMMEDIATE。
     */
    if (first_delay_tick == 0U)//== 不能立刻就开窗
    {
        first_delay_tick = 1U;
    }

    /*
     * 这里做保护：起点间隔最大不超过后续周期。
     * 本需求主要用于“前移一点”，所以通常应小于 1000us。
     */
    if (first_delay_tick > RX_WINDOW_INTERVAL_TICK)//== 第一次的间隔不能比周期还长
    {
        first_delay_tick = RX_WINDOW_INTERVAL_TICK;
    }

    return first_delay_tick;
}


/*
 * 把“第一次窗口延迟”换算成 GTIMER0 的 CNT 预装值。
 *
 * 原理：
 * GTIMER0 自动重装周期是 1000us。
 * 如果希望第一次 880us 后溢出，就把 CNT 预装成 1000-880=120。
 * 这样从 120 计到 1000 只需要 880us。
 */
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


/*
 * 停止并清零 GTIMER0。
 *
 * 影响：
 * 一旦调用这个函数，ADC 周期窗口就停止了。
 * 遮光判断不再通过 rx_process_adc_window() 进行。
 *
 * 排查遮光不判断时，重点看谁调用了它：
 * - rx_enter_block_state()：正常遮光后停止。
 * - rx_force_resync_from_main()：强制重同步时停止，这是可疑路径。
 */
static void rx_timer_stop_clear(void)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;
}



/*
 * 用指定 CNT 预装值启动 GTIMER0。
 *
 * 注意：
 * 这里只改 CNT，不改自动重装周期。
 * 第一次溢出时间由 preload_tick 决定，后续仍然回到 1000us 周期。
 */
static void rx_timer_restart_with_preload(uint16_t preload_tick)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = (uint8_t)(preload_tick & 0x00FFU);
    REG_GTIM0_CNT1 = (uint8_t)((preload_tick >> 8) & 0x00FFU);

    REG_GTIM0_CR0 |= (1 << 0);
}


/*
 * 按 RX_FIRST_WINDOW_DELAY_TICK 启动 GTIMER0。
 *
 * 用于：
 * 每次 P1_4 同步成功后，重新建立 ADC 窗口相位。
 */
static void rx_timer_restart_with_first_delay(void)
{
    uint16_t first_delay_tick;
    uint16_t preload_tick;

    first_delay_tick = rx_first_delay_get();//== 获取第一次延迟计数值880us
    preload_tick     = rx_first_delay_to_timer_preload(first_delay_tick);//== CNT计数预先装一点让他计数更快，这样第一次溢出就会小于设定时间，使得周期迁移

    rx_first_delay_cached_tick = first_delay_tick;//== 记录第一次迁移周期的间隔
    rx_last_trigger_interval_tick = 0U;

    rx_timer_restart_with_preload(preload_tick);//== 把迁移的设定值写入
}


/* 
 * 停止并清零 GTIMER1 同步等待超时定时器。
 *
 * 用于：
 * - 收到新的同步边沿后，说明没有超时，关闭 GTIMER1。
 * - 进入遮光状态时，清理所有等待状态。
 */
static void rx_sync_timeout_timer_stop_clear(void)
{
    rx_sync_wait_timer_running   = 0U;
    rx_sync_wait_timeout_pending = 0U;

    REG_GTIM1_CR0 &= ~(1 << 0);
    REG_GTIM1_CNT0 = 0x00;
    REG_GTIM1_CNT1 = 0x00;
}


/*
 * 启动 GTIMER1 2ms 一次性超时。
 *
 * 只有强制重同步并且当前已经处于有光输出时才启动。
 *
 * 排查点：
 * 如果遮光发生在强制重同步等待期，最终能不能遮光，取决于：
 * - GTIMER1 是否真的中断；
 * - gtimer1_UECallBack() 是否置位 rx_sync_wait_timeout_pending；
 * - 主循环是否调用 rx_handle_sync_wait_timeout_from_main()。
 */
static void rx_sync_timeout_timer_start(void)
{
    rx_sync_wait_timeout_pending = 0U;
    rx_sync_wait_timer_running   = 1U;

    REG_GTIM1_CR0 &= ~(1 << 0);
    REG_GTIM1_CNT0 = 0x00;
    REG_GTIM1_CNT1 = 0x00;
    REG_GTIM1_CR0 |= (1 << 0);
}


/*
 * 打开 P1_4 同步中断。
 *
 * 用于：
 * - 上电等待第一次来光同步。
 * - 遮光后等待下一次来光同步。
 * - 强制重同步时等待下一次边沿校准相位。
 *
 * 风险点：
 * 遮光状态下如果 P1_4 有毛刺，可能误认为重新同步。
 */
static void rx_sync_irq_enable(void)
{
    gpio_irq_clr(P1_4);
    gpio_irq_set(P1_4, GPIO_IRQ_ENABLE, gpio_UECallBack);
}


/*
 * 关闭 P1_4 同步中断。
 *
 * 正常同步成功后关闭，防止每个发射脉冲都进 GPIO 中断。
 * 后续检测完全交给 GTIMER0 + ADC 窗口。
 */
static void rx_sync_irq_disable(void)
{
    gpio_irq_clr(P1_4);
    gpio_irq_set(P1_4, GPIO_IRQ_DISABLE, gpio_UECallBack);
}


/*======================================================================
 * 七、输出控制
 *====================================================================*/

/*
 * 输出有光状态。
 * 当前 P1_2/P1_3 都拉高。
 *
 * 注意：
 * 这里没有修改 rx_light_state，只改物理输出。
 * rx_light_state 在 rx_process_adc_window() 中置 1。
 */
static void rx_output_light(void)
{
    gpio_io_set(P1_2, GPIO_HIGH);
    gpio_io_set(P1_3, GPIO_HIGH);
//    gpio_io_set(P1_0, GPIO_LOW);
}


/*
 * 输出遮光状态。
 * 当前 P1_2/P1_3 都拉低。
 *
 * 注意：
 * 这里没有修改 rx_light_state，只改物理输出。
 * rx_light_state 在 rx_enter_block_state() 中清 0。
 */
static void rx_output_block(void)
{
    gpio_io_set(P1_2, GPIO_LOW);
    gpio_io_set(P1_3, GPIO_LOW);
//    gpio_io_set(P1_0, GPIO_HIGH);
}


/*======================================================================
 * 八、ADC 读取、窗口积分平均与迟滞判断
 *====================================================================*/

/*
 * 单次 ADC 读取。
 *
 * 流程：
 * 1. 启动指定通道转换。
 * 2. 等待 ADC 硬件忙标志清除。
 * 3. 等待转换完成标志。
 * 4. 清完成标志。
 * 5. 读取 ADC 值。
 *
 * 排查点：
 * 这里是阻塞等待。如果 ADC 异常卡住，主循环会卡死，
 * GTIMER0/GTIMER1 中断虽然可能置位，但主循环无法处理状态机。
 */
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


/*
 * 在一个固定时间窗口内连续读取 ADC，并求平均值。
 *
 * 输入：
 * - ch：ADC 通道。
 * - window_tick：窗口宽度，单位 us。
 * - sample_count：返回本窗口有效采样次数。
 *
 * 输出：
 * - 返回窗口平均 ADC。
 * - 如果窗口内没有有效采样，返回 ADC_INVALID。
 *
 * 重要理解：
 * 这里求的是“窗口平均值”，不是峰值。
 * 所以即使真实脉冲峰值很高，只要有效脉宽小于窗口宽度，
 * 平均值就会被低电平部分拉低。
 *
 * 排查遮光不判断时看两点：
 * - 遮光后 avg_adc 是否仍然大于关闭阈值；
 * - sample_count 是否异常为 0 或异常很少。
 */
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


/*
 * 计算 ADC 迟滞关闭阈值。
 *
 * 如果 adc_set=200，RX_ADC_HYS_VALUE=30，则关闭阈值为 170。
 *
 * 迟滞含义：
 * - 无效 -> 有效：必须 >= 200。
 * - 有效 -> 无效：必须 <= 170。
 *
 * 所以遮光后 ADC 如果落在 171~199，状态仍会维持有效。
 */
static uint16_t rx_adc_calc_off_threshold(void)
{
    if (adc_set > RX_ADC_HYS_VALUE)
    {
        return (uint16_t)(adc_set - RX_ADC_HYS_VALUE);
    }

    return 0U;
}


/*
 * ADC 迟滞判断。
 *
 * 返回：
 * - 1：当前窗口认为有效。
 * - 0：当前窗口认为无效。
 *
 * 关键变量：rx_adc_valid_state
 * - 不是每次简单比较 adc_set。
 * - 一旦进入有效状态，必须低于 off_threshold 才会退出。
 *
 * 排查点：
 * 遮光后如果 avg_adc 没有降到 adc_set - RX_ADC_HYS_VALUE 以下，
 * adc_ok 会继续为 1，rx_lost_window_count 永远不会累计。
 */
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

/*
 * 收到 P1_4 同步边沿后，启动周期 ADC 窗口采样。
 *
 * 状态变化：
 * - 停止 GTIMER1 同步等待超时。
 * - rx_seen_once = 1，表示进入“已同步采样状态”。
 * - 清空有效/无效连续计数。
 * - 关闭 P1_4 中断，避免后续每个脉冲都进 GPIO 中断。
 * - 启动 GTIMER0，等待第一次 ADC 窗口。
 *
 * 排查点：
 * 只要这个函数被调用，rx_lost_window_count 会被清零，
 * 所以遮光时如果 P1_4 毛刺误触发同步，会打断遮光累计。
 */
static void rx_start_periodic_window_from_sync(void)
{
    /* 已经收到同步边沿，关闭 2ms 同步等待超时定时器。 */
    rx_sync_timeout_timer_stop_clear();//== 关掉定时器1

    rx_seen_once         = 1U;//== 同步成功
    rx_window_pending    = 0U;
    rx_sync_pending      = 0U;
    rx_resync_pending    = 0U;
    rx_resync_window_count = 0U;
    rx_valid_count       = 0U;//== 清除无效计数
    rx_lost_window_count = 0U;
    rx_adc_valid_state   = 0U;
    rx_first_interval_pending = 1U;//== 告诉系统是第一次窗口
    rx_last_trigger_interval_tick = 0U;
    g_dbg_last_adc_ok    = 0U;
    rx_group_window_count = 0U;

    /* 同步完成后，P1_4 不再反复进中断，后续由 GTIMER0 周期窗口采样。 */
    rx_sync_irq_disable();//== 关闭同步中断

#if RX_FIRST_WINDOW_IMMEDIATE
    /*
     * 这个立即补采窗口不影响后续定时器相位。
     */
    rx_window_pending = 1U;
#endif

    /*
     * 关键修改：
     * GTIMER0 的周期仍然是 1000us，只是启动时预装 CNT，
     * 让“第一次溢出”发生在 RX_FIRST_WINDOW_DELAY_TICK 后。
     * 第一次溢出以后，硬件自动回到 1ms 周期，不需要在中断里改周期。
     */
    rx_timer_restart_with_first_delay();//== 设置第一次溢出的周期迁移
}


/*
 * 进入遮光状态的统一出口。
 *
 * 只有两条正常路径会调用：
 * 1. ADC 连续无效窗口达到 RX_LOST_WINDOW_COUNT。
 * 2. 强制重同步等待 P1_4 超时。
 *
 * 动作：
 * - 清所有计数和 pending 标志。
 * - 停 GTIMER0/GTIMER1。
 * - 输出遮光。
 * - rx_light_state 清 0。
 * - 重新打开 P1_4，等待下一次来光同步。
 */
static void rx_enter_block_state(uint8_t debug_reason)
{
    debug_reason=0;
    rx_valid_count       = 0U;
    rx_lost_window_count = 0U;
    rx_seen_once         = 0U;
    rx_window_pending    = 0U;
    rx_sync_pending      = 0U;
    rx_resync_pending    = 0U;
    rx_sync_wait_timeout_pending = 0U;
    rx_sync_wait_timer_running   = 0U;
    rx_resync_window_count = 0U;
    rx_adc_valid_state   = 0U;
    rx_first_interval_pending = 0U;
    rx_last_trigger_interval_tick = 0U;
    g_dbg_last_adc_ok    = 0U;
    rx_group_window_count = 0U;

    rx_timer_stop_clear();//== 停止并清零 GTIMER0
    rx_sync_timeout_timer_stop_clear();//== 停止并清零 GTIMER1 同步等待超时定时器为下一次同步准备
	
    rx_output_block();//== 遮光输出

    if (rx_light_state)//== 遮光状态
    {
        rx_light_state = 0U;
    }

    /* 遮光后重新打开 P1_4 中断，等待下一次来光重新同步。 */
    rx_sync_irq_enable();

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
    rx_sync_wait_timeout_pending = 0U;
    rx_sync_wait_timer_running   = 0U;
    rx_adc_valid_state = 0U;
    rx_first_interval_pending = 0U;
    rx_last_trigger_interval_tick = 0U;
    rx_group_window_count = 0U;

    /*
     * 停止 GTIMER0：这是强制重同步的核心动作。
     */
    rx_timer_stop_clear();
    
    /*
     * 当前仍然处于有光输出状态，开始 2ms 同步等待超时。
     * 如果 2ms 内没有新的 P1_4 同步边沿，主循环会判定遮光。
     */
    if (rx_light_state != 0U)
    {
        rx_sync_timeout_timer_start();
    }
    
    /* 重新打开 P1_4，等待下一次发射脉冲边沿重新同步。 */
    rx_sync_irq_enable();
		
   
}


/*
 * 处理强制重同步期间的 2ms 超时。
 *
 * 条件：
 * - rx_sync_wait_timeout_pending = 1，说明 GTIMER1 已经超时。
 * - rx_seen_once == 0，说明还没有重新同步成功。
 * - rx_light_state != 0，说明输出之前还保持有光。
 *
 * 满足后调用 rx_enter_block_state() 判定遮光。
、
 */
static void rx_handle_sync_wait_timeout_from_main(void)
{
    if (rx_sync_wait_timeout_pending == 0U)
    {
        return;
    }

    rx_sync_wait_timeout_pending = 0U;
    rx_sync_wait_timer_running   = 0U;

    if ((rx_seen_once == 0U) && (rx_light_state != 0U))
    {
        /*
         * 仍未重新同步，并且输出还保持有光：
         * 这时同步等待超时应当直接判定遮光。
         */

        rx_enter_block_state(UART_DBG_REASON_SYNC_TIMEOUT);
    }
}


/*
 * 每处理一个 ADC 窗口后累计一次。
 *
 * 为了避免真实遮光时刚好碰到重同步周期，导致输出保持有光等待同步，
 * 这里只在“本窗口仍然有效 adc_ok=1”时触发强制重同步。
 * 如果已经开始连续无效，优先走 RX_LOST_WINDOW_COUNT 遮光判定。
 */
/*
 * 每处理完一个 ADC 窗口后，累计强制重同步计数。
 *
 * 当前代码逻辑：
 * - 只要 rx_seen_once=1，就累计 rx_resync_window_count。
 * - 当累计到 RX_RESYNC_WINDOW_COUNT，就置位 rx_resync_pending。
 */
static void rx_resync_count_after_window(void)
{
#if RX_RESYNC_ENABLE
    if (RX_RESYNC_WINDOW_COUNT == 0U)//== 次数上限0不计数
    {
        return;
    }

    if (rx_seen_once == 0U)//== 没同步不计数
    {
        rx_resync_window_count = 0U;
        return;
    }

    if (rx_resync_window_count < 65535U)//== 次数追加
    {
        rx_resync_window_count++;
    }

    if ((rx_resync_window_count >= RX_RESYNC_WINDOW_COUNT))//== 大于强制同步次数设置强制同步标志
    {
        rx_resync_pending = 1U;
        
    }
#else
    (void)0;
#endif
}


/*
 * 处理一次 ADC 窗口，是整个遮光/有光判断的核心函数。
 *
 * 正常遮光路径：
 * 1. 扫描一个 ADC 窗口，得到 avg_adc。
 * 2. 迟滞判断得到 adc_ok。
 * 3. adc_ok=0：rx_lost_window_count++。
 * 4. 连续无效达到 RX_LOST_WINDOW_COUNT：调用 rx_enter_block_state()。
 *
 * 如果明明遮挡但不遮光，重点看：
 * - 这个函数有没有持续执行；
 * - adc_ok 是否真的变成 0；
 * - rx_lost_window_count 有没有被清零；
 * - 是否执行完窗口后马上触发了 rx_resync_pending。
 */
static void rx_process_adc_window(void)
{
    uint16_t avg_adc;
    uint16_t sample_count;
    uint8_t adc_ok;

    if (rx_seen_once == 0U)//== 没同步退出
    {
        return;
    }

    sample_count = 0U;
    avg_adc = rx_adc_scan_window_average(ADC_CHANNEL_1,
                                         RX_ADC_WINDOW_WIDTH_TICK,
                                         &sample_count);//== 获取平均值

    adc_data  = avg_adc;//== 窗口平均
    adc_data1 = sample_count;//== 采样数量
    adcount   = sample_count;
    g_dbg_last_elapsed_tick = rx_last_trigger_interval_tick;//== 采样窗口间隔

#if RX_ADC_HYS_ENABLE
    adc_ok = rx_adc_hysteresis_check(avg_adc);//== 应差判断
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

    g_dbg_last_adc_ok = adc_ok;//== 记录adc是否判断有效

        /*
     * 当前这 3 个窗口作为一个判断组：
     * - valid_count：本组有效窗口数量
     * - lost_count ：本组无效窗口数量
     */
    if (adc_ok != 0U)
    {
        if (rx_valid_count < RX_DECISION_GROUP_WINDOW_COUNT)
        {
            rx_valid_count++;
        }
    }
    else
    {
        if (rx_lost_window_count < RX_DECISION_GROUP_WINDOW_COUNT)
        {
            rx_lost_window_count++;
        }
    }

    /*
     * 每 3 个窗口统一判定一次。
     */
    rx_group_window_count++;

    if (rx_group_window_count >= RX_DECISION_GROUP_WINDOW_COUNT)
    {
        rx_group_window_count = 0U;

        /*
         * 无光状态下：
         * 必须 3 个窗口全部有效，才确认有光。
         */
        if (rx_light_state == 0U)
        {
            if (rx_valid_count >= RX_CONFIRM_COUNT)
            {
                rx_light_state = 1U;
                rx_output_light();
            }
        }
        /*
         * 有光状态下：
         * 本组只要出现一个无效窗口，就判定遮光。
         *
         * 若你希望必须“3 个都无效”才遮光，
         * 改成 rx_lost_window_count >= RX_RESYNC_WINDOW_COUNT。
         */
        else
        {
//            if (rx_lost_window_count != 0U)
//            {
//                rx_enter_block_state(UART_DBG_REASON_TIMEOUT);
//                return;
//            }
        }

        /*
         * 当前一组判断结束，清零，准备统计下一组。
         */
        rx_valid_count       = 0U;
        rx_lost_window_count = 0U;
    }
    
    if (rx_seen_once != 0U)//== 已经同步记录窗口次数，超过窗口次数强制同步
    {
        rx_resync_count_after_window();
    }
}


/*======================================================================
 * 十、串口调试函数
 *====================================================================*/

#if UART_ADC_DEBUG_ENABLE

/* 调试计数器安全自增，防止 uint16_t 溢出回 0。 */
static void uart_debug_inc16(volatile uint16_t *value)
{
    if (*value < 65535U)
    {
        (*value)++;
    }
}


/*
 * 请求串口打印。
 *
 * 当前 UART_ADC_DEBUG_ENABLE=0 时整段不会编译。
 * 如果打开调试，注意不要每 1ms 都大量 printf，
 * 否则主循环处理 pending 的速度会被串口拖慢。
 */
static void uart_debug_request(uint8_t reason, uint8_t force)
{
    (void)force;

    /*
     * 只允许 ADC 窗口处理完成后的事件触发打印。
     * 上电、同步、重同步、窗口积压等状态事件不打印，避免刷屏堵主循环。
     */
    if ((reason != UART_DBG_REASON_VALID_WINDOW) &&
        (reason != UART_DBG_REASON_ADC_FAIL) &&
        (reason != UART_DBG_REASON_LIGHT_ON) &&
        (reason != UART_DBG_REASON_TIMEOUT))
    {
        return;
    }

    g_uart_debug_reason = reason;

    g_uart_debug_div_count++;

    if (g_uart_debug_div_count >= UART_ADC_DEBUG_EVENT_DIV)
    {
        g_uart_debug_div_count  = 0U;
        g_uart_debug_need_print = 1U;
    }
}


/*
 * 主循环里真正执行 printf。
 *
 * 这样避免在中断里打印。
 * 但如果打印过多，仍然会堵主循环。
 */
static void uart_adc_debug_process(void)
{
    if (g_uart_debug_need_print == 0U)
    {
        return;
    }

    g_uart_debug_need_print = 0U;

    /*
     * 只打印两个核心量，CSV 格式更短：
     * 第 1 个数：当前判断阈值 adc_set。
     * 第 2 个数：最近一次 ADC 扫描窗口积分平均值 adc_data。
     */
    printfS("%u,%u\r\n",
            adc_set,
            adc_data);
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

    /*
     * GTIMER1 作为强制重同步等待同步边沿的 2ms 一次性超时定时器。
     * 只在周期性强制重同步时启动，收到 P1_4 后立即关闭。
     */
    gtimer1_count_init(RX_SYNC_WAIT_TIMEOUT_TICK, 24 - 1);
    gtimer1_irq_init(GTIMER_IRQ_ENABLE, GTIMER1_UIE_IRQ, gtimer1_UECallBack);

    /* 上电先不启动定时器，等待第一次 P1_4 同步边沿。 */
    rx_timer_stop_clear();//== 清零并停止定时器
    rx_sync_timeout_timer_stop_clear();//== 停止并清零 GTIMER1 同步等待超时定时器

    /* ADC_CHANNEL_1：接收信号 ADC；ADC_CHANNEL_2：预留 ADJ 电位器。 */
    adc_clk_config(ADC_CLKSOURCE_SYSCLK, ADC_VREFSOURCE_AVDD33, 4, ADC_ENABLE);
    adc_sample_clk_config(ADC_SAMPCLK_4);
    adc_io_config(ADC_CHANNEL_1 | ADC_CHANNEL_2);
    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);

    rx_output_block();//== 默认遮光输出
    rx_sync_irq_enable();//== 打开同步中断

    while (1)
    {
        /*
         * P1_4 interrupt 设置同步 rx_sync_pending.
				 * 在中断后修改第一次迁移时间
         */
        if (rx_sync_pending != 0U)//== 同步成功设定第一次定时器溢出时间迁移
        {
            rx_sync_pending = 0U;

            if (rx_seen_once == 0U)
            {
                rx_start_periodic_window_from_sync();
            }
        }

 
         /*
         * 窗口计数后启动强制同步事件。
         */
        if (rx_resync_pending != 0U)
        {
            rx_force_resync_from_main();
        }
        /*
         * 强制重同步等待期的 2ms 超时后处理结果。
         * 放在同步处理后面：如果同步边沿和超时几乎同时发生，优先认为同步成功。
         */
        if (rx_sync_wait_timeout_pending != 0U)
        {
            rx_handle_sync_wait_timeout_from_main();
        }

        /*
         * 定时器中断只置位，不在中断里做 ADC 连续扫描判断输出。
         */
        if (rx_window_pending != 0U)
        {
            /*
             * 处理 ADC 窗口。
             * 这里是正常有光/遮光判断的主路径。
             */
            rx_window_pending = 0U;
            rx_process_adc_window();
        }

       
    }
}


/*======================================================================
 * 十二、GTIMER0 周期窗口触发回调
 *====================================================================*/

/*
 * GTIMER0 更新中断：周期 ADC 窗口触发。
 *
 * 注意：
 * 中断里不读 ADC，只置位 rx_window_pending。
 * 真正 ADC 扫描放在主循环，避免中断占用太久。
 *
 * 排查点：
 * 如果 rx_seen_once=0，这里会直接停掉 GTIMER0。
 * 所以强制重同步后不会再产生 ADC 窗口。
 */
void gtimer0_UECallBack(void)
{
    if (rx_seen_once == 0U)//== 未同步清除计数不工作
    {
        rx_first_interval_pending = 0U;
        REG_GTIM0_CR0 &= ~(1 << 0);
        REG_GTIM0_CNT0 = 0x00;
        REG_GTIM0_CNT1 = 0x00;
        return;
    }

    if (rx_first_interval_pending != 0U)//== 第一次窗口到来记录880us
    {
        rx_first_interval_pending = 0U;
        rx_last_trigger_interval_tick = rx_first_delay_cached_tick;//== 获取上一次间隔
    }
    else//== 后续窗口都是记录1000us
    {
        rx_last_trigger_interval_tick = RX_WINDOW_INTERVAL_TICK;
    }

    if (rx_window_pending != 0U)//== 窗口没结束就进中断报错
    {
        /*
         * 上一次窗口还没被主循环处理，下一次 GTIMER0 又来了。
         * 说明主循环太忙，可能被 ADC 阻塞或串口打印拖慢。
         */
    }
    else//== 第一次到来时窗口进行中主循环处理窗口
    {
        rx_window_pending = 1U;
    }
}


/*======================================================================
 * 十三、GTIMER1 同步等待超时回调
 *====================================================================*/

/*
 * GTIMER1 更新中断：强制重同步等待超时。
 *
 * 只在 rx_sync_wait_timer_running=1 时有效。
 * 中断里只置位 rx_sync_wait_timeout_pending，
 * 真正遮光处理放到主循环。
 */
void gtimer1_UECallBack(void)
{
    /*
     * GTIMER1 只作为一次性 2ms 看门狗。
     * 中断里不调用 gpio_irq_set()/rx_enter_block_state()，只停表并置位，
     * 真正遮光处理交给主循环，避免 C51 L15 多入口警告。
     */
    REG_GTIM1_CR0 &= ~(1 << 0);
    REG_GTIM1_CNT0 = 0x00;
    REG_GTIM1_CNT1 = 0x00;

    if (rx_sync_wait_timer_running != 0U)//== 运行标志
    {
        rx_sync_wait_timer_running   = 0U;
        rx_sync_wait_timeout_pending = 1U;//== 超时等待
    }
}


/*======================================================================
 * 十四、P1_4 比较器同步中断
 *====================================================================*/

/*
 * P1_4 GPIO 中断：同步边沿入口。
 *
 * 中断里只做三件事：
 * 1. 判断是否是 P1_4 中断。
 * 2. 清中断标志。
 * 3. 如果当前允许同步，则置位 rx_sync_pending。
 *
 * 排查点：
 * 遮光时如果 P1_4 仍然会触发，说明同步输入有毛刺/反射/比较器噪声。
 * 这会让系统重新进入 rx_start_periodic_window_from_sync()。
 */
void GPIO_IRQHandler(void) interrupt 0
{
    if (gpio_irq_get(P1_4))
    {
        gpio_irq_clr(P1_4);

        /* 已同步运行时不再用 P1_4 参与判断。 */
        if ((rx_seen_once != 0U) ||
            (rx_sync_pending != 0U) /*||(rx_sync_wait_timeout_pending != 0U)*/
            )
        {
            return;
        }

        rx_sync_pending = 1U;
    }
}


/*
 * gpio_irq_set() 要求传入的回调。
 * 当前项目实际不用这个二次回调，真正处理在 GPIO_IRQHandler()。
 */
void gpio_UECallBack(void)
{
    /* 不用二次回调。 */
}


/*======================================================================
 * 十五、GPIO 初始化
 *====================================================================*/

/*
 * GPIO 初始化。
 *
 * P1_0：示波器观察 ADC 窗口，窗口期间拉低。
 * P1_2/P1_3：输出脚，有光拉高，遮光拉低。
 * P1_4：同步输入中断，只用于捕获同步边沿。
 * P1_5：保留输入。
 */
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



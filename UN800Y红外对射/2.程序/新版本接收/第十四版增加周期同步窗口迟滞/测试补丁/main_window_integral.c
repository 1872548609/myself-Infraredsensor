/***********************************************************************
 * UM800Y 红外对射接收端 main.c
 *
 * 本版本逻辑：
 * 1. P1_4 只作为“首次同步边沿”。捕获到一次比较器脉冲后，认为已经对齐发射周期。
 * 2. 同步后关闭 P1_4 同步中断，启动 GTIMER0。
 * 3. GTIMER0 按 RX_WINDOW_INTERVAL_TICK 周期触发，默认 1000us，即每 1ms 打开一次 ADC 扫描窗口。
 * 4. ADC 扫描窗口宽度由 RX_ADC_WINDOW_WIDTH_TICK 设置，窗口内连续读取 ADC，做积分平均。
 * 5. 最终使用“窗口平均 ADC”进入原来的 ADC 迟滞判断。
 * 6. 连续 RX_CONFIRM_COUNT 个有效窗口确认有光；连续 RX_LOST_WINDOW_COUNT 个无效窗口判定遮光。
 * 7. 遮光后停止 GTIMER0，重新打开 P1_4 同步中断，等待下一次来光重新同步。
 *
 * 重要参数：
 * - RX_WINDOW_INTERVAL_TICK：窗口间隔周期，默认 1000us。
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
 * ADC 连续扫描窗口宽度。
 * 默认 40us。
 *
 * 发射有效脉宽约 25us 时：
 * - 窗口太小：容易因为相位偏差漏掉脉冲；
 * - 窗口太大：平均值会被窗口内的低电平拉低，需要适当降低 adc_set；
 * - 建议先用 30~60us 调试。
 */
#define RX_ADC_WINDOW_WIDTH_TICK      40U

/*
 * 连续多少个 ADC 窗口无效后判定遮光。
 * 默认 3 个窗口，即约 3ms 遮光确认，响应约 333Hz。
 * 如果想更快，可以改 2；如果现场抖动明显，可以改 4 或 5。
 */
#define RX_LOST_WINDOW_COUNT          3U

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

/* ADC 迟滞状态：0 = ADC 当前认为无效，1 = ADC 当前认为有效 */
static volatile uint8_t rx_adc_valid_state   = 0;


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
static void rx_timer_stop_clear(void);
static void rx_timer_restart(void);
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
    rx_valid_count       = 0U;
    rx_lost_window_count = 0U;
    rx_adc_valid_state   = 0U;
    g_dbg_last_adc_ok    = 0U;

    /* 同步完成后，P1_4 不再反复进中断，后续由 GTIMER0 周期窗口采样。 */
    rx_sync_irq_disable();

#if RX_FIRST_WINDOW_IMMEDIATE
    rx_window_pending = 1U;
#endif

    rx_timer_restart();

    uart_debug_request(UART_DBG_REASON_SYNC, 1U);
}


static void rx_enter_block_state(uint8_t debug_reason)
{
    rx_valid_count       = 0U;
    rx_lost_window_count = 0U;
    rx_seen_once         = 0U;
    rx_window_pending    = 0U;
    rx_adc_valid_state   = 0U;
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
    g_dbg_last_elapsed_tick = RX_WINDOW_INTERVAL_TICK;

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
     * EL  ：窗口间隔，默认 1000us。
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
     * RX_WINDOW_INTERVAL_TICK = 1000 表示每 1ms 产生一次窗口触发。
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
         * 定时器中断只置位，不在中断里做 ADC 连续扫描。
         * 这样可以避免 40us 级窗口扫描长时间占用中断。
         */
        if (rx_window_pending != 0U)
        {
            rx_window_pending = 0U;
            rx_process_adc_window();
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
        rx_timer_stop_clear();
        return;
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
        if (rx_seen_once != 0U)
        {
            return;
        }

        rx_start_periodic_window_from_sync();
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

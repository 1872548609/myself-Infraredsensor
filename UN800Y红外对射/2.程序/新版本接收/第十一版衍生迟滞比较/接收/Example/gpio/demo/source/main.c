/***********************************************************************
 * UM800Y 红外对射接收端
 *
 * 功能：
 * 1. P1_4 捕获比较器输出脉冲。
 * 2. 发射周期 1ms，占空比 0.025，有效脉宽约 25us。
 * 3. 软件过滤 600us 内的比较器毛刺。
 * 4. ADC 增加迟滞判断：
 *      ADC >= adc_set               ：确认 ADC 有效
 *      ADC <= adc_set - RX_ADC_HYS  ：确认 ADC 无效
 *      中间区域保持上一次 ADC 判断状态
 * 5. 连续 2 个有效脉冲确认有光，约 2ms 响应。
 * 6. 4.2ms 无有效脉冲判定遮光，约 238Hz 响应。
 ***********************************************************************/

#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"


/*
 * =========================
 * 接收参数
 * =========================
 *
 * 假设系统主频 24MHz，GTIMER0 使用 24 分频：
 *
 * 定时器频率 = 24MHz / 24 = 1MHz
 * 1 tick ≈ 1us
 *
 * 发射周期 = 1ms
 * 有效脉宽 = 25us
 */

#define RX_CONFIRM_COUNT          2U

#define RX_MIN_PERIOD_TICK        600U     /* 600us  * 1 = 600 tick */
#define RX_MAX_PERIOD_TICK        1600U    /* 1600us * 1 = 1600 tick */
#define RX_LOST_TIMEOUT_TICK      2100U    /* 4200us * 1 = 4200 tick */


/*
 * =========================
 * ADC 迟滞参数
 * =========================
 *
 * adc_set 是 ADC 有效阈值。
 *
 * 举例：
 *      adc_set = 200
 *      RX_ADC_HYS_VALUE = 60
 *
 * 则：
 *      ADC >= 200 ：ADC 判断为有效
 *      ADC <= 140 ：ADC 判断为无效
 *      140 ~ 200 ：保持上一次状态
 *
 * 如果输出仍然抖动，可以把 60 改成 80 或 120。
 * 如果弱光容易不触发，可以把 60 改成 30 或 40。
 */

#define RX_ADC_HYS_ENABLE         1U
#define RX_ADC_HYS_VALUE          60U

#ifndef ADC_INVALID
#define ADC_INVALID               4095U
#endif

#define RX_ADC_MAX_VALID          (ADC_INVALID - 2U)


volatile uint16_t adc_data  = 0;
volatile uint16_t adc_data1 = 0;

/*
 * 原代码里的阈值。
 * 现在作为 ADC 迟滞的上阈值使用。
 *
 * 如果后续要接 ADJ 电位器，可以在主循环里读取 ADC_CHANNEL_2 更新 adc_set。
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
static volatile uint8_t rx_seen_once   = 0;     /* 是否已经捕获过有效同步脉冲 */

/*
 * ADC 迟滞状态
 *
 * 0 = ADC 当前认为无效
 * 1 = ADC 当前认为有效
 */
static volatile uint8_t rx_adc_valid_state = 0;


void gtimer0_UECallBack(void);
void gtimer1_UECallBack(void);
void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);


/*
 * 读取 GTIMER0 当前计数值。
 */
static uint16_t rx_timer_get_count(void)
{
    uint16_t cnt;

    cnt  = REG_GTIM0_CNT0;
    cnt |= ((uint16_t)REG_GTIM0_CNT1 << 8);

    return cnt;
}


/*
 * 停止并清零 GTIMER0。
 */
static void rx_timer_stop_clear(void)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;
}


/*
 * 从 0 重新开始 GTIMER0。
 */
static void rx_timer_restart(void)
{
    REG_GTIM0_CR0 &= ~(1 << 0);

    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    REG_GTIM0_CR0 |= (1 << 0);
}


/*
 * 输出：有光状态
 *
 * 保持你原来的输出逻辑：
 * P1_2 = HIGH
 * P1_3 = HIGH
 * P1_0 = LOW
 */
static void rx_output_light(void)
{
    gpio_io_set(P1_2, GPIO_HIGH);
    gpio_io_set(P1_3, GPIO_HIGH);
    gpio_io_set(P1_0, GPIO_LOW);
}


/*
 * 输出：遮光 / 无光状态
 *
 * 保持你原来的输出逻辑：
 * P1_2 = LOW
 * P1_3 = LOW
 * P1_0 = HIGH
 */
static void rx_output_block(void)
{
    gpio_io_set(P1_2, GPIO_LOW);
    gpio_io_set(P1_3, GPIO_LOW);
    gpio_io_set(P1_0, GPIO_HIGH);
}


/*
 * 单次读取 ADC。
 *
 * 注意：
 * 这里只在 P1_4 通过 1ms 周期过滤之后才读取 ADC。
 * 也就是说，比较器毛刺不会频繁触发 ADC 读取。
 */
static uint16_t rx_adc_read_ch1_once(void)
{
    uint16_t value;

    adc_convert_start(ADC_CHANNEL_1);

    while ((ADCGCR1 & 0x04) != 0);
    while (!(ADCCSTAT & 0x01));

    ADCCSTAT = 0x1;

    value = adc_get_value();

    return value;
}


/*
 * ADC 迟滞判断。
 *
 * 返回值：
 *      0 = ADC 幅度不满足
 *      1 = ADC 幅度满足
 *
 * 判断逻辑：
 *
 * rx_adc_valid_state == 0 时：
 *      必须 adc >= adc_set，才切换为有效。
 *
 * rx_adc_valid_state == 1 时：
 *      只有 adc <= adc_set - RX_ADC_HYS_VALUE，才切换为无效。
 *
 * 中间区域保持原状态，防止 ADC 在阈值附近来回抖。
 */
static uint8_t rx_adc_hysteresis_check(uint16_t adc)
{
    uint16_t adc_on_th;
    uint16_t adc_off_th;

    /*
     * 保护异常值。
     */
    if (adc >= RX_ADC_MAX_VALID)
    {
        rx_adc_valid_state = 0;
        return 0;
    }

    adc_on_th = adc_set;

    if (adc_set > RX_ADC_HYS_VALUE)
    {
        adc_off_th = adc_set - RX_ADC_HYS_VALUE;
    }
    else
    {
        adc_off_th = 0;
    }

    if (rx_adc_valid_state == 0)
    {
        /*
         * 当前 ADC 状态为无效。
         * 只有超过上阈值，才确认有效。
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
         * 只有跌破下阈值，才确认无效。
         */
        if (adc <= adc_off_th)
        {
            rx_adc_valid_state = 0;
        }
    }

    return rx_adc_valid_state;
}


void main(void)
{
    system_init();

    GPIO_Init();

    uart_init();

    /*
     * 4.2ms 丢光超时。
     * 响应速度约 1 / 4.2ms = 238Hz。
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
     * ADC_CHANNEL_1 用于接收信号 ADC。
     * ADC_CHANNEL_2 预留给 ADJ 电位器。
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

    while (1)
    {
        /*
         * 主循环暂时不处理接收逻辑。
         *
         * 接收逻辑由：
         *      P1_4 比较器中断
         *      ADC 迟滞判断
         *      GTIMER0 丢光超时
         * 完成。
         */
    }
}


/*
 * GTIMER0 超时：
 *
 * 超过 4.2ms 没有新的有效脉冲，判定遮光 / 无光。
 */
void gtimer0_UECallBack(void)
{
    rx_valid_count = 0;
    rx_seen_once   = 0;

    /*
     * 丢光后 ADC 迟滞状态也清零。
     *
     * 这样再次来光时，ADC 必须重新超过 adc_set，
     * 不会因为上一次的迟滞状态残留导致误触发。
     */
    rx_adc_valid_state = 0;

    if (rx_light_state)
    {
        rx_light_state = 0;
        rx_output_block();
    }
    else
    {
        rx_output_block();
    }

    /*
     * 无光后停止定时器。
     * 下一次有效 P1_4 脉冲来了再开启。
     */
    rx_timer_stop_clear();
}


/*
 * P1_4 比较器输出中断。
 *
 * 完整判断顺序：
 *
 * 1. P1_4 产生中断；
 * 2. 判断距离上一次有效脉冲的间隔；
 * 3. 小于 600us，认为是毛刺，直接丢弃；
 * 4. 读取 ADC；
 * 5. ADC 通过迟滞判断后，才算一次有效光脉冲；
 * 6. 连续 2 次有效光脉冲，确认有光；
 * 7. 每次有效光脉冲刷新 4.2ms 丢光定时器。
 */
void GPIO_IRQHandler(void) interrupt 0
{
    uint16_t elapsed;
    uint8_t adc_ok;

    if (gpio_irq_get(P1_4))
    {
        /*
         * 先清中断标志。
         */
        gpio_irq_clr(P1_4);

        elapsed = rx_timer_get_count();

        /*
         * 过滤比较器快速毛刺。
         *
         * 发射周期是 1ms。
         * 如果两次边沿间隔小于 600us，
         * 不可能是下一次真实发射脉冲。
         */
        if (rx_seen_once && (elapsed < RX_MIN_PERIOD_TICK))
        {
            return;
        }

#if RX_ADC_HYS_ENABLE
        /*
         * 读取 ADC 幅值。
         */
        adc_data = rx_adc_read_ch1_once();

        /*
         * ADC 迟滞判断。
         */
        adc_ok = rx_adc_hysteresis_check(adc_data);

        /*
         * ADC 不满足时，不刷新丢光定时器，不增加有效计数。
         */
        if (!adc_ok)
        {
            return;
        }
#else
        adc_ok = 1;
#endif

        /*
         * 到这里，说明：
         * 1. P1_4 有比较器脉冲；
         * 2. 不是 600us 内毛刺；
         * 3. ADC 幅值通过迟滞判断。
         *
         * 这才算一次真正有效光脉冲。
         */

        if (rx_seen_once && (elapsed > RX_MAX_PERIOD_TICK))
        {
            /*
             * 间隔超过 1.6ms。
             *
             * 未确认有光时，重新同步。
             * 已确认有光时，允许偶发漏一个周期，避免输出抖动。
             */
            if (rx_light_state)
            {
                rx_valid_count = RX_CONFIRM_COUNT;
            }
            else
            {
                rx_valid_count = 1;
            }
        }
        else
        {
            if (rx_valid_count < RX_CONFIRM_COUNT)
            {
                rx_valid_count++;
            }
        }

        rx_seen_once = 1;

        /*
         * 只有真正有效光脉冲才刷新丢光定时器。
         */
        rx_timer_restart();

        /*
         * 连续 2 个有效 1ms 周期确认有光。
         * 响应时间约 2ms。
         */
        if ((!rx_light_state) && (rx_valid_count >= RX_CONFIRM_COUNT))
        {
            rx_light_state = 1;
            rx_output_light();
        }
    }
}


void gpio_UECallBack(void)
{
    /*
     * 不用二次回调。
     */
}


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
     * P1_4 接比较器输出 ITIN。
     */
    gpio_init(P1_4);
    gpio_dir_set(P1_4, GPIO_DIR_IN);
    gpio_dr_set(P1_4, GPIO_SR_HIGH);
    gpio_in_enable(P1_4, IN_ENABLE);
    gpio_irq_set(P1_4, GPIO_IRQ_ENABLE, gpio_UECallBack);

    /*
     * 保留你原来的 P1_4 特殊配置。
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
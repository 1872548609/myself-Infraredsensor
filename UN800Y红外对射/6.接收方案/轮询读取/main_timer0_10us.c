/***********************************************************************
 * 文件名称：main.c
 * 工程用途：工业级红外对射传感器接收端
 * 芯片平台：UM800Y / UM8004
 * 主频配置：24MHz
 *
 * 工作方式：
 *  1. 不使用 P1.4。
 *     因为当前这版接收板原理图里，P1.4 没有作为有效接收输入使用。
 *
 *  2. 不使用任何中断。
 *     不使用 GPIO 外部中断；
 *     不使用 Timer0 中断；
 *     不使用 UART 接收中断。
 *
 *  3. ADC 连续扫描接收信号。
 *     发射端输出的是一个窄脉冲，不是持续电平。
 *     所以接收端不能 1ms 采一次，否则可能刚好采不到脉冲。
 *     正确方式是在 while(1) 中一直读取 ADC，捕捉脉冲峰值。
 *
 *  4. Timer0 只做 10us 软件时间基准。
 *     Timer0 不负责触发 ADC；
 *     Timer0 不进中断；
 *     只在主循环中轮询 UIF 标志，用于判断多久没收到有效脉冲。
 *
 *  5. 电位器 ADJ 低速更新阈值。
 *     P2.0 / ADJ 不需要高速读取；
 *     这里每 50ms 更新一次阈值，并做 IIR 平滑，避免阈值抖动。
 *
 * 硬件引脚对应：
 *  P1.5 / U3-1  -> LM358 输出，接收红外脉冲信号，ADC 输入
 *  P2.0 / ADJ   -> 电位器阈值调节，ADC 输入
 *  P1.0         -> NO 常开输出
 *  P1.2         -> NC 常闭输出
 *  P1.3         -> 红色指示灯
 *  P1.4         -> 当前版本不用
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
 *
 * 按旧工程习惯：
 *  ADC_CHANNEL_1 对应接收信号，也就是 P1.5 / U3-1。
 *
 * 如果你烧录后发现：
 *  1. 遮挡和无遮挡 ADC 没变化；
 *  2. 反而转动电位器会影响 g_adc_signal；
 * 那说明 ADC 通道映射和库定义不一致。
 * 这时只需要交换 IR_SIGNAL_ADC_CHANNEL 和 IR_ADJ_ADC_CHANNEL。
 */
#define IR_SIGNAL_ADC_CHANNEL           ADC_CHANNEL_1

/*
 * 电位器 ADJ ADC 通道。
 *
 * 原理图说明 ADJ 已经由 P0.2 移动到 P2.0，
 * 通过改变电位器电压来调整接收阈值，进而调节对射距离。
 */
#define IR_ADJ_ADC_CHANNEL              ADC_CHANNEL_2


/*======================================================================
 * 二、GPIO 引脚映射
 *====================================================================*/

/*
 * NO 常开输出。
 *
 * 收到光时，如果是亮通模式，NO 输出动作。
 */
#define IR_OUT_NO_PIN                   P1_0

/*
 * NC 常闭输出。
 *
 * NC 和 NO 做互补输出。
 * NO 动作时，NC 不动作；
 * NO 不动作时，NC 动作。
 */
#define IR_OUT_NC_PIN                   P1_2

/*
 * 红色指示灯。
 *
 * 这里默认 LED 显示“收到光”的状态。
 * 即：
 *  收到光 -> LED 亮
 *  遮光   -> LED 灭
 *
 * 如果你的板子 LED 是低电平点亮，只需要修改 LED_ACTIVE_LEVEL。
 */
#define IR_LED_PIN                      P1_3


/*======================================================================
 * 三、输出极性配置
 *====================================================================*/

/*
 * NO/NC 输出有效电平。
 *
 * 当前默认：
 *  GPIO_HIGH 表示输出动作；
 *  GPIO_LOW  表示输出不动作。
 *
 * 如果实测三极管输出极性相反，把下面两个宏对调即可。
 */
#define OUT_ACTIVE_LEVEL                GPIO_HIGH
#define OUT_INACTIVE_LEVEL              GPIO_LOW

/*
 * LED 有效电平。
 *
 * 如果 LED 是高电平点亮，保持 GPIO_HIGH；
 * 如果 LED 是低电平点亮，改成 GPIO_LOW。
 */
#define LED_ACTIVE_LEVEL                GPIO_HIGH
#define LED_INACTIVE_LEVEL              GPIO_LOW

/*
 * 亮通 / 暗通模式选择。
 *
 * SENSOR_DARK_ON = 0：
 *  亮通模式。
 *  收到光时输出动作；
 *  遮光时输出释放。
 *
 * SENSOR_DARK_ON = 1：
 *  暗通模式。
 *  收到光时输出释放；
 *  遮光时输出动作。
 *
 * 工业对射常见两种逻辑都有，这里用宏统一切换。
 */
#define SENSOR_DARK_ON                  0


/*======================================================================
 * 四、接收脉冲极性配置
 *====================================================================*/

/*
 * 接收信号有效方向。
 *
 * RX_SIGNAL_ACTIVE_HIGH = 1：
 *  表示 U3-1 输出高脉冲有效；
 *  ADC 数值变大代表收到红外脉冲。
 *
 * RX_SIGNAL_ACTIVE_HIGH = 0：
 *  表示 U3-1 输出低脉冲有效；
 *  ADC 数值变小代表收到红外脉冲。
 *
 * 当前按一般 LM358 放大后输出高脉冲有效处理。
 * 如果示波器看到遮光时高、收到光时出现低脉冲，则改成 0。
 */
#define RX_SIGNAL_ACTIVE_HIGH           1


/*======================================================================
 * 五、Timer0 软件时间基准
 *====================================================================*/

/*
 * Timer0 配置为 10us 溢出一次。
 *
 * 计算方式：
 *  主频 24MHz；
 *  TIMER0_PRESCALER = 24 - 1；
 *  Timer 计数频率 = 24MHz / 24 = 1MHz；
 *  1 个计数周期 = 1us；
 *  自动重装值 10，约等于 10us 溢出一次。
 *
 * 注意：
 *  Timer0 这里只提供时间基准。
 *  ADC 不等 Timer0；
 *  ADC 在 while(1) 里一直扫。
 */
#define TIMER0_TICK_US                  10U
#define TIMER0_10US_RELOAD              10U
#define TIMER0_PRESCALER                (24U - 1U)

/*
 * 时间换算宏。
 *
 * 后续所有“超时 / 周期”都先写成 ms 或 us，
 * 再统一换算成 Timer0 tick。
 *
 * 这样以后如果 Timer0 从 10us 改成 20us / 50us，
 * 只需要改 TIMER0_TICK_US 和 TIMER0_xxUS_RELOAD，
 * 不需要到处手动改计数值，避免时间错误。
 */
#define TIMER0_US_TO_TICKS(us)          ((uint16_t)(((uint32_t)(us) + (TIMER0_TICK_US - 1U)) / TIMER0_TICK_US))
#define TIMER0_MS_TO_TICKS(ms)          TIMER0_US_TO_TICKS((uint32_t)(ms) * 1000UL)

/*
 * 有些库文件里已经定义了 GTIMER0_UIF。
 * 如果没有定义，这里补一个默认值。
 * UIF 是 Timer0 溢出标志位。
 */
#ifndef GTIMER0_UIF
#define GTIMER0_UIF                     0x01
#endif


/*======================================================================
 * 六、阈值和检测参数
 *====================================================================*/

/*
 * 电位器 ADJ 映射出来的阈值范围。
 *
 * ADJ 的 ADC 范围理论是 0 ~ 4095。
 * 这里把它映射成 THRESHOLD_MIN_ADC ~ THRESHOLD_MAX_ADC。
 *
 * 如果距离不够：
 *  说明要求的信号太高，阈值太高；
 *  可以降低 THRESHOLD_MIN_ADC 和 THRESHOLD_MAX_ADC。
 *
 * 如果误触发多：
 *  说明阈值太低；
 *  可以提高 THRESHOLD_MIN_ADC 和 THRESHOLD_MAX_ADC。
 */
#define THRESHOLD_MIN_ADC               800U    //650U
#define THRESHOLD_MAX_ADC               3200U   //2800U
#define THRESHOLD_DEFAULT_ADC           1300U

/*
 * 脉冲进入阈值迟滞。
 *
 * 高脉冲有效时：
 *  ADC >= g_threshold + RX_ON_MARGIN_ADC
 *  才认为进入有效脉冲。
 *
 * 这样可以避免 ADC 在阈值附近轻微波动时误触发。
 */
#define RX_ON_MARGIN_ADC                100U

/*
 * 脉冲离开阈值迟滞。
 *
 * 高脉冲有效时：
 *  ADC <= g_threshold - RX_OFF_MARGIN_ADC
 *  才认为当前脉冲已经结束。
 *
 * 这样可以保证一个宽脉冲只计一次，不会在一个脉冲内部重复计数。
 */
#define RX_OFF_MARGIN_ADC               100U

/*
 * 连续有效脉冲确认次数。
 *
 * 捕捉到 1 个脉冲，不一定马上认为有光；
 * 连续捕捉到 RX_PULSE_CONFIRM_COUNT 个有效脉冲后，才确认有光。
 *
 * 值越大：
 *  抗干扰越强；
 *  响应越慢。
 *
 * 值越小：
 *  响应越快；
 *  但容易被单个干扰脉冲误触发。
 */
#define RX_PULSE_CONFIRM_COUNT          2U

/*
 * 丢光超时时间。
 *
 * 单位：ms。
 *
 * 5ms / 10us = 500 tick。
 *
 * 如果 5ms 内没有捕捉到任何有效脉冲，就认为遮光。
 *
 * 如果你的发射频率较低，或者脉冲周期较长，可以把这个值加大：
 *  8  -> 8ms
 *  10 -> 10ms
 *  15 -> 15ms
 */
#define RX_LOST_TIMEOUT_MS              5U
#define RX_LOST_TIMEOUT_TICKS           TIMER0_MS_TO_TICKS(RX_LOST_TIMEOUT_MS)

/*
 * ADJ 阈值更新周期。
 *
 * 单位：ms。
 *
 * 50ms / 10us = 5000 tick。
 *
 * 为什么 ADJ 不一直扫？
 *  因为发射有效信号是窄脉冲；
 *  ADC 时间应该尽量让给接收信号；
 *  电位器变化很慢，50ms 更新一次足够。
 */
#define ADJ_UPDATE_PERIOD_MS            50U
#define ADJ_UPDATE_PERIOD_TICKS         TIMER0_MS_TO_TICKS(ADJ_UPDATE_PERIOD_MS)

/*
 * 阈值 IIR 滤波系数。
 *
 * ADJ_FILTER_SHIFT = 3 表示：
 *  新阈值 = 7/8 旧阈值 + 1/8 新采样值
 *
 * 这样电位器轻微抖动不会导致输出抖动。
 */
#define ADJ_FILTER_SHIFT                3U

/*
 * ADC 最大有效值。
 *
 * 12 位 ADC 理论最大 4095。
 */
#define ADC_MAX_VALUE                   4095U


/*======================================================================
 * 七、全局运行状态
 *====================================================================*/

/*
 * 10us 软件时间计数。
 *
 * 每次 Timer0 约 10us 溢出时加 1。
 * uint16_t 会自然回绕，后面用差值计算超时，回绕不影响短时间判断。
 */
static volatile uint16_t g_tick_10us = 0;

/*
 * 当前接收信号 ADC 值。
 *
 * 调试时可以在仿真器里观察这个变量：
 *  对准时应该能看到脉冲峰值；
 *  遮挡时应该接近背景值。
 */
static volatile uint16_t g_adc_signal = 0;

/*
 * 当前电位器 ADJ ADC 值。
 *
 * 调试时转动电位器，这个值应该变化。
 */
static volatile uint16_t g_adc_adj = 0;

/*
 * 当前实际使用的判断阈值。
 *
 * 它由 ADJ 映射并滤波得到。
 */
static volatile uint16_t g_threshold = THRESHOLD_DEFAULT_ADC;

/*
 * 阈值滤波内部变量。
 *
 * 这里用放大后的整数做 IIR，避免使用 float。
 */
static uint32_t g_threshold_filter = ((uint32_t)THRESHOLD_DEFAULT_ADC << ADJ_FILTER_SHIFT);

/*
 * 阈值是否已经初始化。
 *
 * 第一次读取 ADJ 时，直接同步阈值；
 * 后续再做 IIR 平滑。
 */
static uint8_t g_threshold_init = 0;

/*
 * 当前是否确认收到光。
 *
 * 0：未收到光 / 遮光
 * 1：收到光
 */
static uint8_t g_light_ok = 0;

/*
 * 当前输出动作状态。
 *
 * 这个变量主要用于调试观察。
 * 0：输出不动作
 * 1：输出动作
 */
static uint8_t g_output_state = 0;

/*
 * 当前是否处于一个有效脉冲内部。
 *
 * 0：当前不在脉冲内部，可以等待下一次进入阈值
 * 1：当前已经进入脉冲内部，要等信号离开阈值后才能重新计数
 *
 * 作用：
 *  防止一个宽脉冲被 while(1) 连续 ADC 扫描重复计很多次。
 */
static uint8_t g_in_pulse = 0;

/*
 * 连续捕捉到的有效脉冲数量。
 *
 * 达到 RX_PULSE_CONFIRM_COUNT 后，确认收到光。
 */
static uint8_t g_valid_pulse_count = 0;

/*
 * 最近一次捕捉到有效脉冲的时间。
 *
 * 单位：10us tick。
 * 用于判断多久没有收到脉冲。
 */
static uint16_t g_last_pulse_tick = 0;

/*
 * ADJ 更新计时。
 *
 * 单位：10us tick。
 */
static uint16_t g_adj_update_tick = 0;


/*======================================================================
 * 八、函数声明
 *====================================================================*/

void GPIO_Init(void);
void ADC_Init(void);

static void timer0_init_poll_10us(void);
static void timer0_poll_process(void);

static uint16_t adc_read_once(uint8_t ch);

static void signal_fast_scan_process(void);
static void valid_pulse_process(void);
static void lost_timeout_process(void);

static void threshold_update_process(void);
static uint16_t threshold_map_from_adj(uint16_t adj);

static uint8_t signal_enter_active(uint16_t adc_value);
static uint8_t signal_leave_active(uint16_t adc_value);

static void output_apply(uint8_t light_ok);


/*======================================================================
 * 九、主函数
 *====================================================================*/

void main(void)
{
    /*
     * 系统初始化。
     *
     * system_init() 内部会根据 config.h 的 FCLK 配置系统时钟。
     * 这里要求 config.h 中：
     *  #define FCLK 24000000
     */
    system_init();

    /*
     * 初始化 GPIO。
     *
     * 包括：
     *  P1.0 NO 输出
     *  P1.2 NC 输出
     *  P1.3 LED 输出
     *  P1.5 接收信号 ADC 脚
     */
    GPIO_Init();

    /*
     * 串口初始化。
     *
     * 注意：
     *  app.c 里的 uart_init() 要关闭 uart0_irq_init()。
     *  这版不使用 UART 中断。
     */
    uart_init();

    /*
     * ADC 初始化。
     *
     * 配置接收信号 ADC 通道和 ADJ 阈值 ADC 通道。
     */
    ADC_Init();

    /*
     * Timer0 初始化为 10us 轮询时间基准。
     *
     * 不打开 Timer0 中断。
     */
    timer0_init_poll_10us();

    /*
     * 上电后先读取一次 ADJ。
     *
     * 这样可以避免刚上电时使用默认阈值太久。
     */
    threshold_update_process();

    /*
     * 全局关闭中断。
     *
     * 这个是兜底处理。
     * 即使某些库函数不小心开了中断，也在这里关闭。
     */
    EA = 0;

    /*
     * 上电默认输出为遮光 / 未收到光状态。
     */
    output_apply(0);

    /*
     * 主循环。
     *
     * 重点：
     *  signal_fast_scan_process() 每一轮都执行。
     *  不等待 1ms；
     *  不等待 Timer；
     *  不做多次平均；
     *  目的就是尽量快地捕捉发射端窄脉冲。
     */
    while(1)
    {
        /*
         * 轮询 Timer0 溢出标志。
         *
         * 只更新时间基准和慢任务：
         *  1. 丢光超时判断；
         *  2. ADJ 阈值低速更新。
         */
        timer0_poll_process();

        /*
         * 连续快速扫描接收 ADC。
         *
         * 这是本程序最核心的部分。
         */
        signal_fast_scan_process();
    }
}


/*======================================================================
 * 十、GPIO 初始化
 *====================================================================*/

void GPIO_Init(void)
{
    /*
     * P1.0 -> NO 常开输出。
     *
     * 默认先输出不动作状态。
     */
    REG_P10_CFG = 0x00;
    gpio_init(IR_OUT_NO_PIN);
    gpio_dir_set(IR_OUT_NO_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NO_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);

    /*
     * P1.2 -> NC 常闭输出。
     *
     * 默认 NC 动作，和 NO 互补。
     */
    REG_P12_CFG = 0x00;
    gpio_init(IR_OUT_NC_PIN);
    gpio_dir_set(IR_OUT_NC_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_OUT_NC_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);

    /*
     * P1.3 -> 红色指示灯。
     *
     * 默认灭。
     */
    REG_P13_CFG = 0x00;
    gpio_init(IR_LED_PIN);
    gpio_dir_set(IR_LED_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_LED_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);

 
    /*
     * P1.5 -> U3-1 接收信号。
     *
     * 这里不需要配置成普通 GPIO 输入；
     * 后面的 adc_io_config() 会把它配置为 ADC 功能。
     *
     * 这里先把配置寄存器清一下，避免残留特殊功能。
     */
    REG_P15_CFG = 0x00;
}


/*======================================================================
 * 十一、ADC 初始化
 *====================================================================*/

void ADC_Init(void)
{
    /*
     * ADC 时钟配置。
     *
     * ADC_CLKSOURCE_SYSCLK：
     *  ADC 使用系统时钟。
     *
     * ADC_VREFSOURCE_AVDD33：
     *  ADC 参考电压使用 AVDD33。
     *
     * 4：
     *  ADC 分频参数，沿用旧工程配置。
     */
    adc_clk_config(ADC_CLKSOURCE_SYSCLK,
                   ADC_VREFSOURCE_AVDD33,
                   4,
                   ADC_ENABLE);

    /*
     * ADC 采样时间配置。
     *
     * ADC_SAMPCLK_4：
     *  采样时钟较短，有利于提高扫描速度。
     *
     * 如果现场 ADC 抖动很大，可以适当加大采样时间，
     * 但加大后会降低扫描速度。
     */
    adc_sample_clk_config(ADC_SAMPCLK_6);

    /*
     * 使能接收信号 ADC 通道和 ADJ ADC 通道。
     */
    adc_io_config(IR_SIGNAL_ADC_CHANNEL | IR_ADJ_ADC_CHANNEL);

    /*
     * 单次转换模式。
     *
     * 每次调用 adc_read_once() 时启动一次转换。
     */
    adc_scan_mode_config(ADC_MODE_SINGLE);

    /*
     * 打开 ADC 电源。
     */
    adc_power_config(ADC_ENABLE);

    /*
     * 使能 ADC 控制器。
     */
    adc_controller_config(ADC_ENABLE);
}


/*======================================================================
 * 十二、Timer0 初始化和轮询
 *====================================================================*/

static void timer0_init_poll_10us(void)
{
    /*
     * 配置 Timer0：
     *  自动重装值 TIMER0_10US_RELOAD；
     *  预分频 TIMER0_PRESCALER。
     */
    gtimer0_count_init(TIMER0_10US_RELOAD, TIMER0_PRESCALER);

    /*
     * 如果工程里定义了 Timer0 中断使能寄存器，
     * 这里主动清零，确保不启用 Timer0 中断。
     */
#ifdef REG_GTIM0_IER
    REG_GTIM0_IER = 0x00;
#endif

    /*
     * 清除 Timer0 状态标志。
     *
     * 旧驱动里常用 REG_GTIM0_SR = 0x07 清标志，
     * 这里沿用这个写法。
     */
    REG_GTIM0_SR = 0x07;

    /*
     * 清零计数器。
     */
    REG_GTIM0_CNT0 = 0x00;
    REG_GTIM0_CNT1 = 0x00;

    /*
     * 启动 Timer0。
     *
     * 注意：
     *  这里只启动计数，不开启中断。
     */
    gtimer0_start();
}

static void timer0_poll_process(void)
{
    /*
     * 轮询 Timer0 溢出标志。
     *
     * 如果 UIF 置位，说明过去了约 10us。
     */
    if(REG_GTIM0_SR & GTIMER0_UIF)
    {
        /*
         * 清除 UIF 标志。
         */
        REG_GTIM0_SR = GTIMER0_UIF;

        /*
         * 软件时间 +1。
         *
         * 单位：10us。
         */
        g_tick_10us++;

        /*
         * 检查是否长时间没有捕捉到有效脉冲。
         */
        lost_timeout_process();

        /*
         * ADJ 阈值低速更新计数。
         */
        g_adj_update_tick++;

        /*
         * 到达 ADJ 更新周期后，读取电位器并更新阈值。
         */
        if(g_adj_update_tick >= ADJ_UPDATE_PERIOD_TICKS)
        {
            g_adj_update_tick = 0;
            threshold_update_process();
        }
    }
}


/*======================================================================
 * 十三、ADC 单次读取
 *====================================================================*/

static uint16_t adc_read_once(uint8_t ch)
{
    uint16_t value;

    /*
     * 启动指定通道 ADC 转换。
     */
    adc_convert_start(ch);

    /*
     * 等待 ADC 启动/忙状态结束。
     *
     * 这里沿用旧代码的等待方式。
     */
    while((ADCGCR1 & 0x04) != 0);

    /*
     * 等待 ADC 转换完成。
     */
    while(!(ADCCSTAT & 0x01));

    /*
     * 清除 ADC 转换完成标志。
     */
    ADCCSTAT = 0x01;

    /*
     * 读取 ADC 转换值。
     */
    value = adc_get_value();

    /*
     * 限幅，防止异常值超过 12 位范围。
     */
    if(value > ADC_MAX_VALUE)
    {
        value = ADC_MAX_VALUE;
    }

    return value;
}


/*======================================================================
 * 十四、连续快速扫描接收信号
 *====================================================================*/

static void signal_fast_scan_process(void)
{
    uint16_t signal;

    /*
     * 每次主循环只读取一次接收信号 ADC。
     *
     * 这里不要做多次平均。
     * 原因：
     *  发射有效光是窄脉冲；
     *  多次平均会把脉冲峰值平均掉；
     *  也会拖慢扫描速度，导致漏检脉冲。
     */
    signal = adc_read_once(IR_SIGNAL_ADC_CHANNEL);

    /*
     * 保存到全局变量，方便调试观察。
     */
    g_adc_signal = signal;

    /*
     * 当前不在脉冲内部。
     *
     * 这时要判断是否进入有效脉冲。
     */
    if(g_in_pulse == 0)
    {
        /*
         * 如果信号越过进入阈值，认为捕捉到一个新脉冲。
         */
        if(signal_enter_active(signal))
        {
            /*
             * 标记当前已经进入脉冲内部。
             *
             * 在信号离开脉冲区之前，不再重复计数。
             */
            g_in_pulse = 1;
            
            gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);//== 测试用
            /*
             * 处理一次有效脉冲。
             */
            valid_pulse_process();
        }
    }
    else
    {
        /*
         * 当前已经在脉冲内部。
         *
         * 需要等待信号回落到离开阈值以下，
         * 才允许下一次脉冲重新计数。
         */
        if(signal_leave_active(signal))
        {
            g_in_pulse = 0;
            
            gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL); //== 测试用
        }
    }
}


/*======================================================================
 * 十五、有效脉冲处理
 *====================================================================*/

static void valid_pulse_process(void)
{
    /*
     * 记录最近一次收到有效脉冲的时间。
     *
     * 丢光判断就是靠这个时间做超时。
     */
    g_last_pulse_tick = g_tick_10us;

    /*
     * 连续有效脉冲计数累加。
     *
     * 限制最大 255，防止溢出。
     */
    if(g_valid_pulse_count < 255U)
    {
        g_valid_pulse_count++;
    }

    /*
     * 连续有效脉冲达到确认次数后，判定收到光。
     */
    if(g_valid_pulse_count >= RX_PULSE_CONFIRM_COUNT)
    {
        /*
         * 状态从未收到光变为收到光时，才刷新输出。
         */
        if(g_light_ok == 0)
        {
            g_light_ok = 1;
            output_apply(1);
        }
    }
}


/*======================================================================
 * 十六、丢光超时处理
 *====================================================================*/

static void lost_timeout_process(void)
{
    uint16_t diff;

    /*
     * 计算距离最近一次有效脉冲过去了多久。
     *
     * uint16_t 自然回绕不影响这种短时间差值判断。
     */
    diff = (uint16_t)(g_tick_10us - g_last_pulse_tick);

    /*
     * 如果超过丢光时间，还没有新的有效脉冲，
     * 就认为遮光 / 丢光。
     */
    if(diff >= RX_LOST_TIMEOUT_TICKS)
    {
        /*
         * 清除连续有效脉冲计数。
         */
        g_valid_pulse_count = 0;

        /*
         * 清除脉冲内部状态。
         *
         * 防止信号一直卡在某个异常电平导致不能重新进入判断。
         */
        g_in_pulse = 0;

        /*
         * 如果当前状态是收到光，则切换成丢光状态并刷新输出。
         */
        if(g_light_ok != 0)
        {
            g_light_ok = 0;
            output_apply(0);
        }
    }
}


/*======================================================================
 * 十七、ADJ 阈值更新
 *====================================================================*/
static void threshold_update_process(void)
{
    uint16_t adj;
    uint16_t new_threshold;

    /*
     * 读取 ADJ 电位器。
     *
     * 这里只读取一次，不做多次平均。
     * 原因：
     *  ADJ 更新函数虽然是低速运行，
     *  但也不应该长时间占用 ADC，
     *  否则可能影响接收脉冲捕捉。
     */
    adj = adc_read_once(IR_ADJ_ADC_CHANNEL);

    /*
     * 保存 ADJ 原始值，方便调试观察。
     */
    g_adc_adj = adj;

    /*
     * 把 ADJ 原始 ADC 值映射成实际接收阈值。
     */
    new_threshold = threshold_map_from_adj(adj);

    /*
     * 第一次初始化时，直接同步阈值。
     *
     * 不做 IIR，否则刚上电阈值会慢慢逼近，导致前几十毫秒判断不准。
     */
    if(g_threshold_init == 0)
    {
        g_threshold_init = 1;
        g_threshold_filter = ((uint32_t)new_threshold << ADJ_FILTER_SHIFT);
        g_threshold = new_threshold;
        return;
    }

    /*
     * IIR 平滑滤波。
     *
     * 等价于：
     *  g_threshold = 7/8 * old + 1/8 * new
     *
     * 目的：
     *  避免电位器轻微抖动导致阈值抖动；
     *  避免输出在临界点来回跳。
     */
    g_threshold_filter -= (g_threshold_filter >> ADJ_FILTER_SHIFT);
    g_threshold_filter += new_threshold;

    /*
     * 还原成实际阈值。
     */
    g_threshold = (uint16_t)(g_threshold_filter >> ADJ_FILTER_SHIFT);

    /*
     * 阈值限幅。
     */
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

    /*
     * ADJ 限幅到 12 位 ADC 范围。
     */
    if(adj > ADC_MAX_VALUE)
    {
        adj = ADC_MAX_VALUE;
    }

    /*
     * 阈值范围跨度。
     */
    span = (uint32_t)(THRESHOLD_MAX_ADC - THRESHOLD_MIN_ADC);

    /*
     * 线性映射：
     *
     *  adj = 0    -> threshold = THRESHOLD_MIN_ADC
     *  adj = 4095 -> threshold = THRESHOLD_MAX_ADC
     */
    threshold = (uint16_t)(THRESHOLD_MIN_ADC +
                ((uint32_t)adj * span) / ADC_MAX_VALUE);

    return threshold;
}


/*======================================================================
 * 十八、脉冲进入/离开判断
 *====================================================================*/
static uint8_t signal_enter_active(uint16_t adc_value)
{
    uint16_t th;

#if RX_SIGNAL_ACTIVE_HIGH

    /*
     * 高脉冲有效：
     *
     * 只有 ADC 高于：
     *  g_threshold + RX_ON_MARGIN_ADC
     * 才认为进入有效脉冲。
     */
    th = g_threshold + RX_ON_MARGIN_ADC;

    /*
     * 防止阈值超过 ADC 最大值。
     */
    if(th > ADC_MAX_VALUE)
    {
        th = ADC_MAX_VALUE;
    }

    if(adc_value >= th)
    {
        return 1;
    }

#else

    /*
     * 低脉冲有效：
     *
     * 只有 ADC 低于：
     *  g_threshold - RX_ON_MARGIN_ADC
     * 才认为进入有效脉冲。
     */
    if(g_threshold > RX_ON_MARGIN_ADC)
    {
        th = g_threshold - RX_ON_MARGIN_ADC;
    }
    else
    {
        th = 0;
    }

    if(adc_value <= th)
    {
        return 1;
    }

#endif

    return 0;
}

static uint8_t signal_leave_active(uint16_t adc_value)
{
    uint16_t th;

#if RX_SIGNAL_ACTIVE_HIGH

    /*
     * 高脉冲有效：
     *
     * 当 ADC 低于：
     *  g_threshold - RX_OFF_MARGIN_ADC
     * 认为脉冲已经结束。
     */
    if(g_threshold > RX_OFF_MARGIN_ADC)
    {
        th = g_threshold - RX_OFF_MARGIN_ADC;
    }
    else
    {
        th = 0;
    }

    if(adc_value <= th)
    {
        return 1;
    }

#else

    /*
     * 低脉冲有效：
     *
     * 当 ADC 高于：
     *  g_threshold + RX_OFF_MARGIN_ADC
     * 认为脉冲已经结束。
     */
    th = g_threshold + RX_OFF_MARGIN_ADC;

    if(th > ADC_MAX_VALUE)
    {
        th = ADC_MAX_VALUE;
    }

    if(adc_value >= th)
    {
        return 1;
    }

#endif

    return 0;
}


/*======================================================================
 * 十九、输出控制
 *====================================================================*/
static void output_apply(uint8_t light_ok)
{
    uint8_t output_active;

    /*
     * 根据亮通/暗通模式，决定输出是否动作。
     */
#if SENSOR_DARK_ON
    /*
     * 暗通：
     *  有光 -> 输出不动作
     *  遮光 -> 输出动作
     */
    output_active = light_ok ? 0 : 1;
#else
    /*
     * 亮通：
     *  有光 -> 输出动作
     *  遮光 -> 输出不动作
     */
    output_active = light_ok ? 1 : 0;
#endif

    /*
     * NO / NC 互补输出。
     */
    if(output_active)
    {
        /*
         * 输出动作：
         *  NO 动作；
         *  NC 释放。
         */
        gpio_io_set(IR_OUT_NO_PIN, OUT_ACTIVE_LEVEL);
        //gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
    }
    else
    {
        /*
         * 输出不动作：
         *  NO 释放；
         *  NC 动作。
         */
        gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);
        //gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
    }

    /*
     * LED 显示收到光状态。
     *
     * 注意：
     *  LED 不跟随亮通/暗通模式；
     *  LED 只表示光路是否通。
     */
    if(light_ok)
    {
        gpio_io_set(IR_LED_PIN, LED_ACTIVE_LEVEL);
    }
    else
    {
        gpio_io_set(IR_LED_PIN, LED_INACTIVE_LEVEL);
    }

    /*
     * 保存当前输出状态，方便调试观察。
     */
    g_output_state = output_active;
}



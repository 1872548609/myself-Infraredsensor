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
 *     Timer0 只用于 ADJ 阈值低速更新，不再参与遮光超时判断。
 *
 *  5. 遮光不再使用“定时器超时”。
 *     每次 ADC 快速扫描后，如果本次读取明确处于无效区，
 *     连续无效读取次数加 1；
 *     如果捕捉到有效脉冲，连续无效读取次数清 0。
 *     连续无效读取次数超过 RX_INVALID_SAMPLE_CONFIRM_COUNT 后，
 *     判定遮光。
 *
 *  6. 电位器 ADJ 低速更新阈值。
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
 * 连续无效 ADC 读取确认次数。
 *
 * 这版取消了“定时器遮光超时”，遮光判定改成：
 *  连续 RX_INVALID_SAMPLE_CONFIRM_COUNT 次 ADC 读取都处于无效区，
 *  才认为遮光。
 *
 * 注意：
 *  发射端是窄脉冲，两个有效脉冲之间本来就会读到很多次无效值。
 *  所以这个值不能太小。
 *  如果无遮挡时也容易误判遮光，就增大这个值。
 *  如果遮光响应太慢，就减小这个值。
 */
#define RX_INVALID_SAMPLE_CONFIRM_COUNT 100U

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


/*
 * 串口 ADC 调试开关。
 *
 * UART_ADC_DEBUG_ENABLE = 1：
 *  打开串口周期回传 ADC 调试信息。
 *
 * UART_ADC_DEBUG_ENABLE = 0：
 *  关闭串口回传，恢复最快 ADC 扫描速度。
 *
 * 注意：
 *  printfS() 是阻塞发送，会占用主循环时间；
 *  所以这里只做低频输出，不能每次 ADC 扫描都打印。
 */
#define UART_ADC_DEBUG_ENABLE           1U

/*
 * ADC 调试信息回传周期。
 *
 * 单位：ms。
 * 当前 100ms 回传一次，大约每秒 10 行。
 * 如果仍然影响接收响应，可以改成 200ms / 500ms。
 */
#define UART_ADC_DEBUG_PERIOD_MS        100U
#define UART_ADC_DEBUG_PERIOD_TICKS     TIMER0_MS_TO_TICKS(UART_ADC_DEBUG_PERIOD_MS)


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
 * 连续无效 ADC 读取次数。
 *
 * 只有当本次 ADC 采样已经明确离开有效脉冲区时才累加。
 * 一旦捕捉到新的有效脉冲，就立即清零。
 * 达到 RX_INVALID_SAMPLE_CONFIRM_COUNT 后，判定遮光。
 *
 * 用 uint16_t 是为了以后把确认次数调到 255 以上时不会溢出。
 */
static uint16_t g_invalid_sample_count = 0;

/*
 * ADJ 更新计时。
 *
 * 单位：10us tick。
 */
static uint16_t g_adj_update_tick = 0;


#if UART_ADC_DEBUG_ENABLE
/*
 * 串口 ADC 调试计时。
 *
 * 单位同 g_tick_10us，靠 Timer0 轮询产生节拍。
 */
static uint16_t g_uart_adc_debug_tick = 0;

/*
 * 串口 ADC 调试发送标志。
 *
 * Timer0 轮询只置位标志，不直接 printfS；
 * 真正发送放在主循环中处理，便于控制发送频率。
 */
static uint8_t g_uart_adc_debug_send_flag = 0;
#endif


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
static void invalid_sample_process(void);

static void threshold_update_process(void);
static uint16_t threshold_map_from_adj(uint16_t adj);

static uint8_t signal_enter_active(uint16_t adc_value);
static uint8_t signal_leave_active(uint16_t adc_value);

static void output_apply(uint8_t light_ok);

static void uart_adc_debug_process(void);



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
         *  1. Timer0 不再参与遮光判断；
         *  2. ADJ 阈值低速更新。
         */
        timer0_poll_process();

        /*
         * 连续快速扫描接收 ADC。
         *
         * 这是本程序最核心的部分。
         */
        signal_fast_scan_process();


        /*
         * 串口周期回传 ADC 调试信息。
         *
         * 注意：
         *  这里不是每次 ADC 都打印，避免严重拖慢窄脉冲捕捉。
         */
        uart_adc_debug_process();
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


#if UART_ADC_DEBUG_ENABLE
        /*
         * 串口 ADC 调试周期计数。
         *
         * 这里只置位发送标志，不在这里直接 printfS，
         * 避免把 Timer0 轮询处理变得太重。
         */
        g_uart_adc_debug_tick++;
        if(g_uart_adc_debug_tick >= UART_ADC_DEBUG_PERIOD_TICKS)
        {
            g_uart_adc_debug_tick = 0;
            g_uart_adc_debug_send_flag = 1;
        }
#endif
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
     * 本次采样达到“进入有效脉冲”的阈值。
     *
     * 只要读到有效电平，就说明光路仍然存在，
     * 所以必须立即清零连续无效读取次数。
     */
    if(signal_enter_active(signal))
    {
        g_invalid_sample_count = 0;

        /*
         * 只有从“脉冲外”进入“脉冲内”时，才计 1 个有效脉冲。
         *
         * 如果一个脉冲持续了多个 ADC 扫描周期，
         * 后续扫描虽然仍然有效，但不能重复计数，
         * 否则一个宽脉冲会被误认为很多个脉冲。
         */
        if(g_in_pulse == 0)
        {
            g_in_pulse = 1;
            valid_pulse_process();
        }

        return;
    }

    /*
     * 走到这里，说明本次 ADC 读取没有达到有效脉冲阈值。
     * 按本次需求，它就属于一次“无效读取”，需要累计。
     *
     * 注意：
     *  迟滞仍然保留给 g_in_pulse 使用。
     *  只有信号明确离开有效区后，才允许下一次有效脉冲重新计数；
     *  但遮光计数按“本次是否有效”来统计，避免信号卡在迟滞区时永远不遮光。
     */
    if((g_in_pulse != 0) && signal_leave_active(signal))
    {
        g_in_pulse = 0;
    }

    /*
     * 连续无效 ADC 读取计数。
     *
     * 两个有效脉冲之间会自然出现很多次无效读取，
     * 所以 RX_INVALID_SAMPLE_CONFIRM_COUNT 必须根据实际 ADC 扫描速度
     * 和发射周期调整，不能过小。
     */
    invalid_sample_process();
}


/*======================================================================
 * 十五、有效脉冲处理
 *====================================================================*/

static void valid_pulse_process(void)
{
    /*
     * 捕捉到有效脉冲后，连续无效读取次数必须清零。
     *
     * 这是本次取消定时器超时后的关键点：
     *  遮光只看“连续无效读取次数”；
     *  只要中途出现一次有效脉冲，就不能继续沿用之前的无效计数。
     */
    g_invalid_sample_count = 0;

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
         * 状态从遮光变为有光时，才刷新输出。
         * 如果原来已经是有光状态，则不重复写 GPIO。
         */
        if(g_light_ok == 0)
        {
            g_light_ok = 1;
            output_apply(1);
        }
    }
}

static void invalid_sample_process(void)
{
    /*
     * 连续无效 ADC 读取计数累加。
     *
     * 这里统计的是“ADC 采样次数”，不是毫秒数。
     * 因此它和 adc_read_once() 的耗时、ADJ 更新占用时间、主频有关。
     */
    if(g_invalid_sample_count < 65535U)
    {
        g_invalid_sample_count++;
    }

    /*
     * 连续无效次数没有达到确认值之前，不改变输出。
     *
     * 这样可以避免有效脉冲之间的正常空白区，
     * 或者偶发的一两次低电平读取，导致输出乱跳。
     */
    if(g_invalid_sample_count < RX_INVALID_SAMPLE_CONFIRM_COUNT)
    {
        return;
    }

    /*
     * 已经确认遮光：
     *  1. 清除有效脉冲确认计数；
     *  2. 清除脉冲内部状态；
     *  3. 如果当前还处于有光状态，则切换为遮光并刷新输出。
     */
    g_valid_pulse_count = 0;
    g_in_pulse = 0;

    if(g_light_ok != 0)
    {
        g_light_ok = 0;
        output_apply(0);
    }
}


/*======================================================================
 * 十六、遮光判断说明
 *====================================================================*/
/*
 * 本版本不再使用定时器遮光超时函数。
 *
 * 旧逻辑：
 *  Timer0 每 10us 计数，超过固定毫秒数没有收到脉冲，就认为遮光。
 *
 * 新逻辑：
 *  不再用时间判断遮光，而是用“连续无效 ADC 读取次数”判断遮光。
 *  也就是：
 *      有效脉冲出现 -> g_invalid_sample_count 清 0；
 *      明确无效读取 -> g_invalid_sample_count 加 1；
 *      连续无效超过 RX_INVALID_SAMPLE_CONFIRM_COUNT -> 遮光。
 *
 * 优点：
 *  去掉了遮光判定对 Timer0 超时时间的依赖。
 *
 * 注意：
 *  因为这是按 ADC 读取次数计数，不是按真实时间计数，
 *  所以 RX_INVALID_SAMPLE_CONFIRM_COUNT 需要结合现场发射周期和 ADC 扫描速度调整。
 */


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
 * 十九、串口 ADC 调试输出
 *====================================================================*/
static void uart_adc_debug_process(void)
{
#if UART_ADC_DEBUG_ENABLE
    /*
     * 没到发送周期，不发送。
     */
    if(g_uart_adc_debug_send_flag == 0)
    {
        return;
    }

    /*
     * 先清标志，避免 printfS 阻塞期间重复进入。
     */
    g_uart_adc_debug_send_flag = 0;

    /*
     * 回传当前关键 ADC 和判断状态。
     *
     * S   ：接收信号 ADC，也就是 IR_SIGNAL_ADC_CHANNEL 当前值；
     * A   ：ADJ 电位器 ADC，也就是 IR_ADJ_ADC_CHANNEL 最近一次值；
     * TH  ：当前实际判断阈值；
     * INV ：连续无效 ADC 读取次数；
     * VP  ：连续有效脉冲计数；
     * L   ：当前是否确认有光，1 有光，0 遮光；
     * P   ：当前是否在有效脉冲内部。
     *
     * 如果转动电位器时 S 变化、A 不变化，说明两个 ADC 通道可能接反；
     * 如果遮挡/无遮挡时 S 完全不变，优先检查 IR_SIGNAL_ADC_CHANNEL 映射。
     */
    printfS("S=%u,A=%u,TH=%u,INV=%u,VP=%u,L=%u,P=%u\r\n",
            g_adc_signal,
            g_adc_adj,
            g_threshold,
            g_invalid_sample_count,
            (uint16_t)g_valid_pulse_count,
            (uint16_t)g_light_ok,
            (uint16_t)g_in_pulse);
#endif
}


/*======================================================================
 * 二十、输出控制
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
        gpio_io_set(IR_OUT_NC_PIN, OUT_INACTIVE_LEVEL);
    }
    else
    {
        /*
         * 输出不动作：
         *  NO 释放；
         *  NC 动作。
         */
        gpio_io_set(IR_OUT_NO_PIN, OUT_INACTIVE_LEVEL);
        gpio_io_set(IR_OUT_NC_PIN, OUT_ACTIVE_LEVEL);
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



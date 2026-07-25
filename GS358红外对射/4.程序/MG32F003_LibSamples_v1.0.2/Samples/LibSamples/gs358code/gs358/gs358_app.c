/**
 * @file gs358_app.c
 * @brief GS358 红外对射接收板初版应用
 */

#include "platform.h"
#include "gs358_app.h"
#include "mg32_tim.h"
#include "mg32_iwdg.h"

/* =========================== 引脚定义 =========================== */

#define GS358_COMP_PORT              GPIOA
#define GS358_COMP_PIN               GPIO_Pin_7

#define GS358_OUTPUT_NC_PORT         GPIOA
#define GS358_OUTPUT_NC_PIN          GPIO_Pin_8

#define GS358_OUTPUT_NO_PORT         GPIOA
#define GS358_OUTPUT_NO_PIN          GPIO_Pin_9

#define GS358_RED_LED_PORT           GPIOA
#define GS358_RED_LED_PIN            GPIO_Pin_10

#define GS358_PWM_PORT               GPIOA
#define GS358_PWM_PIN                GPIO_Pin_11

/* ADC 通道在芯片上的物理引脚。 */
#define GS358_ADC_GPIOA_PINS         (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_12)
#define GS358_ADC_GPIOB_PINS         (GPIO_Pin_0 | GPIO_Pin_1)

/* ADC 任意通道扫描数量寄存器填写 N-1：5 路对应 4。 */
#define GS358_ADC_CHANNEL_NUM_FIELD  (GS358_ADC_CHANNEL_COUNT - 1U)

/* =========================== 丢光超时定时器 =========================== */

/*
 * TIM3同时承担：
 * 1. 相邻比较器下降沿的周期计时；
 * 2. 最后一次下降沿后的丢光超时判断。
 *
 * TIM3配置为1 MHz，因此CNT单位为us。
 */
#define GS358_TIMEOUT_TIMER         TIM3

/* =========================== 全局状态 =========================== */

volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT] = {0U};
volatile uint32_t g_gs358_adc_frame_count = 0U;
volatile uint32_t g_gs358_compare_edge_total = 0U;
volatile uint8_t  g_gs358_light_present = 0U;

volatile uint16_t g_gs358_last_period_us = 0U;
volatile uint32_t g_gs358_period_valid_total = 0U;
volatile uint32_t g_gs358_period_invalid_total = 0U;
volatile uint8_t  g_gs358_last_period_valid = 0U;

/*
 * 上一次迟到边沿超过一个目标周期后留下的相位余量。
 * 下一条边沿的原始计数需要加上该余量后再判断。
 */
static volatile uint16_t s_period_carry_us = 0U;

/* 本次运行期间累计的漏周期/迟到边沿次数。 */
volatile uint32_t g_gs358_period_miss_total = 0U;

/* 便于 Keil Watch 观察。 */
volatile uint16_t g_gs358_last_raw_period_us = 0U;
volatile uint16_t g_gs358_last_adjusted_period_us = 0U;
volatile uint16_t g_gs358_last_period_carry_us = 0U;

/*
 * 当前10周期判断窗口内的有效周期数和漏周期数。
 *
 * 注意：
 * g_gs358_period_miss_total 是系统运行期间的累计调试值；
 * s_window_miss_count 只统计当前判断窗口。
 */
static volatile uint8_t s_window_valid_count = 0U;
static volatile uint8_t s_window_miss_count = 0U;

/* 最近一个完整窗口的结果，方便Keil Watch观察。 */
volatile uint8_t g_gs358_last_window_valid_count = 0U;
volatile uint8_t g_gs358_last_window_miss_count = 0U;
volatile uint8_t g_gs358_last_window_light_present = 0U;

/* 完成的窗口总数。 */
volatile uint32_t g_gs358_window_complete_total = 0U;

/* =========================== 内部状态 =========================== */

static volatile uint8_t s_edge_confirm_count = 0U;
static volatile uint8_t s_period_reference_ready = 0U;
static volatile uint8_t s_adc_eoc_count = 0U;
static volatile GS358_LedMode s_led_mode = GS358_LED_BLOCKED_ON;



/* =========================== 内部函数声明 =========================== */

//static void GS358_SysTickInit(void);
static void GS358_GPIOInit(void);
static void GS358_EXTIInit(void);
static void GS358_ADCInit(void);

static void GS358_PeriodTimerInit(void);
static void GS358_PeriodTimerRestart(void);
static void GS358_PeriodTimerStop(void);
static void GS358_LightLostTimerInit(void);
static void GS358_LightLostTimerRestart(void);
static uint16_t GS358_AddPeriodCarrySaturated(uint16_t raw_us,
                                              uint16_t carry_us);
static uint16_t GS358_CalculateNextCarry(uint16_t adjusted_us);


static void GS358_ResetDetectionWindow(void);
static void GS358_CheckDetectionWindowComplete(void);

static uint8_t GS358_IsPeriodValid(uint16_t period_us);
static void GS358_ResetPeriodFilter(void);

static void GS358_SetLightState(uint8_t light_present);
static void GS358_ApplyOutputs(void);

static void GS358_WatchdogInit(void);
static void GS358_WatchdogFeed(void);

static void GS358_WriteLogicalOutput(GPIO_TypeDef *port,
                                     uint16_t pin,
                                     uint8_t logical_active,
                                     uint8_t active_high);


/* =========================== 初始化 =========================== */

void GS358_AppInit(void)
{
    /*
     * 不再调用旧示例中的 PLATFORM_Init()：
     * 旧函数会按开发板 LED 逻辑重新配置 PA10，不符合本接收板原理图。
     */
    //GS358_SysTickInit();    //== 配置1ms平台中断
    GS358_GPIOInit();       //== 配置全部io初始化


    /*
     * 先初始化 TIM3，再开启 PA7 外部中断。
     * 避免 PA7 提前产生下降沿时 TIM3 尚未配置完成。
     */
    GS358_PeriodTimerInit();
    GS358_LightLostTimerInit();

    GS358_EXTIInit();
    GS358_ADCInit();

    s_edge_confirm_count = 0U;
    s_period_reference_ready = 0U;


    //== 周期检测初始化变量
    g_gs358_light_present = 0U;
    g_gs358_last_period_us = 0U;
    g_gs358_period_valid_total = 0U;
    g_gs358_period_invalid_total = 0U;
    g_gs358_last_period_valid = 0U;
    GS358_ResetDetectionWindow();
    g_gs358_last_window_valid_count = 0U;
    g_gs358_last_window_miss_count = 0U;
    g_gs358_last_window_light_present = 0U;
    g_gs358_window_complete_total = 0U;

    /*
     * 上电默认按“遮光/无光”状态输出。
     * TIM3 此时不启动，收到第一个有效下降沿后才开始超时计时。
     */
    GS358_ApplyOutputs();
    
    
    /*
     * 所有硬件初始化完成后再启动独立看门狗。
     */
    GS358_WatchdogInit();
}

void GS358_AppProcess(void)
{
    /*
     * USER CODE BEGIN MAIN_LOOP
     *
     * 主循环预留位置。
     * 建议在 ADC 回调中只置标志，耗时算法放到这里处理。
     *
     * USER CODE END MAIN_LOOP
     */
     
     /*
     * 只有主循环能够持续正常运行时才喂狗。
     *
     * 不要把喂狗放到比较器、TIM3 或 ADC 中断中，
     * 否则主循环即使跑飞，只要中断仍然进入，
     * 看门狗就可能无法检测到故障。
     */
    //GS358_WatchdogFeed();
}
/* =========================== 独立看门狗 =========================== */

static void GS358_WatchdogInit(void)
{
#if (GS358_WATCHDOG_ENABLE != 0U)

    /*
     * 开启 IWDG_PR 和 IWDG_RLR 寄存器写权限。
     */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /*
     * IWDG 时钟源为约 40 kHz 的独立 LSI。
     *
     * 预分频设置为 32：
     * 每次计数约为 32 / 40000 = 0.8 ms。
     */
    IWDG->PR = IWDG_Prescaler_32;

    /*
     * 标称超时时间：
     *
     * (Reload + 1) × Prescaler / LSI
     *
     * (2499 + 1) × 32 / 40000
     * = 2 秒
     */
   IWDG->RLR = (uint16_t)GS358_WATCHDOG_RELOAD_VALUE;

    /*
     * 计数器溢出后直接产生系统复位，
     * 不采用仅产生中断的方式。
     */
    IWDG_OverflowConfig(IWDG_Overflow_Reset);

    /*
     * 启动独立看门狗。
     *
     * IWDG 启动后，正常运行期间不能通过普通软件关闭，
     * 只能通过系统复位重新初始化。
     */
    IWDG_Enable();

    /*
     * 启动后立即装载一次计数器，
     * 从完整超时时间开始计数。
     */
    IWDG_ReloadCounter();

#endif
}

static void GS358_WatchdogFeed(void)
{
#if (GS358_WATCHDOG_ENABLE != 0U)

    /*
     * 重新装载看门狗计数器。
     *
     * 本函数只在主循环 GS358_AppProcess() 中调用。
     */
    IWDG_ReloadCounter();

#endif
}

/* =========================== GPIO =========================== */

static void GS358_GPIOInit(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA |
                          RCC_AHBPERIPH_GPIOB,
                          ENABLE);

    /*
     * 先写入安全初值，再切换成输出模式：
     * PA8/PA9/PA10/PA11 上电均先保持低电平。
     */
    GPIO_ResetBits(GPIOA,
                   GS358_OUTPUT_NC_PIN |
                   GS358_OUTPUT_NO_PIN |
                   GS358_RED_LED_PIN |
                   GS358_PWM_PIN);

    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin =
        GS358_OUTPUT_NC_PIN |
        GS358_OUTPUT_NO_PIN |
        GS358_RED_LED_PIN |
        GS358_PWM_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_High;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);

    /* PA7 接 GSV331R 推挽比较器输出，使用浮空数字输入。 */
    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_COMP_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_High;
    gpio_init.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GS358_COMP_PORT, &gpio_init);

    /* ADC1_VIN2、VIN3、VIN5：PA3、PA12、PA2。 */
    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_ADC_GPIOA_PINS;
    gpio_init.GPIO_Speed = GPIO_Speed_Low;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio_init);

    /* ADC1_VIN0、VIN1：PB1、PB0。 */
    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_ADC_GPIOB_PINS;
    gpio_init.GPIO_Speed = GPIO_Speed_Low;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &gpio_init);

    /* PA11 PWM 尚未启用，保持低电平。 */
    GPIO_ResetBits(GS358_PWM_PORT, GS358_PWM_PIN);
}

static void GS358_WriteLogicalOutput(GPIO_TypeDef *port,
                                     uint16_t pin,
                                     uint8_t logical_active,
                                     uint8_t active_high)
{
    BitAction output_level;

    if (active_high != 0U)
    {
        output_level =
            (logical_active != 0U) ? Bit_SET : Bit_RESET;
    }
    else
    {
        output_level =
            (logical_active != 0U) ? Bit_RESET : Bit_SET;
    }

    GPIO_WriteBit(port, pin, output_level);
}

static void GS358_ApplyOutputs(void)
{
    uint8_t blocked;
    uint8_t no_active;
    uint8_t nc_active;
    uint8_t led_active;

    blocked =
        (g_gs358_light_present == 0U) ? 1U : 0U;

    /*
     * 输出语义：
     * 遮光/无光：NO 动作，NC 释放
     * 有光：     NO 释放，NC 动作
     *
     * 若最终整机定义相反，只需要交换下面两行，
     * 或调整 *_ACTIVE_HIGH 宏。
     */
    no_active = blocked;
    nc_active = (blocked == 0U) ? 1U : 0U;

    GS358_WriteLogicalOutput(GS358_OUTPUT_NO_PORT,
                             GS358_OUTPUT_NO_PIN,
                             no_active,
                             GS358_NO_OUTPUT_ACTIVE_HIGH);

    GS358_WriteLogicalOutput(GS358_OUTPUT_NC_PORT,
                             GS358_OUTPUT_NC_PIN,
                             nc_active,
                             GS358_NC_OUTPUT_ACTIVE_HIGH);

    if (s_led_mode == GS358_LED_LIGHT_ON)
    {
        led_active = g_gs358_light_present;
    }
    else
    {
        led_active = blocked;
    }

    GS358_WriteLogicalOutput(GS358_RED_LED_PORT,
                             GS358_RED_LED_PIN,
                             led_active,
                             GS358_RED_LED_ACTIVE_HIGH);
}

static void GS358_SetLightState(uint8_t light_present)
{
    uint8_t normalized;

    normalized = (light_present != 0U) ? 1U : 0U;

    if (g_gs358_light_present != normalized)
    {
        g_gs358_light_present = normalized;
        GS358_ApplyOutputs();
    }
}

void GS358_SetLedMode(GS358_LedMode mode)
{
    if ((mode == GS358_LED_BLOCKED_ON) ||
        (mode == GS358_LED_LIGHT_ON))
    {
        s_led_mode = mode;
        GS358_ApplyOutputs();
    }
}

GS358_LedMode GS358_GetLedMode(void)
{
    return s_led_mode;
}

/* =========================== 周期判断 =========================== */
static void GS358_ResetDetectionWindow(void)
{
    s_window_valid_count = 0U;
    s_window_miss_count = 0U;
}
static void GS358_CheckDetectionWindowComplete(void)
{
    uint16_t total_count;
    uint8_t light_present;

    total_count =
        (uint16_t)s_window_valid_count +
        (uint16_t)s_window_miss_count;

    if (total_count < GS358_DETECT_WINDOW_COUNT)
    {
        return;
    }

    /*
     * 正常情况下刚好等于10。
     *
     * 使用 >= 作为保护，避免后续修改逻辑后因计数超过10，
     * 导致窗口一直无法结束。
     */
    g_gs358_last_window_valid_count =
        s_window_valid_count;

    g_gs358_last_window_miss_count =
        s_window_miss_count;

    if (s_window_valid_count >
        GS358_DETECT_VALID_THRESHOLD)
    {
        light_present = 1U;
    }
    else
    {
        light_present = 0U;
    }

    g_gs358_last_window_light_present =
        light_present;

    g_gs358_window_complete_total++;

    /*
     * 只有完整收集10个结果后才更新输出。
     */
    GS358_SetLightState(light_present);

    /*
     * 当前窗口结束，马上开始下一窗口。
     *
     * 周期参考、carry和两个定时器不需要在这里清除，
     * 下一边沿继续沿用当前周期跟踪状态。
     */
    GS358_ResetDetectionWindow();
}
static uint8_t GS358_IsPeriodValid(uint16_t period_us)
{
    uint32_t period_min_us;
    uint32_t period_max_us;
    uint32_t measured_period_us;

    if (GS358_SIGNAL_PERIOD_US >
        GS358_SIGNAL_PERIOD_TOLERANCE_US)
    {
        period_min_us =
            GS358_SIGNAL_PERIOD_US -
            GS358_SIGNAL_PERIOD_TOLERANCE_US;
    }
    else
    {
        period_min_us = 0U;
    }

    period_max_us =
        GS358_SIGNAL_PERIOD_US +
        GS358_SIGNAL_PERIOD_TOLERANCE_US;

    measured_period_us = (uint32_t)period_us;

    if ((measured_period_us >= period_min_us) &&
        (measured_period_us <= period_max_us))
    {
        return 1U;
    }

    return 0U;
}

static void GS358_ResetPeriodFilter(void)
{
    s_period_reference_ready = 0U;
    s_edge_confirm_count = 0U;

    g_gs358_last_period_us = 0U;
    g_gs358_last_period_valid = 0U;
}

static uint16_t GS358_AddPeriodCarrySaturated(uint16_t raw_us,
                                              uint16_t carry_us)
{
    uint32_t sum;

    sum = (uint32_t)raw_us + (uint32_t)carry_us;
    if (sum > 0xFFFFUL)
    {
        sum = 0xFFFFUL;
    }

    return (uint16_t)sum;
}

/*
 * 迟到周期的补偿值：
 *
 * adjusted = 目标周期整数倍 + 相位余量
 *
 * 只把相位余量留给下一条边沿，避免 adjusted 已跨过多个周期时，
 * 将一个大于 75 us 的补偿永久带入后续判断。
 *
 * 例如：
 *   adjusted = 90 us  -> carry = 15 us
 *   adjusted = 165 us -> carry = 15 us
 */
static uint16_t GS358_CalculateNextCarry(uint16_t adjusted_us)
{
    uint16_t carry;

    if (GS358_SIGNAL_PERIOD_US == 0U)
    {
        return 0U;
    }

    carry = (uint16_t)(adjusted_us % GS358_SIGNAL_PERIOD_US);
    return carry;
}

/*
 * TIM1：只测量从上一个“周期基准边沿”到当前边沿的时间。
 * 无中断、自由向上计数。
 */
static void GS358_PeriodTimerInit(void)
{
    TIM_TimeBaseInitTypeDef timer_init;
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TIM1, ENABLE);
    TIM_DeInit(GS358_PERIOD_TIMER);

    timer_clock_hz = TIM_GetTIMxClock(GS358_PERIOD_TIMER);
    prescaler_div = timer_clock_hz / GS358_TIMER_TICK_HZ;

    if ((prescaler_div == 0U) ||
        ((timer_clock_hz % GS358_TIMER_TICK_HZ) != 0U))
    {
        while (1)
        {
            /* TIM1 无法精确分频到 1 MHz。 */
        }
    }

    TIM_TimeBaseStructInit(&timer_init);
    timer_init.TIM_Prescaler = (uint16_t)(prescaler_div - 1U);
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    timer_init.TIM_Period = 0xFFFFU;
    timer_init.TIM_ClockDivision = TIM_CKD_Div1;
    timer_init.TIM_RepetitionCounter = 0U;
    TIM_TimeBaseInit(GS358_PERIOD_TIMER, &timer_init);

    TIM_ClearFlag(GS358_PERIOD_TIMER, TIM_FLAG_Update);
    TIM_SetCounter(GS358_PERIOD_TIMER, 0U);
    TIM_Cmd(GS358_PERIOD_TIMER, DISABLE);
}

static void GS358_PeriodTimerRestart(void)
{
    TIM_Cmd(GS358_PERIOD_TIMER, DISABLE);
    TIM_ClearFlag(GS358_PERIOD_TIMER, TIM_FLAG_Update);
    TIM_SetCounter(GS358_PERIOD_TIMER, 0U);
    TIM_Cmd(GS358_PERIOD_TIMER, ENABLE);
}

static void GS358_PeriodTimerStop(void)
{
    TIM_Cmd(GS358_PERIOD_TIMER, DISABLE);
    TIM_ClearFlag(GS358_PERIOD_TIMER, TIM_FLAG_Update);
    TIM_SetCounter(GS358_PERIOD_TIMER, 0U);
}

/* TIM3：只负责最后一个有效边沿之后 1500 us 无有效边沿超时。 */
static void GS358_LightLostTimerInit(void)
{
    TIM_TimeBaseInitTypeDef timer_init;
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TIM3, ENABLE);
    TIM_DeInit(GS358_TIMEOUT_TIMER);

    timer_clock_hz = TIM_GetTIMxClock(GS358_TIMEOUT_TIMER);
    prescaler_div = timer_clock_hz / GS358_TIMER_TICK_HZ;

    if ((prescaler_div == 0U) ||
        ((timer_clock_hz % GS358_TIMER_TICK_HZ) != 0U) ||
        (GS358_LIGHT_LOST_TIMEOUT_US == 0U) ||
        (GS358_LIGHT_LOST_TIMEOUT_US > 65536UL))
    {
        while (1)
        {
            /* TIM3 参数错误。 */
        }
    }

    TIM_TimeBaseStructInit(&timer_init);
    timer_init.TIM_Prescaler = (uint16_t)(prescaler_div - 1U);
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    timer_init.TIM_Period = (uint16_t)(GS358_LIGHT_LOST_TIMEOUT_US - 1U);
    timer_init.TIM_ClockDivision = TIM_CKD_Div1;
    timer_init.TIM_RepetitionCounter = 0U;
    TIM_TimeBaseInit(GS358_TIMEOUT_TIMER, &timer_init);

    TIM_SelectOnePulseMode(GS358_TIMEOUT_TIMER, TIM_OPMode_Single);
    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER, TIM_IT_Update);
    TIM_ITConfig(GS358_TIMEOUT_TIMER, TIM_IT_Update, ENABLE);

    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 1U);
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM_SetCounter(GS358_TIMEOUT_TIMER, 0U);
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);
}

static void GS358_LightLostTimerRestart(void)
{
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);
    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER, TIM_IT_Update);
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    TIM_SetCounter(GS358_TIMEOUT_TIMER, 0U);
    TIM_Cmd(GS358_TIMEOUT_TIMER, ENABLE);
}


/* =========================== PA7 外部中断 =========================== */

static void GS358_EXTIInit(void)
{
    EXTI_InitTypeDef exti_init;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SYSCFG,
                           ENABLE);

    /* 将 EXTI7 映射到 PA7。 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,
                          EXTI_PinSource7);

    EXTI_ClearITPendingBit(EXTI_Line7);

    EXTI_StructInit(&exti_init);
    exti_init.EXTI_Line = EXTI_Line7;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init);

    NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 0U);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

 
void GS358_ComparatorFallingIRQHandler(void)
{
    uint16_t raw_period_us;
    uint16_t adjusted_period_us;
    uint16_t period_min_us;
    uint16_t period_max_us;
    uint8_t period_valid;

    g_gs358_compare_edge_total++;

    period_min_us =
        (GS358_SIGNAL_PERIOD_US > GS358_SIGNAL_PERIOD_TOLERANCE_US) ?
        (uint16_t)(GS358_SIGNAL_PERIOD_US -
                   GS358_SIGNAL_PERIOD_TOLERANCE_US) : 0U;

    period_max_us =
        (uint16_t)(GS358_SIGNAL_PERIOD_US +
                   GS358_SIGNAL_PERIOD_TOLERANCE_US);

    /* 第一条边沿只建立时间基准。 */
    if (s_period_reference_ready == 0U)
    {
        s_period_reference_ready = 1U;
        s_period_carry_us = 0U;

        GS358_ResetDetectionWindow();

        g_gs358_last_period_us = 0U;
        g_gs358_last_raw_period_us = 0U;
        g_gs358_last_adjusted_period_us = 0U;
        g_gs358_last_period_carry_us = 0U;
        g_gs358_last_period_valid = 0U;

        GS358_PeriodTimerRestart();
        GS358_LightLostTimerRestart();
        return;
    }

    raw_period_us = (uint16_t)TIM_GetCounter(GS358_PERIOD_TIMER);

    adjusted_period_us = GS358_AddPeriodCarrySaturated(raw_period_us,s_period_carry_us);

    g_gs358_last_raw_period_us = raw_period_us;
    
    g_gs358_last_adjusted_period_us = adjusted_period_us;
    
    g_gs358_last_period_us = adjusted_period_us;
    

    period_valid = GS358_IsPeriodValid(adjusted_period_us);
    
    g_gs358_last_period_valid = period_valid;
    

    if (period_valid != 0U)
    {
        /*
         * 修正周期有效：
         * 1. 当前边沿成为新的周期基准；
         * 2. 已使用的相位补偿清零；
         * 3. 1500 us 丢光超时重新开始；
         * 4. 其余确认计数逻辑保持原仓库方法。
         */
        s_period_carry_us = 0U;
        g_gs358_last_period_carry_us = 0U;

        GS358_PeriodTimerRestart();
        GS358_LightLostTimerRestart();

//        g_gs358_period_valid_total++;

//        if (s_edge_confirm_count < GS358_EDGE_CONFIRM_COUNT)
//        {
//            s_edge_confirm_count++;
//        }

//        if (s_edge_confirm_count >= GS358_EDGE_CONFIRM_COUNT)
//        {
//            GS358_SetLightState(1U);
//        }

        g_gs358_period_valid_total++;

        /*
         * 当前周期有效，占用窗口中的一个位置。
         */
        if (s_window_valid_count < GS358_DETECT_WINDOW_COUNT)
        {
            s_window_valid_count++;
        }

        /*
         * 原来的确认计数可以继续保留作为调试信息，
         * 但不再由它直接更新有光输出。
         */
        if (s_edge_confirm_count < GS358_EDGE_CONFIRM_COUNT)
        {
            s_edge_confirm_count++;
        }

        /*
         * valid + miss达到10后统一完成本轮判断。
         */
        GS358_CheckDetectionWindowComplete();
    }
    else if (adjusted_period_us < period_min_us)
    {
        /*
         * 提前边沿：视为毛刺。
         * 不清 TIM1，不改 carry，不重启 TIM3。
         * 后面的真实边沿仍相对于原周期基准测量。
         */
        g_gs358_period_invalid_total++;

#if (GS358_PERIOD_ERROR_RESET_CONFIRM != 0U)
        s_edge_confirm_count = 0U;
#endif
    }
    else if (adjusted_period_us > period_max_us)
    {
        /*
         * 迟到边沿：认为至少错过了一个有效周期。
         *
         * adjusted = raw + 旧 carry
         * 新 carry = adjusted 对 75 us 的相位余量
         *
         * 当前边沿作为新的硬件计时起点，所以清零 TIM1；
         * 但不重启 TIM3，1500 us 超时仍以最后一个真正有效边沿为基准。
         */
//        g_gs358_period_invalid_total++;
//        g_gs358_period_miss_total++;

//        s_period_carry_us = GS358_CalculateNextCarry(adjusted_period_us);
//        
//        g_gs358_last_period_carry_us = s_period_carry_us;

//        GS358_PeriodTimerRestart();

//#if (GS358_PERIOD_ERROR_RESET_CONFIRM != 0U)
//        s_edge_confirm_count = 0U;
//#endif

         /*
         * 迟到边沿：
         * 1. 认为漏掉一个有效周期；
         * 2. 当前窗口miss次数加1；
         * 3. 保存相位余量；
         * 4. 当前边沿作为新的TIM1计时起点；
         * 5. 不重启TIM3，超时仍从最后一个有效周期计算。
         */
        g_gs358_period_invalid_total++;
        g_gs358_period_miss_total++;

        if (s_window_miss_count < GS358_DETECT_WINDOW_COUNT)
        {
            s_window_miss_count++;
        }

        s_period_carry_us = GS358_CalculateNextCarry(adjusted_period_us);

        g_gs358_last_period_carry_us = s_period_carry_us;

        GS358_PeriodTimerRestart();

#if (GS358_PERIOD_ERROR_RESET_CONFIRM != 0U)
        s_edge_confirm_count = 0U;
#endif

        GS358_CheckDetectionWindowComplete();
    }
    else
    {
        /* 理论上不会进入，仅作保护。 */
        g_gs358_period_invalid_total++;
    }
}

void GS358_LightLostTimerIRQHandler(void)
{
    if (TIM_GetITStatus(GS358_TIMEOUT_TIMER,
                        TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER,
                              TIM_IT_Update);

        TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);

        GS358_PeriodTimerStop();

        s_period_carry_us = 0U;
        g_gs358_last_period_carry_us = 0U;

        GS358_ResetPeriodFilter();
        GS358_ResetDetectionWindow();

        GS358_SetLightState(0U);
    }
}

/* =========================== 1 ms SysTick =========================== */

//static void GS358_SysTickInit(void)
//{
//    RCC_ClocksTypeDef clocks;

//    RCC_GetClocksFreq(&clocks);

//    /*
//     * SysTick 保持原来的 1 ms，只用于 PLATFORM_DelayTick。
//     * 丢光超时判断已经完全交给 TIM3。
//     */
//    if (SysTick_Config(clocks.HCLK_Frequency / 1000U) != 0U)
//    {
//        while (1)
//        {
//            /* SysTick 配置失败。 */
//        }
//    }

//    NVIC_SetPriority(SysTick_IRQn, 3U);
//}

/* =========================== ADC 五通道扫描 =========================== */

static void GS358_ADCInit(void)
{
    ADC_InitTypeDef adc_init;
    volatile uint32_t startup_delay;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1,
                           ENABLE);

    ADC_DeInit(ADC1);

    ADC_StructInit(&adc_init);
    adc_init.ADC_Resolution = ADC_Resolution_12b;
    adc_init.ADC_Prescaler = ADC_Prescaler_16;
    adc_init.ADC_Mode = ADC_Mode_Scan;

    /*
     * 软件启动时外部触发源字段不会实际触发转换，
     * 这里保留库要求的合法默认值。
     */
    adc_init.ADC_ExternalTrigConv =
        ADC_ExtTrig_T1_CC1;

    adc_init.ADC_DataAlign =
        ADC_DataAlign_Right;

    ADC_Init(ADC1, &adc_init);

    /*
     * 使用最长采样时间，提高对板上分压、电流检测和偏置节点的兼容性。
     * 后续确认源阻抗较低后，可以缩短以提高帧率。
     */
    ADC_SampleTimeConfig(ADC1,
                         ADC_SampleTime_240_5);

    /* 固定扫描顺序：0 -> 1 -> 2 -> 3 -> 5。 */
    ADC_AnyChannelSelect(ADC1,
                         ADC_AnyChannel_0,
                         ADC_Channel_0);

    ADC_AnyChannelSelect(ADC1,
                         ADC_AnyChannel_1,
                         ADC_Channel_1);

    ADC_AnyChannelSelect(ADC1,
                         ADC_AnyChannel_2,
                         ADC_Channel_2);

    ADC_AnyChannelSelect(ADC1,
                         ADC_AnyChannel_3,
                         ADC_Channel_3);

    ADC_AnyChannelSelect(ADC1,
                         ADC_AnyChannel_4,
                         ADC_Channel_5);

    ADC_AnyChannelNumCfg(ADC1,
                         GS358_ADC_CHANNEL_NUM_FIELD);

    ADC_AnyChannelCmd(ADC1, ENABLE);

    s_adc_eoc_count = 0U;

    ADC_ClearITPendingBit(ADC1,
                          ADC_IT_EOC);

    ADC_ITConfig(ADC1,
                 ADC_IT_EOC,
                 ENABLE);

    NVIC_ClearPendingIRQ(ADC1_IRQn);
    NVIC_SetPriority(ADC1_IRQn, 2U);
    NVIC_EnableIRQ(ADC1_IRQn);

    ADC_Cmd(ADC1, ENABLE);

    /*
     * 手册要求 ADC 上电后等待约 200 ns；
     * 这里留出更宽裕的软件延时。
     */
    for (startup_delay = 0U;
         startup_delay < 64U;
         startup_delay++)
    {
        __NOP();
    }

    /* 启动第一帧五通道单周期扫描。 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void GS358_ADC_EOCIRQHandler(void)
{
    if ((ADC1->ADSTA & ADC_ADSTA_ADIF_Msk) != 0U)
    {
        ADC_ClearITPendingBit(ADC1,
                              ADC_IT_EOC);

        if (s_adc_eoc_count < GS358_ADC_CHANNEL_COUNT)
        {
            s_adc_eoc_count++;
        }

        /*
         * MG32F003 的每一路转换完成都会产生 EOC。
         * 第五次 EOC 表示 0、1、2、3、5 已全部转换完成。
         */
        if (s_adc_eoc_count >= GS358_ADC_CHANNEL_COUNT)
        {
            s_adc_eoc_count = 0U;

            g_gs358_adc_values[0] =
                ADC_GetChannelConvertedValue(ADC1,
                                             ADC_Channel_0);

            g_gs358_adc_values[1] =
                ADC_GetChannelConvertedValue(ADC1,
                                             ADC_Channel_1);

            g_gs358_adc_values[2] =
                ADC_GetChannelConvertedValue(ADC1,
                                             ADC_Channel_2);

            g_gs358_adc_values[3] =
                ADC_GetChannelConvertedValue(ADC1,
                                             ADC_Channel_3);

            g_gs358_adc_values[4] =
                ADC_GetChannelConvertedValue(ADC1,
                                             ADC_Channel_5);

            g_gs358_adc_frame_count++;

            /* 用户后续处理入口。 */
            GS358_ADC_ScanCompleteIRQHook();

            /* 单周期扫描结束后，软件启动下一帧。 */
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        }
    }
}

void GS358_ADC_ScanCompleteIRQHook(void)
{
    /*
     * USER CODE BEGIN ADC_SCAN_COMPLETE_IRQ
     *
     * 五路 ADC 已按以下顺序保存：
     * g_gs358_adc_values[0] = ADC_IN0
     * g_gs358_adc_values[1] = ADC_IN1
     * g_gs358_adc_values[2] = ADC_IN2
     * g_gs358_adc_values[3] = ADC_IN3
     * g_gs358_adc_values[4] = ADC_IN5
     *
     * 注意：本函数在 ADC 中断内运行，不要加入长延时、printf、
     * Flash 擦写或复杂排序。推荐只复制数据或置位处理标志。
     *
     * USER CODE END ADC_SCAN_COMPLETE_IRQ
     */
}

/* =========================== PA11 PWM 预留 =========================== */

void GS358_PWM_ConfigureReserved(uint32_t frequency_hz,
                                uint16_t duty_permille)
{
    (void)frequency_hz;
    (void)duty_permille;

    /*
     * USER CODE BEGIN PWM_RESERVED
     *
     * 当前硬件 PA11 可复用为 TIM14_CH1（AF3）。
     * 后续需要发射 PWM 时，可在此处：
     * 1. 关闭 PA11 普通 GPIO 输出；
     * 2. GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_3)；
     * 3. PA11 配置为 GPIO_Mode_AF_PP；
     * 4. 初始化 TIM14 周期、比较值及输出极性；
     * 5. 启动 TIM14。
     *
     * USER CODE END PWM_RESERVED
     */
}

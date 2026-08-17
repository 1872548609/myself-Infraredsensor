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

/* =========================== 定时器分配 =========================== */

/*
 * 定时器实例统一在 gs358_app.h 中配置：
 *
 * GS358_PERIOD_TIMER  = TIM3，用于周期计时；
 * GS358_TIMEOUT_TIMER = TIM1，用于1500 us丢光超时。
 *
 * 两个定时器均配置为1 MHz，因此CNT单位为us。
 */

/* =========================== 全局状态 =========================== */

volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT] = {0U};
volatile uint32_t g_gs358_adc_frame_count = 0U;
volatile uint32_t g_gs358_compare_edge_total = 0U;
volatile uint8_t  g_gs358_light_present = 0U;

volatile uint16_t g_gs358_last_period_us = 0U;
volatile uint32_t g_gs358_period_valid_total = 0U;
volatile uint32_t g_gs358_period_invalid_total = 0U;
volatile uint8_t  g_gs358_last_period_valid = 0U;

/* 本次运行期间累计的超长周期次数。 */
volatile uint32_t g_gs358_period_miss_total = 0U;

/*
 * 便于 Keil Watch 观察。
 * 简化后 adjusted 始终等于 raw，carry 始终为 0。
 * 保留变量名是为了避免已有 Watch 配置失效。
 */
volatile uint16_t g_gs358_last_raw_period_us = 0U;
volatile uint16_t g_gs358_last_adjusted_period_us = 0U;
volatile uint16_t g_gs358_last_period_carry_us = 0U;


/* =========================== 内部状态 =========================== */

/*
 * 一个统计窗口内：
 * s_period_sample_count：已经统计的周期总数；
 * s_edge_confirm_count ：其中有效周期的数量。
 */
static volatile uint8_t s_period_sample_count = 0U;
static volatile uint8_t s_edge_confirm_count = 0U;
static volatile uint8_t s_period_reference_ready = 0U;

static volatile GS358_LedMode s_led_mode = GS358_LED_BLOCKED_ON;

volatile uint16_t validcount = 0;
volatile uint16_t valid_count[100] = {0};

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
     * 先初始化TIM3周期计时和TIM1丢光超时，
     * 再开启PA7外部中断，避免边沿提前到来。
     */
    GS358_PeriodTimerInit();
    GS358_LightLostTimerInit();

    GS358_EXTIInit();
    GS358_ADCInit();

    /* 周期检测状态初始化。 */
    s_period_sample_count = 0U;
    s_edge_confirm_count = 0U;
    s_period_reference_ready = 0U;

    g_gs358_light_present = 0U;
    g_gs358_last_period_us = 0U;
    g_gs358_period_valid_total = 0U;
    g_gs358_period_invalid_total = 0U;
    g_gs358_last_period_valid = 0U;
    g_gs358_period_miss_total = 0U;

    g_gs358_last_raw_period_us = 0U;
    g_gs358_last_adjusted_period_us = 0U;
    g_gs358_last_period_carry_us = 0U;


    /*
     * 上电默认按“遮光/无光”状态输出。
     * TIM1超时定时器此时不启动，收到第一条边沿后才开始计时。
     */
    GS358_ApplyOutputs();
    
    
    /*
     * 所有硬件初始化完成后再启动独立看门狗。
     */
//    GS358_WatchdogInit();
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
     * 不要把喂狗放到比较器、TIM1或ADC中断中，
     * 否则主循环即使跑飞，只要中断仍然进入，
     * 看门狗就可能无法检测到故障。
     */
    GS358_WatchdogFeed();
}
/* =========================== 独立看门狗 =========================== */

static void GS358_WatchdogInit(void)
{
#if (GS358_WATCHDOG_ENABLE != 0U)

    /*
     * IWDG 使用独立 LSI 时钟。
     * 必须先开启 LSI，否则写 PR 后 PVU 可能一直不清零。
     */
    RCC_LSICmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
        /*
         * 等待 LSI 稳定。
         *
         * 调试时不要在这里逐语句单步，
         * 直接全速运行到后面的断点。
         */
    }

    /*
     * 设置为计数到零后产生系统复位。
     */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_OverflowConfig(IWDG_Overflow_Reset);

    /*
     * 修改预分频值之前，先确认上一次 PR 更新已经完成。
     */
    PVU_CheckStatus();

    /*
     * 开启写权限并设置 32 分频。
     *
     * LSI 标称 40 kHz：
     * 32 / 40000 = 0.8 ms/count。
     */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /*
     * 修改重装载值之前，先确认上一次 RLR 更新已经完成。
     */
    RVU_CheckStatus();

    /*
     * 写入 12 位重装载值。
     *
     * Reload = 2499 时：
     * (2499 + 1) × 32 / 40000
     * = 2 秒标称超时。
     */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(
        (uint16_t)(GS358_WATCHDOG_RELOAD_VALUE & 0x0FFFU));

    /*
     * 先把 RLR 装载到实际递减计数器。
     * 该操作相当于向 IWDG_KR 写入 0xAAAA。
     */
    IWDG_ReloadCounter();

    /*
     * 最后启动 IWDG。
     * 该操作相当于向 IWDG_KR 写入 0xCCCC。
     */
    IWDG_Enable();

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
#if (GS358_COMPARE_IRQ_TEST_ENABLE == 0U)
    uint8_t nc_active;
#endif
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
#if (GS358_COMPARE_IRQ_TEST_ENABLE == 0U)
    nc_active = (blocked == 0U) ? 1U : 0U;
#endif

    GS358_WriteLogicalOutput(GS358_OUTPUT_NO_PORT,
                             GS358_OUTPUT_NO_PIN,
                             no_active,
                             GS358_NO_OUTPUT_ACTIVE_HIGH);

#if (GS358_COMPARE_IRQ_TEST_ENABLE == 0U)
    /* 测试功能关闭时，PA8保持原来的正常NC输出逻辑。 */
    GS358_WriteLogicalOutput(GS358_OUTPUT_NC_PORT,
                             GS358_OUTPUT_NC_PIN,
                             nc_active,
                             GS358_NC_OUTPUT_ACTIVE_HIGH);
#endif

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

    /* 清除当前10次统计窗口。 */
    s_period_sample_count = 0U;
    s_edge_confirm_count = 0U;

    g_gs358_last_period_us = 0U;
    g_gs358_last_period_valid = 0U;
}

/*
 * TIM3：只测量相邻两个下降沿之间的时间。
 * 无中断、自由向上计数。
 */
static void GS358_PeriodTimerInit(void)
{
    TIM_TimeBaseInitTypeDef timer_init;
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TIM3, ENABLE);
    TIM_DeInit(GS358_PERIOD_TIMER);

    timer_clock_hz = TIM_GetTIMxClock(GS358_PERIOD_TIMER);
    prescaler_div = timer_clock_hz / GS358_TIMER_TICK_HZ;

    if ((prescaler_div == 0U) ||
        ((timer_clock_hz % GS358_TIMER_TICK_HZ) != 0U))
    {
        while (1)
        {
            /* TIM3无法精确分频到1 MHz。 */
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

/* TIM1：只负责最后一个有效边沿之后1500 us无有效边沿超时。 */
static void GS358_LightLostTimerInit(void)
{
    TIM_TimeBaseInitTypeDef timer_init;
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TIM1, ENABLE);
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
            /* TIM1参数错误。 */
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

    NVIC_ClearPendingIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1U);
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    TIM_SetCounter(GS358_TIMEOUT_TIMER, 0U);
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);
}

static void GS358_LightLostTimerRestart(void)
{
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);
    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER, TIM_IT_Update);
    NVIC_ClearPendingIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
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
    uint16_t measured_period_us;
    uint8_t period_valid;

#if (GS358_COMPARE_IRQ_TEST_ENABLE != 0U)
    /* 进入比较器下降沿中断：PA8测试输出动作。 */
    GS358_WriteLogicalOutput(GS358_OUTPUT_NC_PORT,
                             GS358_OUTPUT_NC_PIN,
                             1U,
                             GS358_NC_OUTPUT_ACTIVE_HIGH);
#endif

    g_gs358_compare_edge_total++;

    /* 第一条边沿只建立时间基准。 */
    if (s_period_reference_ready == 0U)
    {
        s_period_reference_ready = 1U;

        g_gs358_last_period_us = 0U;
        g_gs358_last_raw_period_us = 0U;
        g_gs358_last_adjusted_period_us = 0U;
        g_gs358_last_period_carry_us = 0U;
        g_gs358_last_period_valid = 0U;

        GS358_PeriodTimerRestart();
        GS358_LightLostTimerRestart();
        return;
    }

    /*
     * 直接读取相邻两次下降沿的间隔，并立即重新计时。
     * 不再区分提前毛刺和迟到边沿，也不再进行相位补偿。
     * 因此每一个下降沿都会成为下一次测量的起点。
     */
    measured_period_us =
        (uint16_t)TIM_GetCounter(GS358_PERIOD_TIMER);
    GS358_PeriodTimerRestart();

    g_gs358_last_raw_period_us = measured_period_us;
    g_gs358_last_adjusted_period_us = measured_period_us;
    g_gs358_last_period_us = measured_period_us;
    g_gs358_last_period_carry_us = 0U;

    period_valid = GS358_IsPeriodValid(measured_period_us);
    g_gs358_last_period_valid = period_valid;
    
    validcount++;
    
    valid_count[validcount] = g_gs358_last_period_us;
        
    if(validcount>=100)
    {
        
        validcount = 0;
    }

    /*
     * 每个完成测量的周期都占用一个统计名额。
     * 毛刺或周期错误只作为一次无效样本，不立即清空有效计数。
     */
    if (s_period_sample_count < GS358_PERIOD_CHECK_COUNT)
    {
        s_period_sample_count++;
    }

    if (period_valid != 0U)
    {
        /*
         * 本次周期有效：
         * 1. 重启丢光超时；
         * 2. 记录有效总数；
         * 3. 当前统计窗口的有效数加一。
         */
        GS358_LightLostTimerRestart();

        g_gs358_period_valid_total++;

        if (s_edge_confirm_count < GS358_EDGE_CONFIRM_COUNT)
        {
            s_edge_confirm_count++;
        }

    
    }
    else
    {
        /*
         * 本次周期无效：
         * 只记录为当前10次窗口中的一个无效样本，
         * 不清除前面已经累计的有效周期。
         */
        g_gs358_period_invalid_total++;

        if ((uint32_t)measured_period_us >
            ((uint32_t)GS358_SIGNAL_PERIOD_US +
             (uint32_t)GS358_SIGNAL_PERIOD_TOLERANCE_US))
        {
            g_gs358_period_miss_total++;
        }
    }

    /*
     * 10次中有效数达到设定值，例如7次，立即确认有光。
     *
     * 达到7次后无需继续等待剩余3次，响应速度更快。
     */
    if (s_edge_confirm_count >= GS358_EDGE_CONFIRM_COUNT)
    {
        GS358_SetLightState(1U);

        /*
         * 本轮已经成功，重新开始下一轮统计。
         * 有光状态仍由TIM1超时负责撤销。
         */
        s_period_sample_count = 0U;
        s_edge_confirm_count = 0U;
    }
    else if (s_period_sample_count >= GS358_PERIOD_CHECK_COUNT)
    {
        /*
         * 已经统计满10次，但有效数不足7次。
         * 本轮判定失败，开始下一轮。
         *
         * 不在这里立即设置无光，防止短时干扰造成输出抖动。
         * 已经确认的有光状态仍由TIM1丢光超时撤销。
         */
        s_period_sample_count = 0U;
        s_edge_confirm_count = 0U;
    }
    
#if (GS358_COMPARE_IRQ_TEST_ENABLE != 0U)
    /* 本次周期有效：PA8测试输出释放。 */
    GS358_WriteLogicalOutput(GS358_OUTPUT_NC_PORT,
                             GS358_OUTPUT_NC_PIN,
                             0U,
                             GS358_NC_OUTPUT_ACTIVE_HIGH);
#endif
    
    
//#if (GS358_COMPARE_IRQ_TEST_ENABLE != 0U)
//        GS358_WriteLogicalOutput(GS358_OUTPUT_NC_PORT,
//                                 GS358_OUTPUT_NC_PIN,
//                                 0U,
//                                 GS358_NC_OUTPUT_ACTIVE_HIGH);
//#endif
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

        g_gs358_last_period_carry_us = 0U;

        GS358_ResetPeriodFilter();

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
//     * 丢光超时判断已经完全交给TIM1。
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
        /*
         * 单周期扫描完成：
         * ADC0、ADC1、ADC2、ADC3、ADC5 的结果已经分别写入
         * 对应通道数据寄存器。
         */
        ADC_ClearITPendingBit(ADC1,
                              ADC_IT_EOC);

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

        /*
         * 用户处理入口。
         * 中断中不要执行延时、打印或复杂算法。
         */
        GS358_ADC_ScanCompleteIRQHook();

        /*
         * 当前是单周期扫描模式。
         * 一帧完成后 ADST 被硬件清零，因此需要软件重新启动下一帧。
         */
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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

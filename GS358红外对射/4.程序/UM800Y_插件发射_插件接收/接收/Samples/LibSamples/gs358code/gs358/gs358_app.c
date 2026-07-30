/**
 * @file gs358_app.c
 * @brief GS358 红外对射接收板应用层
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

#define GS358_ADC_GPIOA_PINS         (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_12)
#define GS358_ADC_GPIOB_PINS         (GPIO_Pin_0 | GPIO_Pin_1)
#define GS358_ADC_CHANNEL_NUM_FIELD  (GS358_ADC_CHANNEL_COUNT - 1U)

/* =========================== 全局状态 =========================== */

volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT] = {0U};
volatile uint32_t g_gs358_adc_frame_count = 0U;

volatile uint32_t g_gs358_compare_edge_total = 0U;
volatile uint8_t  g_gs358_light_present = 0U;

volatile uint16_t g_gs358_last_period_us = 0U;
volatile uint32_t g_gs358_period_valid_total = 0U;
volatile uint32_t g_gs358_period_invalid_total = 0U;
volatile uint8_t  g_gs358_last_period_valid = 0U;

/* =========================== 内部状态 =========================== */

static volatile uint8_t s_edge_confirm_count = 0U;
static volatile uint8_t s_period_reference_ready = 0U;
static volatile GS358_LedMode s_led_mode = GS358_LED_BLOCKED_ON;

/* =========================== 内部函数声明 =========================== */

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
    GS358_GPIOInit();

    GS358_PeriodTimerInit();
    GS358_LightLostTimerInit();

    GS358_EXTIInit();
    GS358_ADCInit();

    s_edge_confirm_count = 0U;
    s_period_reference_ready = 0U;

    g_gs358_light_present = 0U;
    g_gs358_last_period_us = 0U;
    g_gs358_period_valid_total = 0U;
    g_gs358_period_invalid_total = 0U;
    g_gs358_last_period_valid = 0U;

    GS358_ApplyOutputs();
    GS358_WatchdogInit();
}

void GS358_AppProcess(void)
{
    GS358_WatchdogFeed();
}

/* =========================== 独立看门狗 =========================== */

static void GS358_WatchdogInit(void)
{
#if (GS358_WATCHDOG_ENABLE != 0U)

    RCC_LSICmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
        /* 等待 LSI 稳定。 */
    }

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_OverflowConfig(IWDG_Overflow_Reset);

    PVU_CheckStatus();

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    RVU_CheckStatus();

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetReload(
        (uint16_t)(GS358_WATCHDOG_RELOAD_VALUE & 0x0FFFU));

    IWDG_ReloadCounter();
    IWDG_Enable();

#endif
}

static void GS358_WatchdogFeed(void)
{
#if (GS358_WATCHDOG_ENABLE != 0U)

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

    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_COMP_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_High;
    gpio_init.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GS358_COMP_PORT, &gpio_init);

    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_ADC_GPIOA_PINS;
    gpio_init.GPIO_Speed = GPIO_Speed_Low;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio_init);

    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_ADC_GPIOB_PINS;
    gpio_init.GPIO_Speed = GPIO_Speed_Low;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &gpio_init);

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

    blocked = (g_gs358_light_present == 0U) ? 1U : 0U;

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

static uint8_t GS358_IsPeriodValid(uint16_t period_us)
{
    uint32_t period_min_us;
    uint32_t period_max_us;
    uint32_t measured_period_us;

    period_min_us =
        (uint32_t)GS358_SIGNAL_PERIOD_US -
        (uint32_t)GS358_SIGNAL_PERIOD_TOLERANCE_US;

    period_max_us =
        (uint32_t)GS358_SIGNAL_PERIOD_US +
        (uint32_t)GS358_SIGNAL_PERIOD_TOLERANCE_US;

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

/* =========================== 周期计时器 TIM1 =========================== */

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
            /* 无法精确分频到 1 MHz。 */
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

/* =========================== 丢光超时定时器 TIM3 =========================== */

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
        ((timer_clock_hz % GS358_TIMER_TICK_HZ) != 0U))
    {
        while (1)
        {
            /* 无法精确分频到 1 MHz。 */
        }
    }

    TIM_TimeBaseStructInit(&timer_init);
    timer_init.TIM_Prescaler = (uint16_t)(prescaler_div - 1U);
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    timer_init.TIM_Period =
        (uint16_t)(GS358_LIGHT_LOST_TIMEOUT_US - 1U);
    timer_init.TIM_ClockDivision = TIM_CKD_Div1;
    timer_init.TIM_RepetitionCounter = 0U;
    TIM_TimeBaseInit(GS358_TIMEOUT_TIMER, &timer_init);

    TIM_SelectOnePulseMode(GS358_TIMEOUT_TIMER,
                           TIM_OPMode_Single);

    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER,
                          TIM_IT_Update);

    TIM_ITConfig(GS358_TIMEOUT_TIMER,
                 TIM_IT_Update,
                 ENABLE);

    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 1U);
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM_SetCounter(GS358_TIMEOUT_TIMER, 0U);
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);
}

static void GS358_LightLostTimerRestart(void)
{
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);
    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER,
                          TIM_IT_Update);
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
    uint16_t period_us;
    uint8_t period_valid;

    g_gs358_compare_edge_total++;

    /*
     * 第一条下降沿只建立参考，不参与有效周期计数。
     */
    if (s_period_reference_ready == 0U)
    {
        s_period_reference_ready = 1U;

        g_gs358_last_period_us = 0U;
        g_gs358_last_period_valid = 0U;

        GS358_PeriodTimerRestart();
        GS358_LightLostTimerRestart();
        return;
    }

    /*
     * 先读取计数值。
     */
    period_us =
        (uint16_t)TIM_GetCounter(GS358_PERIOD_TIMER);

    /*
     * 检查 TIM1 是否已经发生过16位计数溢出。
     *
     * TIM1为1 MHz时：
     * 65536个计数约等于65.536 ms。
     *
     * 一旦溢出，CNT已经回绕，当前period_us不能再表示
     * 两个边沿之间的真实时间，因此直接作为无效周期处理。
     *
     * 注意：必须在GS358_PeriodTimerRestart()之前检查，
     * 因为Restart函数会清除Update标志。
     */
    if (TIM_GetFlagStatus(GS358_PERIOD_TIMER,
                          TIM_FLAG_Update) != RESET)
    {
        /*
         * 当前边沿重新作为下一次测量起点。
         */
        GS358_PeriodTimerRestart();

        g_gs358_last_period_us = period_us;
        g_gs358_last_period_valid = 0U;
        g_gs358_period_invalid_total++;

#if (GS358_PERIOD_ERROR_RESET_CONFIRM != 0U)
        s_edge_confirm_count = 0U;
#endif

        /*
         * 不刷新TIM3。
         * 丢光超时仍然从最后一个有效周期开始计算。
         */
        return;
    }

    /*
     * 当前边沿作为下一次周期测量起点。
     */
    GS358_PeriodTimerRestart();

    g_gs358_last_period_us = period_us;

    period_valid = GS358_IsPeriodValid(period_us);
    g_gs358_last_period_valid = period_valid;

    if (period_valid != 0U)
    {
        GS358_LightLostTimerRestart();

        g_gs358_period_valid_total++;

        if (s_edge_confirm_count < GS358_EDGE_CONFIRM_COUNT)
        {
            s_edge_confirm_count++;
        }

        if (s_edge_confirm_count >= GS358_EDGE_CONFIRM_COUNT)
        {
            s_edge_confirm_count =
                GS358_EDGE_CONFIRM_COUNT;

            GS358_SetLightState(1U);
        }
    }
    else
    {
        g_gs358_period_invalid_total++;

#if (GS358_PERIOD_ERROR_RESET_CONFIRM != 0U)
        s_edge_confirm_count = 0U;
#endif
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
        GS358_ResetPeriodFilter();
        GS358_SetLightState(0U);
    }
}

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
    adc_init.ADC_ExternalTrigConv = ADC_ExtTrig_T1_CC1;
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &adc_init);

    ADC_SampleTimeConfig(ADC1,
                         ADC_SampleTime_240_5);

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

    for (startup_delay = 0U;
         startup_delay < 64U;
         startup_delay++)
    {
        __NOP();
    }

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void GS358_ADC_EOCIRQHandler(void)
{
    if ((ADC1->ADSTA & ADC_ADSTA_ADIF_Msk) != 0U)
    {
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

        GS358_ADC_ScanCompleteIRQHook();

        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    }
}

void GS358_ADC_ScanCompleteIRQHook(void)
{
    /* 用户代码区域：中断内只建议复制数据或置标志。 */
}

/* =========================== PA11 PWM 预留 =========================== */

void GS358_PWM_ConfigureReserved(uint32_t frequency_hz,
                                 uint16_t duty_permille)
{
    (void)frequency_hz;
    (void)duty_permille;

    /* PA11 当前保持普通推挽低电平。 */
}

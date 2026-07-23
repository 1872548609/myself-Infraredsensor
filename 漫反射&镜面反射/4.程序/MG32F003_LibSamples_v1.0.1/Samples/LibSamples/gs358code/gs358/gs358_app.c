/**
 * @file gs358_app.c
 * @brief GS358 红外对射接收板初版应用
 */

#include "platform.h"
#include "gs358_app.h"
#include "mg32_tim.h"

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
 * TIM3 只作为内部 16 位计时器使用，不配置任何 TIM3 通道输出，
 * 因此不会改变 PA7 当前的普通输入/EXTI 配置。
 */
#define GS358_TIMEOUT_TIMER          TIM3

/* PA11 使用 TIM14_CH1（AF3）输出发射 PWM。 */
#define GS358_PWM_TIMER              TIM14

/* =========================== 全局状态 =========================== */

volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT] = {0U};
volatile uint32_t g_gs358_adc_frame_count = 0U;
volatile uint32_t g_gs358_compare_edge_total = 0U;
volatile uint8_t g_gs358_light_present = 0U;

/* =========================== 内部状态 =========================== */

static volatile uint8_t s_edge_confirm_count = 0U;
static volatile uint8_t s_adc_eoc_count = 0U;

static volatile GS358_LedMode s_led_mode =
    GS358_LED_BLOCKED_ON;

/* =========================== 内部函数声明 =========================== */

static void GS358_SysTickInit(void);
static void GS358_GPIOInit(void);
static void GS358_EXTIInit(void);
static void GS358_ADCInit(void);

static void GS358_LightLostTimerInit(void);
static void GS358_LightLostTimerRestart(void);
static void GS358_PWMInit(void);

static void GS358_SetLightState(uint8_t light_present);
static void GS358_ApplyOutputs(void);

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
    GS358_SysTickInit();
    GS358_GPIOInit();

    /*
     * 先初始化 TIM3，再开启 PA7 外部中断。
     * 避免 PA7 提前产生下降沿时 TIM3 尚未配置完成。
     */
    GS358_LightLostTimerInit();

    GS358_EXTIInit();
    GS358_ADCInit();

    s_edge_confirm_count = 0U;
    g_gs358_light_present = 0U;

    /*
     * 上电默认按“遮光/无光”状态输出。
     * TIM3 此时不启动，收到第一个有效下降沿后才开始超时计时。
     */
    GS358_ApplyOutputs();

    /*
     * 接收端、EXTI 和超时定时器全部准备完成后再启动发射 PWM。
     */
    GS358_PWMInit();
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
}

/* =========================== GPIO =========================== */

static void GS358_GPIOInit(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA |
                          RCC_AHBPERIPH_GPIOB,
                          ENABLE);

    /*
     * 先写入安全初值，再切换成输出模式。
     * PA11 在 TIM14 初始化前也先保持低电平。
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
        GS358_RED_LED_PIN;
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

    /* PA11 在 GS358_PWMInit() 前保持低电平。 */
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

    normalized =
        (light_present != 0U) ? 1U : 0U;

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

/* =========================== TIM3 丢光超时检测 =========================== */

static void GS358_LightLostTimerInit(void)
{
    TIM_TimeBaseInitTypeDef timer_init;
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TIM3, ENABLE);

    TIM_DeInit(GS358_TIMEOUT_TIMER);

    /*
     * 读取 TIM3 实际输入时钟，并分频到 1 MHz。
     *
     * 例如 TIM3 时钟为 48 MHz：
     * prescaler_div = 48
     * PSC = 47
     * 计数频率 = 48 MHz / 48 = 1 MHz
     */
    timer_clock_hz =
        TIM_GetTIMxClock(GS358_TIMEOUT_TIMER);

    prescaler_div =
        timer_clock_hz / GS358_TIMEOUT_TIMER_TICK_HZ;

    /*
     * TIM3 为 16 位定时器，ARR 最大为 65535，
     * 所以 1 MHz 计数时最大单次超时为 65536 us。
     */
    if ((prescaler_div == 0U) ||
        ((timer_clock_hz % GS358_TIMEOUT_TIMER_TICK_HZ) != 0U) ||
        (GS358_LIGHT_LOST_TIMEOUT_US == 0U) ||
        (GS358_LIGHT_LOST_TIMEOUT_US > 65536UL))
    {
        while (1)
        {
            /* TIM3 时钟或超时参数非法。 */
        }
    }

    TIM_TimeBaseStructInit(&timer_init);

    timer_init.TIM_Prescaler =
        (uint16_t)(prescaler_div - 1U);

    timer_init.TIM_CounterMode =
        TIM_CounterMode_Up;

    /*
     * 定时器从 0 计数到 ARR 后产生更新事件。
     *
     * 1500 us：
     * ARR = 1500 - 1 = 1499
     */
    timer_init.TIM_Period =
        (uint32_t)(GS358_LIGHT_LOST_TIMEOUT_US - 1U);

    timer_init.TIM_ClockDivision =
        TIM_CKD_Div1;

    timer_init.TIM_RepetitionCounter = 0U;

    TIM_TimeBaseInit(GS358_TIMEOUT_TIMER,
                     &timer_init);

    /*
     * 单脉冲模式：
     * 达到超时时间产生更新事件后，硬件自动清除 CEN 并停止。
     */
    TIM_SelectOnePulseMode(GS358_TIMEOUT_TIMER,
                           TIM_OPMode_Single);

    /*
     * 初始化函数会产生一次更新事件以装载 PSC。
     * 在开启更新中断前，先停止定时器并清除这个初始化标志。
     */
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);

    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER,
                          TIM_IT_Update);

    NVIC_ClearPendingIRQ(TIM3_IRQn);

    TIM_ITConfig(GS358_TIMEOUT_TIMER,
                 TIM_IT_Update,
                 ENABLE);

    /*
     * 中断优先级：
     * PA7 EXTI = 0，最高，负责及时重启超时计时器；
     * TIM3     = 1，负责精确判定丢光；
     * ADC      = 2；
     * SysTick  = 3。
     */
    NVIC_SetPriority(TIM3_IRQn, 1U);
    NVIC_EnableIRQ(TIM3_IRQn);
}

static void GS358_LightLostTimerRestart(void)
{
    /*
     * 每个比较器下降沿都将超时计时重新从 0 开始。
     *
     * 先停止并清除 TIM3/NVIC 的旧超时状态，
     * 防止刚到达边界的旧更新事件在退出 EXTI 后误进入 TIM3 ISR。
     */
    TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);

    TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER,
                          TIM_IT_Update);

    NVIC_ClearPendingIRQ(TIM3_IRQn);

    TIM_SetCounter(GS358_TIMEOUT_TIMER, 0U);

    /* 重新启动 1.5 ms 单次超时计数。 */
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
    g_gs358_compare_edge_total++;

    /*
     * 每次收到光强比较器下降沿，都重新开始计算
     * GS358_LIGHT_LOST_TIMEOUT_US。
     */
    GS358_LightLostTimerRestart();

    if (s_edge_confirm_count < GS358_EDGE_CONFIRM_COUNT)
    {
        s_edge_confirm_count++;
    }

    /*
     * 达到确认次数后，直接在中断内更新两路输出和指示灯，
     * 满足快速响应要求。
     */
    if (s_edge_confirm_count >= GS358_EDGE_CONFIRM_COUNT)
    {
        GS358_SetLightState(1U);
    }
}

void GS358_LightLostTimerIRQHandler(void)
{
    if (TIM_GetITStatus(GS358_TIMEOUT_TIMER,
                        TIM_IT_Update) != RESET)
    {
        /*
         * 先清除更新中断标志，再处理状态。
         */
        TIM_ClearITPendingBit(GS358_TIMEOUT_TIMER,
                              TIM_IT_Update);

        /*
         * 单脉冲模式正常情况下已经自动停止；
         * 再次显式停止，确保定时器状态明确。
         */
        TIM_Cmd(GS358_TIMEOUT_TIMER, DISABLE);

        /*
         * 从最后一次下降沿开始，已经连续 1.5 ms 没有新下降沿。
         * 清除有光确认次数，下一次必须重新累计指定次数。
         */
        s_edge_confirm_count = 0U;

        GS358_SetLightState(0U);
    }
}

/* =========================== 1 ms SysTick =========================== */

static void GS358_SysTickInit(void)
{
    RCC_ClocksTypeDef clocks;

    RCC_GetClocksFreq(&clocks);

    /*
     * SysTick 保持原来的 1 ms，只用于 PLATFORM_DelayTick。
     * 丢光超时判断已经完全交给 TIM3。
     */
    if (SysTick_Config(clocks.HCLK_Frequency / 1000U) != 0U)
    {
        while (1)
        {
            /* SysTick 配置失败。 */
        }
    }

    NVIC_SetPriority(SysTick_IRQn, 3U);
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

/* =========================== PA11 / TIM14_CH1 发射 PWM =========================== */

static void GS358_PWMInit(void)
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef timer_init;
    TIM_OCInitTypeDef oc_init;
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;
    uint32_t period_ticks;
    uint32_t pulse_ticks;
    uint64_t period_scaled;
    uint64_t duty_scaled;

    /*
     * 即使关闭 PWM，也明确将 PA11 保持为低电平普通输出。
     */
    if (GS358_PWM_ENABLE == 0U)
    {
        GPIO_PinAFConfig(GS358_PWM_PORT,
                         GPIO_PinSource11,
                         GPIO_AF_0);

        GPIO_StructInit(&gpio_init);
        gpio_init.GPIO_Pin = GS358_PWM_PIN;
        gpio_init.GPIO_Speed = GPIO_Speed_High;
        gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GS358_PWM_PORT, &gpio_init);

        GPIO_ResetBits(GS358_PWM_PORT,
                       GS358_PWM_PIN);
        return;
    }

    /*
     * 参数检查：
     * 1. 计数频率和周期必须能够精确换算成整数个定时器计数；
     * 2. TIM14 为 16 位，周期计数不能超过 65536；
     * 3. 占空比使用 0～1000 的千分比表示。
     */
    if ((GS358_PWM_TIMER_TICK_HZ == 0UL) ||
        (GS358_PWM_PERIOD_US == 0UL) ||
        (GS358_PWM_DUTY_PERMILLE > 1000UL))
    {
        while (1)
        {
            /* PWM 宏参数非法。 */
        }
    }

    period_scaled =
        (uint64_t)GS358_PWM_TIMER_TICK_HZ *
        (uint64_t)GS358_PWM_PERIOD_US;

    if ((period_scaled % 1000000ULL) != 0ULL)
    {
        while (1)
        {
            /* 当前计数频率无法精确生成目标 us 周期。 */
        }
    }

    period_ticks =
        (uint32_t)(period_scaled / 1000000ULL);

    if ((period_ticks == 0UL) ||
        (period_ticks > 65536UL))
    {
        while (1)
        {
            /* TIM14 的 ARR 范围不足。 */
        }
    }

    duty_scaled =
        (uint64_t)period_ticks *
        (uint64_t)GS358_PWM_DUTY_PERMILLE;

    /* 四舍五入计算 CCR1。默认 1000 × 50 / 1000 = 50。 */
    pulse_ticks =
        (uint32_t)((duty_scaled + 500ULL) / 1000ULL);

    if (pulse_ticks > period_ticks)
    {
        pulse_ticks = period_ticks;
    }

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TIM14,
                           ENABLE);

    TIM_DeInit(GS358_PWM_TIMER);

    timer_clock_hz =
        TIM_GetTIMxClock(GS358_PWM_TIMER);

    prescaler_div =
        timer_clock_hz / GS358_PWM_TIMER_TICK_HZ;

    if ((prescaler_div == 0UL) ||
        ((timer_clock_hz % GS358_PWM_TIMER_TICK_HZ) != 0UL) ||
        (prescaler_div > 65536UL))
    {
        while (1)
        {
            /* TIM14 无法精确分频到目标计数频率。 */
        }
    }

    /*
     * PA11 复用表：AF3 = TIM14_CH1。
     */
    GPIO_ResetBits(GS358_PWM_PORT,
                   GS358_PWM_PIN);

    GPIO_PinAFConfig(GS358_PWM_PORT,
                     GPIO_PinSource11,
                     GPIO_AF_3);

    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Pin = GS358_PWM_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_High;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GS358_PWM_PORT, &gpio_init);

    TIM_TimeBaseStructInit(&timer_init);

    /*
     * 默认 TIM14 输入时钟若为 48 MHz：
     * PSC = 48 MHz / 1 MHz - 1 = 47。
     */
    timer_init.TIM_Prescaler =
        (uint16_t)(prescaler_div - 1UL);

    timer_init.TIM_CounterMode =
        TIM_CounterMode_Up;

    /*
     * 默认周期 1000 us、计数频率 1 MHz：
     * period_ticks = 1000，ARR = 999。
     */
    timer_init.TIM_Period =
        period_ticks - 1UL;

    timer_init.TIM_ClockDivision =
        TIM_CKD_Div1;

    timer_init.TIM_RepetitionCounter = 0U;

    TIM_TimeBaseInit(GS358_PWM_TIMER,
                     &timer_init);

    TIM_OCStructInit(&oc_init);
    oc_init.TIM_OCMode = TIM_OCMode_PWM1;
    oc_init.TIM_OutputState = TIM_OutputState_Enable;
    oc_init.TIM_Pulse = pulse_ticks;

    if (GS358_PWM_ACTIVE_HIGH != 0U)
    {
        oc_init.TIM_OCPolarity = TIM_OCPolarity_High;
    }
    else
    {
        oc_init.TIM_OCPolarity = TIM_OCPolarity_Low;
    }

    TIM_OC1Init(GS358_PWM_TIMER,
                &oc_init);

    TIM_OC1PreloadConfig(GS358_PWM_TIMER,
                         TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(GS358_PWM_TIMER,
                         ENABLE);

    TIM_SetCounter(GS358_PWM_TIMER, 0UL);

    /* TIM14 具有 MOE，必须开启主输出后 PA11 才能输出 PWM。 */
    TIM_CtrlPWMOutputs(GS358_PWM_TIMER,
                       ENABLE);

    TIM_Cmd(GS358_PWM_TIMER,
            ENABLE);
}

void GS358_PWM_Start(void)
{
    if (GS358_PWM_ENABLE != 0U)
    {
        TIM_SetCounter(GS358_PWM_TIMER, 0UL);
        TIM_CtrlPWMOutputs(GS358_PWM_TIMER, ENABLE);
        TIM_Cmd(GS358_PWM_TIMER, ENABLE);
    }
}

void GS358_PWM_Stop(void)
{
    TIM_Cmd(GS358_PWM_TIMER, DISABLE);
    TIM_CtrlPWMOutputs(GS358_PWM_TIMER, DISABLE);

    /*
     * 保留 PA11 的 AF3 配置，后续调用 GS358_PWM_Start() 可直接恢复。
     * MOE 关闭后输出空闲电平由 TIM14 硬件控制。
     */
}

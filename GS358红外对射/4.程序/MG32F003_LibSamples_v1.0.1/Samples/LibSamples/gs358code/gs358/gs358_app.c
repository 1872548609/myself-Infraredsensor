/**
 * @file    gs358_app.c
 * @brief   GS358 红外对射接收板初版应用
 */

#include "platform.h"
#include "gs358_app.h"

/* =========================== 引脚定义 =========================== */

#define GS358_COMP_PORT                   GPIOA
#define GS358_COMP_PIN                    GPIO_Pin_7

#define GS358_OUTPUT_NC_PORT              GPIOA
#define GS358_OUTPUT_NC_PIN               GPIO_Pin_8

#define GS358_OUTPUT_NO_PORT              GPIOA
#define GS358_OUTPUT_NO_PIN               GPIO_Pin_9

#define GS358_RED_LED_PORT                GPIOA
#define GS358_RED_LED_PIN                 GPIO_Pin_10

#define GS358_PWM_PORT                    GPIOA
#define GS358_PWM_PIN                     GPIO_Pin_11

/* ADC 通道在芯片上的物理引脚。 */
#define GS358_ADC_GPIOA_PINS              (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_12)
#define GS358_ADC_GPIOB_PINS              (GPIO_Pin_0 | GPIO_Pin_1)

/* ADC 任意通道扫描数量寄存器填写 N-1：5 路对应 4。 */
#define GS358_ADC_CHANNEL_NUM_FIELD       (GS358_ADC_CHANNEL_COUNT - 1U)

/* =========================== 全局状态 =========================== */

volatile uint16_t g_gs358_adc_values[GS358_ADC_CHANNEL_COUNT] = {0U};
volatile uint32_t g_gs358_adc_frame_count = 0U;
volatile uint32_t g_gs358_compare_edge_total = 0U;
volatile uint8_t g_gs358_light_present = 0U;

/* =========================== 内部状态 =========================== */

static volatile uint8_t s_edge_confirm_count = 0U;
static volatile uint16_t s_no_edge_time_ms = GS358_LIGHT_LOST_TIMEOUT_MS;
static volatile uint8_t s_adc_eoc_count = 0U;

static volatile GS358_LedMode s_led_mode = GS358_LED_BLOCKED_ON;

/* =========================== 内部函数声明 =========================== */

static void GS358_SysTickInit(void);
static void GS358_GPIOInit(void);
static void GS358_EXTIInit(void);
static void GS358_ADCInit(void);

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
    GS358_EXTIInit();
    GS358_ADCInit();

    s_edge_confirm_count = 0U;
    s_no_edge_time_ms = GS358_LIGHT_LOST_TIMEOUT_MS;
    g_gs358_light_present = 0U;

    /* 上电默认按“遮光/无光”状态输出。 */
    GS358_ApplyOutputs();
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
                          RCC_AHBPERIPH_GPIOB, ENABLE);

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
    gpio_init.GPIO_Pin = GS358_OUTPUT_NC_PIN |
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
        output_level = (logical_active != 0U) ? Bit_SET : Bit_RESET;
    }
    else
    {
        output_level = (logical_active != 0U) ? Bit_RESET : Bit_SET;
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

    /*
     * 输出语义：
     *   遮光/无光：NO 动作，NC 释放
     *   有光：     NO 释放，NC 动作
     *
     * 若最终整机定义相反，只需要交换下面两行，或调整 *_ACTIVE_HIGH 宏。
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

/* =========================== PA7 外部中断 =========================== */

static void GS358_EXTIInit(void)
{
    EXTI_InitTypeDef exti_init;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SYSCFG, ENABLE);

    /* 将 EXTI7 映射到 PA7。 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);

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

    /* 每个有效下降沿都重新开始“丢光”计时。 */
    s_no_edge_time_ms = 0U;

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

/* =========================== 1 ms 时间基准 =========================== */

static void GS358_SysTickInit(void)
{
    RCC_ClocksTypeDef clocks;

    RCC_GetClocksFreq(&clocks);

    if (SysTick_Config(clocks.HCLK_Frequency / 1000U) != 0U)
    {
        while (1)
        {
            /* SysTick 配置失败。 */
        }
    }

    /*
     * Cortex-M0 仅有 2 位优先级。
     * EXTI=0，ADC=1，SysTick=2。
     */
    NVIC_SetPriority(SysTick_IRQn, 2U);
}

void GS358_1msIRQHandler(void)
{
    if (s_no_edge_time_ms < GS358_LIGHT_LOST_TIMEOUT_MS)
    {
        s_no_edge_time_ms++;
    }

    if (s_no_edge_time_ms >= GS358_LIGHT_LOST_TIMEOUT_MS)
    {
        /*
         * 超时没有下降沿，认为已遮光/无光。
         * 清除确认计数，下一次必须重新累计指定次数。
         */
        s_edge_confirm_count = 0U;
        GS358_SetLightState(0U);
    }
}

/* =========================== ADC 五通道扫描 =========================== */

static void GS358_ADCInit(void)
{
    ADC_InitTypeDef adc_init;
    volatile uint32_t startup_delay;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_ADC1, ENABLE);

    ADC_DeInit(ADC1);
    ADC_StructInit(&adc_init);

    adc_init.ADC_Resolution = ADC_Resolution_12b;
    adc_init.ADC_Prescaler = ADC_Prescaler_16;
    adc_init.ADC_Mode = ADC_Mode_Scan;

    /*
     * 软件启动时外部触发源字段不会实际触发转换，
     * 这里保留库要求的合法默认值。
     */
    adc_init.ADC_ExternalTrigConv = ADC_ExtTrig_T1_CC1;
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &adc_init);

    /*
     * 使用最长采样时间，提高对板上分压、电流检测和偏置节点的兼容性。
     * 后续确认源阻抗较低后，可以缩短以提高帧率。
     */
    ADC_SampleTimeConfig(ADC1, ADC_SampleTime_240_5);

    /* 固定扫描顺序：0 -> 1 -> 2 -> 3 -> 5。 */
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_0, ADC_Channel_0);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_1, ADC_Channel_1);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_2, ADC_Channel_2);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_3, ADC_Channel_3);
    ADC_AnyChannelSelect(ADC1, ADC_AnyChannel_4, ADC_Channel_5);
    ADC_AnyChannelNumCfg(ADC1, GS358_ADC_CHANNEL_NUM_FIELD);
    ADC_AnyChannelCmd(ADC1, ENABLE);

    s_adc_eoc_count = 0U;
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    NVIC_ClearPendingIRQ(ADC1_IRQn);
    NVIC_SetPriority(ADC1_IRQn, 1U);
    NVIC_EnableIRQ(ADC1_IRQn);

    ADC_Cmd(ADC1, ENABLE);

    /* 手册要求 ADC 上电后等待约 200 ns；这里留出更宽裕的软件延时。 */
    for (startup_delay = 0U; startup_delay < 64U; startup_delay++)
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
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

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
                ADC_GetChannelConvertedValue(ADC1, ADC_Channel_0);
            g_gs358_adc_values[1] =
                ADC_GetChannelConvertedValue(ADC1, ADC_Channel_1);
            g_gs358_adc_values[2] =
                ADC_GetChannelConvertedValue(ADC1, ADC_Channel_2);
            g_gs358_adc_values[3] =
                ADC_GetChannelConvertedValue(ADC1, ADC_Channel_3);
            g_gs358_adc_values[4] =
                ADC_GetChannelConvertedValue(ADC1, ADC_Channel_5);

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
     *   g_gs358_adc_values[0] = ADC_IN0
     *   g_gs358_adc_values[1] = ADC_IN1
     *   g_gs358_adc_values[2] = ADC_IN2
     *   g_gs358_adc_values[3] = ADC_IN3
     *   g_gs358_adc_values[4] = ADC_IN5
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
     *   1. 关闭 PA11 普通 GPIO 输出；
     *   2. GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_3)；
     *   3. PA11 配置为 GPIO_Mode_AF_PP；
     *   4. 初始化 TIM14 周期、比较值及输出极性；
     *   5. 启动 TIM14。
     *
     * USER CODE END PWM_RESERVED
     */
}

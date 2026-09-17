/************************************************************************************************/
/**
 * @file    common.c
 * @brief   槽型开关 48MHz 时钟、NO/NC/LED GPIO 和 ADC 中断采集初始化
 ************************************************************************************************/

#include "common.h"

/*----------------------------------------调试变量----------------------------------------------*/
/* 可在 Keil Watch 窗口直接观察最新 ADC 原始值、采样次数和溢出次数。 */
__IO uint16_t g_adc_value = 0U;
__IO uint32_t g_adc_sample_count = 0U;
__IO uint32_t g_adc_overrun_count = 0U;

/*-----------------------------------------内部函数---------------------------------------------*/
static void gpio_write_level(GPIO_t *gpio_port, uint32_t pin, uint8_t high_level)
{
    if(high_level != 0U)
    {
        std_gpio_set_pin(gpio_port, pin);
    }
    else
    {
        std_gpio_reset_pin(gpio_port, pin);
    }
}

static void gpio_write_active_state(GPIO_t *gpio_port,
                                    uint32_t pin,
                                    uint8_t active,
                                    uint8_t active_level)
{
    uint8_t high_level;

    if(active != 0U)
    {
        high_level = active_level;
    }
    else
    {
        high_level = (uint8_t)(active_level == IO_ACTIVE_LOW);
    }

    gpio_write_level(gpio_port, pin, high_level);
}

static void adc_software_calibrate(void)
{
    int32_t calibration_factor;

    /* 参考官方 ADC 中断例程：ADC 每次复位后校准一次。 */
    std_adc_calibration_enable();
    while(std_adc_get_flag(ADC_FLAG_EOCAL) == 0U)
    {
        /* 仅在上电初始化阶段等待校准完成，不是 ADC 采集轮询。 */
    }

    std_adc_clear_flag(ADC_FLAG_ALL);

    calibration_factor = std_adc_get_calibration_factor();
    if((calibration_factor & ADC_CALFACT_CALFACT_5) != 0)
    {
        calibration_factor |= (int32_t)0xFFFFFFE0;
    }

    calibration_factor -= (*(int32_t *)(0x1FFF03CC));
    if(calibration_factor > 31)
    {
        calibration_factor = 31;
    }
    else if(calibration_factor < -31)
    {
        calibration_factor = -31;
    }
    else
    {
        /* 校准值在允许范围内，无需修正。 */
    }

    std_adc_calibration_factor_config(calibration_factor);
}

/*-----------------------------------------外部函数---------------------------------------------*/
void system_clock_config(void)
{
    /* HCLK > 24MHz 时，Flash 必须配置 1 个等待周期。 */
    std_flash_set_latency(FLASH_LATENCY_1CLK);

    /* RCH=48MHz；SYSCLK/HCLK/PCLK 均不分频，以最高主频运行。 */
    std_rcc_rch_enable();
    while(std_rcc_get_rch_ready() != RCC_CSR1_RCHRDY)
    {
    }

    std_rcc_set_sysclk_source(RCC_SYSCLK_SRC_RCH);
    while(std_rcc_get_sysclk_source() != RCC_SYSCLK_SRC_STATUS_RCH)
    {
    }

    std_rcc_set_ahbdiv(RCC_HCLK_DIV1);
    std_rcc_set_apbdiv(RCC_PCLK_DIV1);
    SystemCoreClock = RCH_VALUE;
}

void no_output_set(uint8_t active)
{
#if (NO_OUTPUT_FUNCTION_ENABLE == FUNCTION_ENABLE)
    gpio_write_active_state(NO_OUTPUT_GPIO_PORT,
                            NO_OUTPUT_PIN,
                            active,
                            NO_OUTPUT_ACTIVE_LEVEL);
#else
    (void)active;
    gpio_write_active_state(NO_OUTPUT_GPIO_PORT,
                            NO_OUTPUT_PIN,
                            0U,
                            NO_OUTPUT_ACTIVE_LEVEL);
#endif
}

void nc_output_set(uint8_t active)
{
#if (NC_OUTPUT_FUNCTION_ENABLE == FUNCTION_ENABLE)
    gpio_write_active_state(NC_OUTPUT_GPIO_PORT,
                            NC_OUTPUT_PIN,
                            active,
                            NC_OUTPUT_ACTIVE_LEVEL);
#else
    (void)active;
    gpio_write_active_state(NC_OUTPUT_GPIO_PORT,
                            NC_OUTPUT_PIN,
                            0U,
                            NC_OUTPUT_ACTIVE_LEVEL);
#endif
}

void led_set(uint8_t on)
{
#if (LED_FUNCTION_ENABLE == FUNCTION_ENABLE)
    gpio_write_active_state(LED_GPIO_PORT, LED_PIN, on, LED_ACTIVE_LEVEL);
#else
    (void)on;
    gpio_write_active_state(LED_GPIO_PORT, LED_PIN, 0U, LED_ACTIVE_LEVEL);
#endif
}

void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};

    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);

    /* 先写入安全的上电状态，再切换成输出模式，减小 GPIO 毛刺。 */
    no_output_set(NO_OUTPUT_POWER_ON_STATE);
    nc_output_set(NC_OUTPUT_POWER_ON_STATE);
    led_set(LED_POWER_ON_STATE);

    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;

    gpio_config.pin = NC_OUTPUT_PIN | LED_PIN;
    std_gpio_init(GPIOA, &gpio_config);

    gpio_config.pin = NO_OUTPUT_PIN;
    std_gpio_init(GPIOB, &gpio_config);

    /* PB1/ADC_IN0 使用模拟模式，无上下拉。 */
    gpio_config.pin = ADC_INPUT_PIN;
    gpio_config.mode = GPIO_MODE_ANALOG;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;
    std_gpio_init(ADC_INPUT_GPIO_PORT, &gpio_config);
}

void tim1_init(void)
{
    std_tim_basic_init_t timer_config = {0};

    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_TIM1);

    /*
     * TIM1_KCLK=PCLK=48MHz。
     * PSC=47 -> 48MHz/(47+1)=1MHz，即计数器每计数一次为 1us。
     * ARR=N-1 -> 每 N us 产生一次更新事件/TRGO。
     */
    timer_config.prescaler = 47U;
    timer_config.counter_mode = TIM_COUNTER_MODE_UP;
    timer_config.period = ADC_SAMPLE_PERIOD_US - 1U;
    timer_config.clock_div = TIM_CLOCK_DTS_DIV1;
    timer_config.repeat_counter = 0U;
    std_tim_init(TIM1, &timer_config);

    std_tim_clear_flag(TIM1, TIM_FLAG_UPDATE);
    std_tim_trigout_mode_config(TIM1, TIM_TRIG_OUT_UPDATE);
}

void adc_init(void)
{
    std_rcc_apb2_clk_enable(RCC_PERIPH_CLK_ADC);

    /* PCLK=48MHz，3 分频后 ADC_CK=16MHz（5V 供电允许的最高 ADC 时钟）。 */
    std_adc_clock_config(ADC_CK_DIV3);
    /* TIM1 更新事件的 TRGO 上升沿触发 ADC。 */
    std_adc_trig_config(ADC_TRIG_HW_EDGE_RISING, ADC_EXTRIG_TIM1_TRGO);

    /* 间断模式下，每个 TIM1 触发沿只转换一次当前通道。 */
    std_adc_conversion_mode_config(ADC_DISCONTINUOUS_CONVER_MODE);
    /* 若 CPU 来不及读取，保留最新采样值，并通过 OVRN 计数暴露丢样情况。 */
    std_adc_ovrn_mode_config(ADC_OVRN_MODE_OVERWRITTEN);

    /* 最短采样 3 周期；总转换时间=(3+13)/16MHz=1us，理论 1Msps。 */
    std_adc_sampt_time_config(ADC_SAMPTIME_3CYCLES);
    std_adc_fix_sequence_channel_enable(ADC_INPUT_CHANNEL);

    /* 定时器决定采样节拍，不启用自动等待模式。 */
    std_adc_wait_mode_disable();
    std_adc_enable();
    std_delayus(ADC_EN_DELAY);

    adc_software_calibrate();
    std_adc_clear_flag(ADC_FLAG_ALL);

    NVIC_SetPriority(ADC_COMP_IRQn, NVIC_PRIO_0);
    NVIC_EnableIRQ(ADC_COMP_IRQn);

    std_adc_interrupt_enable(ADC_INTERRUPT_EOC);
    std_adc_interrupt_enable(ADC_INTERRUPT_OVRN);
}

void adc_start(void)
{
    /* 先使 ADC 等待硬件触发，再启动 TIM1，避免丢失第一个触发沿。 */
    std_adc_start_conversion();
    std_tim_enable(TIM1);
}

void ADC_COMP_IRQHandler(void)
{
    if(std_adc_get_flag(ADC_FLAG_OVRN) != 0U)
    {
        std_adc_clear_flag(ADC_FLAG_OVRN);
        g_adc_overrun_count++;
    }

    if(std_adc_get_flag(ADC_FLAG_EOC) != 0U)
    {
        std_adc_clear_flag(ADC_FLAG_EOC);
        g_adc_value = (uint16_t)std_adc_get_conversion_value();
        g_adc_sample_count++;
    }
}

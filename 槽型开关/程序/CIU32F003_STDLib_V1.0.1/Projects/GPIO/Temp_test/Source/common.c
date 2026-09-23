/************************************************************************************************/
/**
 * @file    common.c
 * @brief   槽型开关完整测试程序：48MHz、TIM1触发ADC、中断采集、阈值及互补输出
 ************************************************************************************************/

#include "common.h"

/*==============================================================================================*/
/* 1. Keil Watch调试变量                                                                        */
/*==============================================================================================*/
__IO uint16_t g_adc_value = 0U;             // ADC原始值，可以看到电机干扰脉冲
__IO uint16_t g_adc_filtered_value = 0U;    // 滤波后的真实电平，实际用于阈值判断      // 滤波后的真实电平，实际用于阈值判断
__IO uint16_t g_adc_window_min = 0U;        // 当前9点中的最低值
__IO uint16_t g_adc_window_max = 0U;        // 当前9点中的最高值
__IO uint8_t g_adc_filter_ready = 0U;       // 9点收满后变为1
__IO uint32_t g_adc_sample_count = 0U;      // 当前检测状态
__IO uint32_t g_adc_overrun_count = 0U;
__IO uint32_t g_threshold_switch_count = 0U;

__IO uint8_t g_detection_state = DETECTION_POWER_ON_STATE;
__IO uint8_t g_output_channels_enabled = OUTPUT_CHANNELS_POWER_ON_ENABLE;
__IO uint8_t g_led_enabled = LED_POWER_ON_ENABLE;

/*==============================================================================================*/
/* 2. 内部GPIO辅助函数                                                                          */
/*==============================================================================================*/
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

static void detection_outputs_apply(void)
{
    uint8_t detection_active = (uint8_t)(g_detection_state != STATE_OFF);

    if(g_output_channels_enabled != FUNCTION_DISABLE)
    {
        /* 正常工作时NO和NC始终相反。 */
        no_output_set(detection_active);
        nc_output_set((uint8_t)(detection_active == STATE_OFF));
    }
    else
    {
        /* 整体关闭输出通道时，两个通道都输出各自的无效电平。 */
        no_output_set(STATE_OFF);
        nc_output_set(STATE_OFF);
    }

    if(g_led_enabled != FUNCTION_DISABLE)
    {
        led_set(detection_active);
    }
    else
    {
        led_set(STATE_OFF);
    }
}

/*==============================================================================================*/
/* 3. ADC校准                                                                                   */
/*==============================================================================================*/
static void adc_software_calibrate(void)
{
    int32_t calibration_factor;

    std_adc_calibration_enable();
    while(std_adc_get_flag(ADC_FLAG_EOCAL) == 0U)
    {
        /* 只在上电初始化阶段等待校准完成，不参与正常ADC采集。 */
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
        /* 校准值在允许范围内。 */
    }

    std_adc_calibration_factor_config(calibration_factor);
}

/*==============================================================================================*/
/* 4. 系统与外设初始化                                                                          */
/*==============================================================================================*/
void system_clock_config(void)
{
    /* HCLK超过24MHz时，Flash必须配置1个等待周期。 */
    std_flash_set_latency(FLASH_LATENCY_1CLK);

    /* RCH=48MHz，SYSCLK/HCLK/PCLK均不分频。 */
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

void gpio_init(void)
{
    std_gpio_init_t gpio_config = {0};

    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOA);
    std_rcc_gpio_clk_enable(RCC_PERIPH_CLK_GPIOB);

    /* 先把输出数据寄存器写成正确的上电状态，再切换GPIO模式，减小输出毛刺。 */
    detection_outputs_apply();

    gpio_config.mode = GPIO_MODE_OUTPUT;
    gpio_config.pull = GPIO_NOPULL;
    gpio_config.output_type = GPIO_OUTPUT_PUSHPULL;

    gpio_config.pin = NC_OUTPUT_PIN | LED_PIN;
    std_gpio_init(GPIOA, &gpio_config);

    gpio_config.pin = NO_OUTPUT_PIN;
    std_gpio_init(GPIOB, &gpio_config);

    /* PB1对应ADC_IN0。 */
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
     * PSC=47：48MHz/(47+1)=1MHz，每个计数单位为1us。
     * ARR=N-1：每N us产生一次更新事件，并通过TRGO触发ADC。
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

    /* PCLK=48MHz，三分频后ADC_CK=16MHz。 */
    std_adc_clock_config(ADC_CK_DIV3);

    /* TIM1更新事件的TRGO上升沿触发ADC。 */
    std_adc_trig_config(ADC_TRIG_HW_EDGE_RISING, ADC_EXTRIG_TIM1_TRGO);

    /* 每次定时器触发只转换一次PB1/ADC_IN0。 */
    std_adc_conversion_mode_config(ADC_DISCONTINUOUS_CONVER_MODE);
    std_adc_fix_sequence_channel_enable(ADC_INPUT_CHANNEL);

    /* 最短采样3周期，总转换时间=(3+13)/16MHz=1us。 */
    std_adc_sampt_time_config(ADC_SAMPTIME_3CYCLES);

    /* 溢出时保留最新值，并记录溢出次数。 */
    std_adc_ovrn_mode_config(ADC_OVRN_MODE_OVERWRITTEN);
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

/*==============================================================================================*/
/* 5. 输出控制接口                                                                              */
/*==============================================================================================*/
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
                            STATE_OFF,
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
                            STATE_OFF,
                            NC_OUTPUT_ACTIVE_LEVEL);
#endif
}

void led_set(uint8_t on)
{
#if (LED_FUNCTION_ENABLE == FUNCTION_ENABLE)
    gpio_write_active_state(LED_GPIO_PORT, LED_PIN, on, LED_ACTIVE_LEVEL);
#else
    (void)on;
    gpio_write_active_state(LED_GPIO_PORT, LED_PIN, STATE_OFF, LED_ACTIVE_LEVEL);
#endif
}

void output_channels_enable(uint8_t enable)
{
    g_output_channels_enabled = (uint8_t)(enable != FUNCTION_DISABLE);
    detection_outputs_apply();
}

void led_enable(uint8_t enable)
{
    g_led_enabled = (uint8_t)(enable != FUNCTION_DISABLE);
    detection_outputs_apply();
}

void detection_state_set(uint8_t detection_state)
{
    g_detection_state = (uint8_t)(detection_state != STATE_OFF);
    detection_outputs_apply();
}

/*==============================================================================================*/
/* 6. 抗脉冲滤波、ADC阈值及互补输出                                                     */
/*==============================================================================================*/
static uint16_t s_adc_filter_buffer[ADC_FILTER_WINDOW_SIZE];
static uint8_t s_adc_filter_index = 0U;
static uint8_t s_adc_filter_count = 0U;

static uint8_t adc_pulse_filter_update(uint16_t adc_value)
{
    uint16_t sorted[ADC_FILTER_WINDOW_SIZE];
    uint16_t temp;
    uint32_t sum = 0U;
    uint8_t i;
    uint8_t j;

    s_adc_filter_buffer[s_adc_filter_index] = adc_value;
    s_adc_filter_index++;
    if(s_adc_filter_index >= ADC_FILTER_WINDOW_SIZE)
    {
        s_adc_filter_index = 0U;
    }

    if(s_adc_filter_count < ADC_FILTER_WINDOW_SIZE)
    {
        s_adc_filter_count++;
        if(s_adc_filter_count < ADC_FILTER_WINDOW_SIZE)
        {
            return 0U;
        }
    }

    /* 9点插入排序。 */
    for(i = 0U; i < ADC_FILTER_WINDOW_SIZE; i++)
    {
        sorted[i] = s_adc_filter_buffer[i];
    }

    for(i = 1U; i < ADC_FILTER_WINDOW_SIZE; i++)
    {
        temp = sorted[i];
        j = i;
        while((j > 0U) && (sorted[j - 1U] > temp))
        {
            sorted[j] = sorted[j - 1U];
            j--;
        }
        sorted[j] = temp;
    }

    /* 去掉最低2点和最高2点，只平均中间5点。 */
    for(i = ADC_FILTER_TRIM_COUNT;
        i < (ADC_FILTER_WINDOW_SIZE - ADC_FILTER_TRIM_COUNT);
        i++)
    {
        sum += sorted[i];
    }

    g_adc_filtered_value = (uint16_t)(sum /
        (ADC_FILTER_WINDOW_SIZE - (2U * ADC_FILTER_TRIM_COUNT)));
    g_adc_filter_ready = 1U;

#if (ADC_FILTER_DEBUG_ENABLE == FUNCTION_ENABLE)
    g_adc_window_min = sorted[0];
    g_adc_window_max = sorted[ADC_FILTER_WINDOW_SIZE - 1U];
#endif

    return 1U;
}

void slot_switch_adc_process(uint16_t adc_value)
{
#if (ADC_THRESHOLD_CONTROL_ENABLE == FUNCTION_ENABLE)
    uint16_t value_for_judgement;

#if (ADC_PULSE_FILTER_ENABLE == FUNCTION_ENABLE)
    if(adc_pulse_filter_update(adc_value) == 0U)
    {
        return;
    }
    value_for_judgement = g_adc_filtered_value;
#else
    g_adc_filtered_value = adc_value;
    g_adc_filter_ready = 1U;
    value_for_judgement = adc_value;
#endif

    if((g_detection_state == STATE_OFF) &&
       (value_for_judgement > ADC_SWITCH_THRESHOLD))
    {
        g_threshold_switch_count++;
        detection_state_set(STATE_ON);
    }
    else if((g_detection_state != STATE_OFF) &&
            (value_for_judgement <
             (ADC_SWITCH_THRESHOLD - ADC_SWITCH_HYSTERESIS)))
    {
        g_threshold_switch_count++;
        detection_state_set(STATE_OFF);
    }
    else
    {
    }
#else
    (void)adc_value;
#endif
}

/*==============================================================================================*/
/* 7. ADC采样启动、停止及中断                                                                   */
/*==============================================================================================*/
void adc_start(void)
{
    /* 先让ADC进入等待硬件触发状态，再启动TIM1。 */
    std_adc_start_conversion();
    std_tim_enable(TIM1);
}

void adc_stop(void)
{
    std_tim_disable(TIM1);
    std_adc_stop_conversion();
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

        /* 新增阈值功能在这里执行，主循环不轮询ADC。 */
        slot_switch_adc_process(g_adc_value);
    }
}

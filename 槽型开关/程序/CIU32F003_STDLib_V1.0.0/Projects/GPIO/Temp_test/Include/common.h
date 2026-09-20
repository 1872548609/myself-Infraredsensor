/************************************************************************************************/
/**
 * @file    common.h
 * @brief   槽型开关完整测试程序配置与接口
 ************************************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ciu32f003_std.h"

/*==============================================================================================*/
/* 1. 通用开关值                                                                                */
/*==============================================================================================*/
#define FUNCTION_DISABLE                    (0U)
#define FUNCTION_ENABLE                     (1U)

#define IO_ACTIVE_LOW                       (0U)
#define IO_ACTIVE_HIGH                      (1U)

#define STATE_OFF                           (0U)
#define STATE_ON                            (1U)

/*==============================================================================================*/
/* 2. 用户功能配置：通常只需要修改本区域                                                         */
/*==============================================================================================*/

/* NO、NC、LED功能总开关。关闭后，对应引脚始终输出无效电平。 */
#define NO_OUTPUT_FUNCTION_ENABLE           FUNCTION_ENABLE
#define NC_OUTPUT_FUNCTION_ENABLE           FUNCTION_ENABLE
#define LED_FUNCTION_ENABLE                 FUNCTION_ENABLE

/* 运行时功能的上电默认使能状态。 */
#define OUTPUT_CHANNELS_POWER_ON_ENABLE     FUNCTION_ENABLE
#define LED_POWER_ON_ENABLE                 FUNCTION_ENABLE

/* MCU引脚的有效电平，可分别修改为IO_ACTIVE_HIGH或IO_ACTIVE_LOW。 */
#define NO_OUTPUT_ACTIVE_LEVEL              IO_ACTIVE_HIGH
#define NC_OUTPUT_ACTIVE_LEVEL              IO_ACTIVE_HIGH
#define LED_ACTIVE_LEVEL                    IO_ACTIVE_HIGH

/* 上电检测状态：0=NO无效/NC有效/LED灭，1=NO有效/NC无效/LED亮。 */
#define DETECTION_POWER_ON_STATE            STATE_OFF

/* 定时器触发ADC的周期，单位us；默认10us，即100kS/s。 */
#define ADC_SAMPLE_PERIOD_US                (10U)

/*----------------------------------------------------------------------------------------------*/
/* 新增功能：ADC阈值自动切换NO、NC和LED                                                         */
/*----------------------------------------------------------------------------------------------*/
#define ADC_THRESHOLD_CONTROL_ENABLE        FUNCTION_ENABLE

/* 12位ADC范围为0~4095。ADC大于阈值时切换为检测状态1。 */
#define ADC_SWITCH_THRESHOLD                (2048U)

/* ADC小于“阈值-回差”时恢复为检测状态0。 */
#define ADC_SWITCH_HYSTERESIS               (50U)

/*==============================================================================================*/
/* 3. 配置合法性检查                                                                            */
/*==============================================================================================*/
#if ((ADC_SAMPLE_PERIOD_US < 2U) || (ADC_SAMPLE_PERIOD_US > 65536U))
#error "ADC_SAMPLE_PERIOD_US must be between 2us and 65536us"
#endif

#if (ADC_SWITCH_THRESHOLD > 4095U)
#error "ADC_SWITCH_THRESHOLD must be between 0 and 4095"
#endif

#if (ADC_SWITCH_HYSTERESIS > ADC_SWITCH_THRESHOLD)
#error "ADC_SWITCH_HYSTERESIS must not exceed ADC_SWITCH_THRESHOLD"
#endif

#if ((DETECTION_POWER_ON_STATE != STATE_OFF) && (DETECTION_POWER_ON_STATE != STATE_ON))
#error "DETECTION_POWER_ON_STATE must be STATE_OFF or STATE_ON"
#endif

/* 兼容此前版本的上电状态名称；三个状态由检测状态统一产生，保证NO和NC互补。 */
#define NO_OUTPUT_POWER_ON_STATE            (DETECTION_POWER_ON_STATE)
#define NC_OUTPUT_POWER_ON_STATE            ((uint8_t)(DETECTION_POWER_ON_STATE == STATE_OFF))
#define LED_POWER_ON_STATE                  (DETECTION_POWER_ON_STATE)

/*==============================================================================================*/
/* 4. 原理图引脚定义                                                                            */
/*==============================================================================================*/
#define NC_OUTPUT_GPIO_PORT                 GPIOA
#define NC_OUTPUT_PIN                       GPIO_PIN_0

#define NO_OUTPUT_GPIO_PORT                 GPIOB
#define NO_OUTPUT_PIN                       GPIO_PIN_0

#define LED_GPIO_PORT                       GPIOA
#define LED_PIN                             GPIO_PIN_1

#define ADC_INPUT_GPIO_PORT                 GPIOB
#define ADC_INPUT_PIN                       GPIO_PIN_1
#define ADC_INPUT_CHANNEL                   ADC_CHANNEL_0

/*==============================================================================================*/
/* 5. Keil Watch调试变量                                                                        */
/*==============================================================================================*/
extern __IO uint16_t g_adc_value;
extern __IO uint32_t g_adc_sample_count;
extern __IO uint32_t g_adc_overrun_count;
extern __IO uint32_t g_threshold_switch_count;

extern __IO uint8_t g_detection_state;
extern __IO uint8_t g_output_channels_enabled;
extern __IO uint8_t g_led_enabled;

/*==============================================================================================*/
/* 6. 初始化与ADC采样控制接口                                                                   */
/*==============================================================================================*/
void system_clock_config(void);
void gpio_init(void);
void tim1_init(void);
void adc_init(void);
void adc_start(void);
void adc_stop(void);

/*==============================================================================================*/
/* 7. 输出控制接口                                                                              */
/*==============================================================================================*/
/* 底层单通道接口保留，1=有效，0=无效。 */
void no_output_set(uint8_t active);
void nc_output_set(uint8_t active);
void led_set(uint8_t on);

/* 运行时整体使能接口。关闭输出通道时，NO和NC均进入无效状态。 */
void output_channels_enable(uint8_t enable);
void led_enable(uint8_t enable);

/* 设置检测状态；输出使能时自动保证NO和NC相反，LED跟随检测状态。 */
void detection_state_set(uint8_t detection_state);

/* ADC阈值处理函数，由ADC中断自动调用。 */
void slot_switch_adc_process(uint16_t adc_value);

void ADC_COMP_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */

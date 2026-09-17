/************************************************************************************************/
/**
 * @file    common.h
 * @brief   槽型开关硬件初始化与基础 IO/ADC 接口
 ************************************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ciu32f003_std.h"

/*---------------------------------------功能配置宏---------------------------------------------*/
#define FUNCTION_DISABLE                    (0U)
#define FUNCTION_ENABLE                     (1U)

#define IO_ACTIVE_LOW                       (0U)
#define IO_ACTIVE_HIGH                      (1U)

/*
 * 原理图对应关系：
 * PA0 -> IO_OUTPUT_NC，PB0 -> IO_OUTPUT_NO，PA1 -> IO_LED_RED。
 * 三路信号均为 MCU 高电平驱动外部三极管/LED，默认使用高电平有效。
 */
#define NO_OUTPUT_FUNCTION_ENABLE           FUNCTION_ENABLE
#define NC_OUTPUT_FUNCTION_ENABLE           FUNCTION_ENABLE
#define LED_FUNCTION_ENABLE                 FUNCTION_ENABLE

/* 修改以下宏即可分别改变 NO、NC 和 LED 的有效电平。 */
#define NO_OUTPUT_ACTIVE_LEVEL              IO_ACTIVE_HIGH
#define NC_OUTPUT_ACTIVE_LEVEL              IO_ACTIVE_HIGH
#define LED_ACTIVE_LEVEL                    IO_ACTIVE_HIGH

/* 上电默认状态：0=关闭，1=开启。 */
#define NO_OUTPUT_POWER_ON_STATE            (0U)
#define NC_OUTPUT_POWER_ON_STATE            (0U)
#define LED_POWER_ON_STATE                  (0U)

/*
 * ADC 定时采样周期，单位为 us。
 * TIM1 配置为 1MHz 计数频率，因此计数器每一格正好是 1us。
 * 当前 ADC 单次转换约需 1us，为避免触发重叠，周期不得小于 2us。
 */
#define ADC_SAMPLE_PERIOD_US                (10U)

#if ((ADC_SAMPLE_PERIOD_US < 2U) || (ADC_SAMPLE_PERIOD_US > 65536U))
#error "ADC_SAMPLE_PERIOD_US must be between 2us and 65536us"
#endif

/*-----------------------------------------引脚定义---------------------------------------------*/
#define NC_OUTPUT_GPIO_PORT                 GPIOA
#define NC_OUTPUT_PIN                       GPIO_PIN_0

#define NO_OUTPUT_GPIO_PORT                 GPIOB
#define NO_OUTPUT_PIN                       GPIO_PIN_0

#define LED_GPIO_PORT                       GPIOA
#define LED_PIN                             GPIO_PIN_1

#define ADC_INPUT_GPIO_PORT                 GPIOB
#define ADC_INPUT_PIN                       GPIO_PIN_1
#define ADC_INPUT_CHANNEL                   ADC_CHANNEL_0

/*----------------------------------------调试变量----------------------------------------------*/
extern __IO uint16_t g_adc_value;
extern __IO uint32_t g_adc_sample_count;
extern __IO uint32_t g_adc_overrun_count;

/*-----------------------------------------函数声明---------------------------------------------*/
void system_clock_config(void);
void gpio_init(void);
void tim1_init(void);
void adc_init(void);
void adc_start(void);

void no_output_set(uint8_t active);
void nc_output_set(uint8_t active);
void led_set(uint8_t on);

void ADC_COMP_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */

/***********************************************************************
 * Copyright (c) 2017 - 2021, Unicmicro Co.,Ltd .
 *
 * All rights reserved.
 *
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : Will
 * Version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/

#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"

/*
 * 系统主频：24 MHz
 *
 * PWM周期：
 * 24 MHz × 100 us = 2400
 *
 * PWM有效脉宽：
 * 24 MHz × 10 us = 240
 *
 * 占空比：
 * 10 us / 100 us = 10%
 */
#define SYSTEM_CLOCK_HZ           24000000UL
#define PWM_TX_PERIOD_US          100UL
#define PWM_TX_PULSE_US           5UL

#define PWM_TX_PERIOD_COUNT       \
    ((uint16_t)((SYSTEM_CLOCK_HZ / 1000000UL) * PWM_TX_PERIOD_US))

#define PWM_TX_PULSE_COUNT        \
    ((uint16_t)((SYSTEM_CLOCK_HZ / 1000000UL) * PWM_TX_PULSE_US))

void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);
void gtimer0_UECallBack(void);

void main(void)
{
    /*
     * 初始化内部RCH时钟。
     * system_init()配置系统运行在24 MHz。
     */
    clock_init(24000000UL);


    GPIO_Init();

    /*
     * 当前发射程序实际上不需要串口时，
     * 可以屏蔽uart_init()，减少无关资源占用。
     */
    uart_init();

    /*
     * PWM2输出参数：
     *
     * 周期计数：2400，对应100 us
     * 脉宽计数：240，对应10 us
     * 有效电平：LOW
     *
     * 最终波形：
     * 周期100 us，频率10 kHz；
     * 低电平有效10 us，占空比10%。
     */
    pwm2_init(PWM_TX_PERIOD_COUNT,
              PWM_TX_PULSE_COUNT,
              LOW);

    pwm2_start();

    while (1)
    {
    }
}

void GPIO_Init(void)
{
    /*
     * 红色指示灯：P1.3
     */
    gpio_init(P1_3);
    gpio_dir_set(P1_3, GPIO_DIR_OUT);
    gpio_dr_set(P1_3, GPIO_SR_HIGH);
    gpio_io_set(P1_3, GPIO_HIGH);

    /*
     * 红外脉冲发射端口：
     * P1.4复用为PWM2。
     */
    REG_P14_CFG = 0x02;
    gpio_sr_set(P1_4, GPIO_SR_HIGH);

    /*
     * 脉冲测试输入端口：P1.5
     */
    REG_P15_CFG = 0x00;

    gpio_init(P1_5);
    gpio_dir_set(P1_5, GPIO_DIR_IN);
    gpio_in_enable(P1_5, IN_ENABLE);
}
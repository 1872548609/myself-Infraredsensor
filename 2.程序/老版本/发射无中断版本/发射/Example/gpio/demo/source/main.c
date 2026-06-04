/***********************************************************************
 * Industrial Infrared Through-beam Transmitter
 * Platform : UM800Y / 24MHz
 * Mode     : Hardware PWM, no interrupt
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
 * 发射脚：旧代码使用 P1.4 PWM 输出。
 * 原理图里发射管驱动接在 MCU 控制脚后级三极管/MOS 驱动上。
 */
#define IR_TX_PWM_PIN               P1_4
#define IR_TX_LED_PIN               P1_3

/*
 * 沿用旧工程实测参数。
 * 如果示波器测得发射频率不合适，优先只改这两个参数。
 */
#define IR_PWM_PERIOD_COUNT         16000U
#define IR_PWM_DUTY_COUNT           400U

#define LED_ACTIVE_LEVEL            GPIO_HIGH
#define LED_INACTIVE_LEVEL          GPIO_LOW

void GPIO_Init(void);

void main(void)
{
    system_init();

    GPIO_Init();
    uart_init();

    /*
     * 真正无中断发射端。
     * PWM 由硬件输出，不依赖中断。
     */
    EA = 0;

    pwm2_init(IR_PWM_PERIOD_COUNT, IR_PWM_DUTY_COUNT, LOW);
    pwm2_start();

    gpio_io_set(IR_TX_LED_PIN, LED_ACTIVE_LEVEL);

    while(1)
    {
        /*
         * 发射端无需 CPU 参与。
         * 如后续要做低功耗或故障检测，可以在这里加轮询逻辑。
         */
    }
}

void GPIO_Init(void)
{
    /*
     * 发射指示灯。
     */
    REG_P13_CFG = 0x00;
    gpio_init(IR_TX_LED_PIN);
    gpio_dir_set(IR_TX_LED_PIN, GPIO_DIR_OUT);
    gpio_dr_set(IR_TX_LED_PIN, GPIO_SR_HIGH);
    gpio_io_set(IR_TX_LED_PIN, LED_INACTIVE_LEVEL);

    /*
     * P1.4 配置为 PWM2 输出。
     * 旧工程就是 REG_P14_CFG = 0x20。
     */
    REG_P14_CFG = 0x20;
    gpio_sr_set(IR_TX_PWM_PIN, GPIO_SR_HIGH);

    /*
     * P1.5 保留输入。
     */
    REG_P15_CFG = 0x00;
    gpio_init(P1_5);
    gpio_dir_set(P1_5, GPIO_DIR_IN);
    gpio_in_enable(P1_5, IN_ENABLE);
}
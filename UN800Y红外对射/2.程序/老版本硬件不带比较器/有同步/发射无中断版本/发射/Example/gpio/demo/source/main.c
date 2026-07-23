/***********************************************************************
 * 工业红外对射 - 发射端
 * 平台：UM800Y / 24MHz
 *
 * 功能：
 * 1. P1.4 输出红外发射 PWM
 * 2. 周期约 1ms，窄脉冲发射
 * 3. CPU 不参与发射时序，硬件 PWM 自动输出
 *
 * 引脚：
 * P1.4 -> 红外发射驱动
 * P1.3 -> 指示灯
 ***********************************************************************/

#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"

#define IR_TX_PWM_PIN      P1_4
#define IR_TX_LED_PIN      P1_3

/*
 * PWM 参数。
 *
 * 你旧版仓库里原来就是这组：
 * period = 16000
 * duty   = 400
 *
 * 如果示波器看到周期不是约 1ms，优先改 IR_PWM_PERIOD_COUNT。
 * 如果红外发射脉冲太窄/太弱，增大 IR_PWM_DUTY_COUNT。
 */
#define IR_PWM_PERIOD_COUNT    16000U
#define IR_PWM_DUTY_COUNT      400U

#define LED_ACTIVE_LEVEL       GPIO_HIGH
#define LED_INACTIVE_LEVEL     GPIO_LOW

void GPIO_Init(void);

void main(void)
{
    system_init();

    GPIO_Init();

    /*
     * 不需要串口时可以注释。
     * 如果 app.c 里的 uart_init() 会打开中断，建议关掉里面的 uart0_irq_init()。
     */
    uart_init();

    /*
     * 发射端不使用中断。
     * PWM 硬件自己跑。
     */
    EA = 0;

    /*
     * P1.4 -> PWM2。
     * 旧工程使用 pwm2_init(period, duty, LOW)。
     */
    pwm2_init(IR_PWM_PERIOD_COUNT, IR_PWM_DUTY_COUNT, LOW);
    pwm2_start();

    gpio_io_set(IR_TX_LED_PIN, LED_ACTIVE_LEVEL);

    while(1)
    {
        /*
         * 发射端无任务。
         * 红外脉冲由 PWM 硬件持续输出。
         */
    }
}

void GPIO_Init(void)
{
    /*
     * P1.3 指示灯
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
}
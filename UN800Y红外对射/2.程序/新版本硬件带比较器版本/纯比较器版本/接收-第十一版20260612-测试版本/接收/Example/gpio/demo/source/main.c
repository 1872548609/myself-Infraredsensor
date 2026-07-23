/***********************************************************************
 * 纯比较器接收版本：增加看门狗与中断置位
 *
 * 关键原则：
 * 1. 不在定时器中断里直接喂狗。
 * 2. GTIMER0 中断只产生“心跳”。
 * 3. 主循环确认心跳后才喂狗，同时监测主循环和定时器中断是否正常。
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"
#include "wdt.h"

#define APP_WDT_TIMEOUT    WDT_ARR_1024_MS

volatile uint16_t adc_data  = 0;
volatile uint16_t adc_data1 = 0;
volatile uint16_t adc_set   = 200;

/* 在中断和主程序之间共享的变量必须声明为 volatile */
volatile uint16_t status   = 0;
volatile uint16_t adcount  = 0;
volatile uint16_t adcount1 = 0;

/*中断产生心跳，主循环消费心跳后才能喂狗 */
volatile uint8_t g_wdt_run_heartbeat = 0;

/* 便于调试：1 表示上一次复位由 WDT 溢出引起 */
volatile uint8_t g_wdt_reset_detected = 0;

void gtimer0_UECallBack(void);
void gtimer1_UECallBack(void);
void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);

void main(void)
{
    system_init();

    /*
     * WDT 复位后 WDOF 会置 1。
     * 先保存复位原因，再清标志，便于在 Keil Watch 中观察。
     */
    g_wdt_reset_detected = WDOF;
    WDOF = 0;

    /*
     * 尽早启动 WDT，使后续外设初始化异常也能够恢复。
     * wdt_load()/wdt_feed() 对 RSTSTAT 的读写会同时清除 WDT 计数。
     */
    wdt_init();
    wdt_load(APP_WDT_TIMEOUT);

    GPIO_Init();
    wdt_feed(APP_WDT_TIMEOUT);

    uart_init();
    wdt_feed(APP_WDT_TIMEOUT);

    gtimer0_count_init(2200, 24 - 1);
    gtimer0_irq_init(GTIMER_IRQ_ENABLE,
                     GTIMER0_UIE_IRQ,
                     gtimer0_UECallBack);
    gtimer0_start();
    wdt_feed(APP_WDT_TIMEOUT);

    adc_clk_config(ADC_CLKSOURCE_SYSCLK,
                   ADC_VREFSOURCE_AVDD33,
                   4,
                   ADC_ENABLE);
    adc_sample_clk_config(ADC_SAMPCLK_4);
    adc_io_config(ADC_CHANNEL_1 | ADC_CHANNEL_2);
    adc_scan_mode_config(ADC_MODE_SINGLE);
    adc_power_config(ADC_ENABLE);
    adc_controller_config(ADC_ENABLE);
    wdt_feed(APP_WDT_TIMEOUT);

    while (1)
    {
        /*
         * 只有以下两个条件同时成立才会持续喂狗：
         * 1. 主循环仍在运行；
         * 2. GTIMER0 周期中断仍在运行。
         *
         * 如果主循环卡死、GPIO 中断风暴长期占用 CPU、
         * GTIMER0 停止或程序跑飞，均不会继续喂狗。
         */
        if (g_wdt_run_heartbeat != 0)
        {
            g_wdt_run_heartbeat = 0;
            wdt_feed(APP_WDT_TIMEOUT);
        }
    }
}

void gtimer0_UECallBack(void)
{
    /* 只产生健康心跳，不在中断内直接喂狗 */
    g_wdt_run_heartbeat = 1;

    adcount = 0;

    gpio_io_set(P1_2, GPIO_LOW);
    gpio_io_set(P1_3, GPIO_LOW);
    gpio_io_set(P1_0, GPIO_HIGH);     /* 常闭 */
}

void GPIO_IRQHandler(void) interrupt 0
{
    if (gpio_irq_get(P1_4))
    {
        static uint8_t ir_wdt_feed_count = 0;
        /*
         * 先清中断标志。
         * 若 ISR 执行期间又出现新边沿，硬件可重新置位，
         * 避免在函数末尾把新到的边沿一起清掉。
         */
        gpio_irq_clr(P1_4);

        REG_GTIM0_CR0 &= ~(1 << 0);

        /*
         * 连续有信号时，GTIMER0 会不断清零，
         * 所以必须由 GPIO 中断提供运行心跳。
         */
        ir_wdt_feed_count++;
        if(ir_wdt_feed_count==100)
        {
            ir_wdt_feed_count = 0;
            
            g_wdt_run_heartbeat = 1;
        }
        
        /*
         * 当前纯比较器版本等价于原来的 if(1)，
         * 即收到 P1.4 有效边沿就直接计数。
         */
        REG_GTIM0_CNT0 = 0;
        REG_GTIM0_CNT1 = 0;

        adcount++;

        /*
         * 使用 >= 而不是 ==，即使计数值因异常超过 2，
         * 也可以回到正常状态，不会永久失去输出切换。
         */
        if (adcount >= 2)
        {
            gpio_io_set(P1_2, GPIO_HIGH);
            gpio_io_set(P1_3, GPIO_HIGH);
            gpio_io_set(P1_0, GPIO_LOW);

            adcount = 0;
        }

        REG_GTIM0_CR0 |= (1 << 0);
    }
}

void gpio_UECallBack(void)
{
    /* 不用二次回调 */
}

void GPIO_Init(void)
{
    REG_P10_CFG = 0x00;
    gpio_init(P1_0);
    gpio_dir_set(P1_0, GPIO_DIR_OUT);
    gpio_dr_set(P1_0, GPIO_SR_HIGH);
    gpio_io_set(P1_0, GPIO_HIGH);

    REG_P12_CFG = 0x00;
    gpio_init(P1_2);
    gpio_dir_set(P1_2, GPIO_DIR_OUT);
    gpio_dr_set(P1_2, GPIO_SR_HIGH);
    gpio_io_set(P1_2, GPIO_HIGH);

    REG_P13_CFG = 0x00;
    gpio_init(P1_3);
    gpio_dir_set(P1_3, GPIO_DIR_OUT);
    gpio_dr_set(P1_3, GPIO_SR_HIGH);
    gpio_io_set(P1_3, GPIO_LOW);

    gpio_init(P1_4);
    gpio_dir_set(P1_4, GPIO_DIR_IN);
    gpio_dr_set(P1_4, GPIO_SR_HIGH);
    gpio_in_enable(P1_4, IN_ENABLE);
    gpio_irq_set(P1_4, GPIO_IRQ_ENABLE, gpio_UECallBack);

    P1AH &= ~(0x02);
    P1AH |=  (0x01);
    REG_P14_CFG = 0x20;

    REG_P15_CFG = 0x00;
    gpio_init(P1_5);
    gpio_dir_set(P1_5, GPIO_DIR_IN);
    gpio_in_enable(P1_5, IN_ENABLE);
}

/************************************************************************************************/
/**
 * @file    main.c
 * @brief   槽型开关初始硬件测试程序
 ************************************************************************************************/

#include "main.h"

int main(void)
{
    system_clock_config();
    std_delay_init();

    gpio_init();
    tim1_init();
    adc_init();
    adc_start();

    while(1)
    {
        /* ADC 数据只在 ADC_COMP_IRQHandler() 内更新，主循环不轮询 ADC。 */
        __WFI();
    }
}

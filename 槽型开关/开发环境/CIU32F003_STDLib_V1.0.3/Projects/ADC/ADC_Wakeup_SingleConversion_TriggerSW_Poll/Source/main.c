/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示通过EXTI（按键）方式退出DeepStop，软件触发ADC单次扫描转换，配置2个通道
*                     (外部通道PA7和内部通道VBGR),进入Stop低功耗模式等待EXTI触发唤醒，唤醒CPU后，ADC采样10次
*                     (根据实际需求可调整采样次数)，再进行数据平均，提升采样精度，使用VBGR平均采样值校准
*                     ADC当前的参考电压，最后将外部通道采样平均值转换成电压值，通过串口打印输出。 
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/


/*------------------------------------------includes--------------------------------------------*/
#include "main.h"

/*-------------------------------------------variables------------------------------------------*/
uint32_t g_voltage_buf[ADC_CHANNEL_NUM];

/*-------------------------------------------functions------------------------------------------*/
int main(void)
{    
    /* 配置系统时钟 */
    system_clock_config();

    /* Systick初始化 */
    std_delay_init();
    
    /* GPIO初始化 */
    gpio_init();
    
    /* UART初始化 */
    uart_init();
    
    /* ADC初始化 */
    adc_init();
       
    /* NVIC初始化 */
    nvic_init(); 
    
    /* 提升ADC校准系数精度，系统复位后只需执行一次 */ 
    bsp_adc_software_calibrate();
    
    /* 等待3S，便于用户调试 */
    std_delayms(3000); 
    
    while(1)
    {
        /* 使能通道VBGR */
        std_adc_internal_channel_vbgr_enable();
        
        /* 等待内部VBGR通道启动稳定 */
        std_delayus(ADC_VBGR_CHANNEL_DELAY);
        
        /* 使能ADC */
        std_adc_enable();
        
        /* 等待ADC使能状态稳定 */
        std_delayus(ADC_EN_DELAY);
        
        /* 获取采样值并转换为电压值，单位mV*/
        bsp_adc_get_channel_voltage(g_voltage_buf);
        
        /* 禁止ADC */
        std_adc_disable();
        
        /* 关闭通道VBGR */
        std_adc_internal_channel_vbgr_disable();
        
        for(uint8_t i = 0; i < ADC_CHANNEL_NUM; i++)
        {
            /* 串口打印ADC外部通道转换电压 */
            printf("ADC sample voltage: %d mV\r\n",g_voltage_buf[i]); 
        }

        /* 进入deepstop模式 */  
        std_pmu_enter_stop(PMU_MODE_DEEPSTOP, PMU_ENTRY_LOWPOWER_MODE_WFI);
        
        /* 唤醒后，恢复系统时钟配置 */
        system_clock_config();
        
    }
}





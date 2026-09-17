/************************************************************************************************/
/**
* @file               pmu_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              PMU BSP驱动函数，实现PMU DeepStop唤醒配置及唤醒流程。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "pmu_bsp.h"


/*-------------------------------------------functions------------------------------------------*/
#if   defined ( __CC_ARM )
#pragma arm section code = "WAKEUP_PROGRAM"
#elif defined ( __ICCARM__ )
__ramfunc
#elif defined ( __GNUC__ )
__attribute__  ((section (".RamFunc")))
#endif


/**
* @brief  配置Flash Deepstop唤醒等待时间为0us，唤醒后清除唤醒源标志
* @retval 无
*/
void bsp_pmu_deepstop_flash_wakeup_time(void)
{   
    /* 配置Flash唤醒等待时间为0us */
    PMU->FLASH_WAKEUP = PMU_DEEPSTOP_FLASH_WAKEUP_TIME_0;

    /* 置位SLEEPDEEP标志 */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;     
    
    /* 配置 SEVONPEND = 1 */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;  
    
    /* 配置Deepstop模式 */
    MODIFY_REG(PMU->CR, PMU_CR_LP_MODE, PMU_MODE_DEEPSTOP);
    
    /* 清除LPTIM溢出标志 */
    LPTIM1->ICR = LPTIM_CLEAR_ARRM;
    
    /* LPTIM启动单次计数 */
    LPTIM1->CR |= LPTIM_COUNT_SINGLE;
    
     __SEV();
     __WFE();
     __WFE();
    
    /* 还原SLEEPDEEP标志 */
    SCB->SCR &= (~SCB_SCR_SLEEPDEEP_Msk);    
    
    /* 用户可在低功耗唤醒后无需等待，即可快速响应相关应用程序 */    
    /* 等待LPTIM计数溢出后唤醒CPU，并清除标志 */
    while((LPTIM1->ISR & LPTIM_FLAG_ARRM) != LPTIM_FLAG_ARRM);
    LPTIM1->ICR = LPTIM_CLEAR_ARRM;     
    
    /* 清除Pending位 */
    NVIC->ICPR[0U] = (uint32_t)(1UL << (LPTIM1_IRQn));
    
    /* 切换到Flash中运行程序，需延时10us */
    for(;g_flash_delay_time >0 ;g_flash_delay_time--);
}


#if   defined ( __CC_ARM )
#pragma arm section
#endif



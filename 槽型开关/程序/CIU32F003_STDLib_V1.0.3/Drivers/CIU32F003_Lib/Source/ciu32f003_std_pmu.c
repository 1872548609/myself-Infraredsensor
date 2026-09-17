/************************************************************************************************/
/**
* @file               ciu32f003_std_pmu.c
* @author             MCU Ecosystem Development Team
* @brief              PMU STD库驱动。
*                     实现低功耗模式进入功能API。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/************************************************************************************************/
/**
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @addtogroup PMU 
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"


#ifdef STD_PMU_PERIPHERAL_USED

/*-------------------------------------------functions------------------------------------------*/
/************************************************************************************************/
/**
* @addtogroup PMU_External_Functions 
* @{
*
*/
/************************************************************************************************/ 

/**
* @brief  进入sleep模式
* @param  mode_entry 进入低功耗模式的方式
*             @arg PMU_ENTRY_LOWPOWER_MODE_WFE
*             @arg PMU_ENTRY_LOWPOWER_MODE_WFI
* @retval 无
*/
void std_pmu_enter_sleep(uint32_t mode_entry)
{    
    /* 清除 SLEEPDEEP 标志 */
    SCB->SCR &= (~SCB_SCR_SLEEPDEEP_Msk); 
    
    /* 处理低功耗模式的进入方式*/
    if(PMU_ENTRY_LOWPOWER_MODE_WFI == mode_entry)
    {
        __WFI();
    }
    else
    {
        /* 先生成一个事件，通过WFE清除事件，重新调用WFE进入低功耗模式*/
        __SEV();
        __WFE();
        __WFE();
    }
}

/**
* @brief  进入stop/Deepstop模式
* @param  stop_mode 低功耗模式选择
*             @arg PMU_MODE_STOP
*             @arg PMU_MODE_DEEPSTOP
* @param  mode_entry 进入低功耗模式的方式
*             @arg PMU_ENTRY_LOWPOWER_MODE_WFE
*             @arg PMU_ENTRY_LOWPOWER_MODE_WFI
* @retval 无
*/
void std_pmu_enter_stop(uint32_t stop_mode, uint32_t mode_entry)
{    
    std_rcc_apb1_clk_enable(RCC_PERIPH_CLK_PMU);
    
    /* 配置低功耗模式  */
    MODIFY_REG(PMU->CR, PMU_CR_LP_MODE, stop_mode);
    
    /* 置位SLEEPDEEP标志 */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;   
    
     /* 处理低功耗模式的进入方式*/
    if(PMU_ENTRY_LOWPOWER_MODE_WFI == mode_entry)
    {
        __WFI();
    }
    else
    {
        /* 先生成一个事件，通过WFE清除事件，重新调用WFE进入低功耗模式*/
        __SEV();
        __WFE();
        __WFE();
    }   
    
    /* 还原SLEEPDEEP标志 */
    SCB->SCR &= (~SCB_SCR_SLEEPDEEP_Msk);
}



/** 
* @} 
*/

#endif /* STD_PMU_PERIPHERAL_USED */

/** 
* @} 
*/

/** 
* @} 
*/


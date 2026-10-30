/************************************************************************************************/
/**
* @file               ciu32f003_std_rcc.c
* @author             MCU Ecosystem Development Team
* @brief              RCC STD库驱动。
*                     实现RCC时钟频率获取等API。
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
* @addtogroup RCC 
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @addtogroup RCC_External_Functions 
* @{
*
*/
/************************************************************************************************/ 

/**
* @brief  获取当前系统时钟频率（SYSCLK）
* @retval 返回系统时钟频率（Hz）
*/
uint32_t std_rcc_get_sysclkfreq(void)
{
    uint32_t frequency = 0;
    
    /* 获取当前系统时钟源 */
    switch(std_rcc_get_sysclk_source())
    {
        /* 系统时钟为EXTCLK，默认为8MHz */
        case RCC_SYSCLK_SRC_STATUS_EXTCLK:
        {
            frequency = EXTCLK_VALUE;
        }break;
        
        /* 系统时钟为RCHDIV3 */
        case RCC_SYSCLK_SRC_STATUS_RCHDIV3:
        {
            frequency = RCH_VALUE/3;
        }break;
        
        /* 系统时钟为RCH */
        case RCC_SYSCLK_SRC_STATUS_RCH:
        {
            frequency = RCH_VALUE;
        }break;
        
        /* 系统时钟为RCL */
        case RCC_SYSCLK_SRC_STATUS_RCL:
        {
            frequency = RCL_VALUE;
        }break;       
        
        /* 系统时钟为RCHDIV6 */
        case RCC_SYSCLK_SRC_STATUS_RCHDIV6:
        default:
        {
            frequency = RCH_VALUE/6;
        }break;
    }
    return frequency;
}


/**
* @brief  获取AHB时钟频率（HCLK）
* @retval 返回HCLK时钟频率（Hz）
*/
uint32_t std_rcc_get_hclkfreq(void)
{
    uint32_t frequency = 0;
    uint32_t tmp, hclk_div;
    
    tmp = std_rcc_get_sysclkfreq();
    
    /* 获取AHB分频因子 */
    hclk_div = std_rcc_get_ahbdiv()>>RCC_CFG_HPRE_POS;
    frequency = tmp >> hclk_div;    

    return frequency;
}

/**
* @brief  获取APB时钟频率（PCLK）
* @retval 返回PCLK1时钟频率（Hz）
*/
uint32_t std_rcc_get_pclkfreq(void)
{
    uint32_t frequency = 0;
    uint32_t tmp, pclk_div;
    
    tmp = std_rcc_get_hclkfreq();
    
    /* 获取APB分频因子 */
    pclk_div = std_rcc_get_apbdiv()>>RCC_CFG_PPRE_POS;
    
    if (pclk_div < 3)
    {
        frequency = tmp;
    }
    else
    {
        pclk_div -= 3;
        frequency = tmp >> pclk_div;
    }
    
    return frequency;
}


/** 
* @} 
*/



/** 
* @} 
*/

/** 
* @} 
*/

/************************************************************************************************/
/**
* @file               ciu32f003_std_pmu.h
* @author             MCU Ecosystem Development Team
* @brief              PMU STD库驱动头文件。
*                     提供PMU相关STD库操作函数声明、数据类型以及常量的定义。 
*                      
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*避免头文件重复引用*/
#ifndef CIU32F003_STD_PMU_H
#define CIU32F003_STD_PMU_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup PMU PMU
* @brief 电源管理单元的STD库驱动
* @{
*/
/************************************************************************************************/

#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std_common.h"


/*--------------------------------------------define--------------------------------------------*/
/************************************************************************************************/
/** 
* @defgroup PMU_Constants PMU Constants
* @brief PMU常量定义及宏定义
* @{
*/
/************************************************************************************************/
/* 低功耗进入方式定义 */
#define PMU_ENTRY_LOWPOWER_MODE_WFI         (0x00UL)                            /**< WFI方式进入低功耗   */
#define PMU_ENTRY_LOWPOWER_MODE_WFE         (0x01UL)                            /**< WFE方式进入低功耗   */
     
/* 低功耗模式定义 */     
#define PMU_MODE_STOP                       PMU_CR_LP_MODE_STOP                 /**< Stop模式            */
#define PMU_MODE_DEEPSTOP                   PMU_CR_LP_MODE_DEEPSTOP             /**< Deepstop模式        */

/* Deepstop模式唤醒过程中Flash的唤醒等待时间定义 */
#define PMU_DEEPSTOP_FLASH_WAKEUP_TIME_0       (0x3UL << PMU_FLASH_WAKEUP_FLASH_WAKEUP_POS)       /**< Deepstop模式唤醒过程中Flash的唤醒等待时间为0us   */
#define PMU_DEEPSTOP_FLASH_WAKEUP_TIME_10      (0x00000000U)                                      /**< Deepstop模式唤醒过程中Flash的唤醒等待时间为10us  */


/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/
/************************************************************************************************/
/**
* @defgroup PMU_External_Functions PMU External Functions
* @brief    PMU对外函数
* @{
*/
/************************************************************************************************/
/**
* @brief  配置Deepstop模式唤醒过程中Flash的唤醒等待时间
* @param  time_value 等待时间
*             @arg PMU_DEEPSTOP_FLASH_WAKEUP_TIME_0 : 0us
*             @arg PMU_DEEPSTOP_FLASH_WAKEUP_TIME_10: 10us
* @retval 无
*/
__STATIC_INLINE void std_pmu_deepstop_flash_wakeup_time_config(uint32_t time_value)           
{
    MODIFY_REG(PMU->FLASH_WAKEUP, PMU_FLASH_WAKEUP_FLASH_WAKEUP, time_value);
}


/* PMU低功耗模式相关函数 */
void std_pmu_enter_sleep(uint32_t mode_entry);
void std_pmu_enter_stop(uint32_t stop_mode, uint32_t mode_entry);


/**
* @}
*/


#ifdef __cplusplus
}
#endif

/**
* @}
*/

/**
* @}
*/

#endif /* CIU32F003_STD_PMU_H */


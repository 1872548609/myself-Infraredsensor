/************************************************************************************************/
/**
* @file               ciu32f003_std_lptim.h
* @author             MCU Ecosystem Development Team
* @brief              LPTIM STD库驱动头文件。
*                     提供LPTIM相关的STD库操作函数声明、数据类型以及常量的定义。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*避免头文件重复引用*/
#ifndef CIU32F003_STD_LPTIM_H
#define CIU32F003_STD_LPTIM_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup LPTIM LPTIM
* @brief 低功耗定时器的STD库驱动
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
* @defgroup LPTIM_Constants LPTIM Constants
* @brief    LPTIM常量定义及宏定义
* @{
*
*/
/************************************************************************************************/
/* 计数器计数模式 */
#define LPTIM_COUNT_CONTINUOUS          LPTIM_CR_CNTSTRT            /**< 连续计数模式    */
#define LPTIM_COUNT_SINGLE              LPTIM_CR_SNGSTRT            /**< 单次计数模式    */

/* LPTIM PSC预分频器分频系数选择 */
#define LPTIM_PRESCALER_DIV1            LPTIM_CFG_PRESC_1           /**< PSC预分频器1分频   */
#define LPTIM_PRESCALER_DIV2            LPTIM_CFG_PRESC_2           /**< PSC预分频器2分频   */
#define LPTIM_PRESCALER_DIV4            LPTIM_CFG_PRESC_4           /**< PSC预分频器4分频   */
#define LPTIM_PRESCALER_DIV8            LPTIM_CFG_PRESC_8           /**< PSC预分频器8分频   */
#define LPTIM_PRESCALER_DIV16           LPTIM_CFG_PRESC_16          /**< PSC预分频器16分频  */
#define LPTIM_PRESCALER_DIV32           LPTIM_CFG_PRESC_32          /**< PSC预分频器32分频  */
#define LPTIM_PRESCALER_DIV64           LPTIM_CFG_PRESC_64          /**< PSC预分频器64分频  */
#define LPTIM_PRESCALER_DIV128          LPTIM_CFG_PRESC_128         /**< PSC预分频器128分频 */

/* LPTIM中断源 */
#define LPTIM_INTERRUPT_ARRM            LPTIM_IER_ARRM_IE           /**< 自动重载匹配中断使能 */
#define LPTIM_INTERRUPT_ITRF            LPTIM_IER_ITRF_IE           /**< 级联触发中断使能     */

/* LPTIM中断状态标志 */
#define LPTIM_FLAG_ARRM                 LPTIM_ISR_ARRM              /**< 自动重载匹配标志     */
#define LPTIM_FLAG_ITRF                 LPTIM_ISR_ITRF              /**< 级联触发标志         */

/* LPTIM中断清除标志 */
#define LPTIM_CLEAR_ARRM                LPTIM_ICR_ARRM_CF           /**< 自动重载匹配标志清零 */
#define LPTIM_CLEAR_ITRF                LPTIM_ICR_ITRF_CF           /**< 级联触发标志清零     */

/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/
/************************************************************************************************/
/**
* @defgroup LPTIM_External_Functions LPTIM External Functions
* @brief    LPTIM对外函数
* @{
*
*/
/************************************************************************************************/


/** 
* @brief  使能LPTIM
* @note   在LPTIM使能位置位后，需要两个计数器周期后才能生效
* @retval 无
*/
__STATIC_INLINE void std_lptim_enable(void)
{
    LPTIM1->CR |= LPTIM_CR_ENABLE;
}


/** 
* @brief  禁止LPTIM
* @retval 无
*/
__STATIC_INLINE void std_lptim_disable(void)
{
    LPTIM1->CR &= (~LPTIM_CR_ENABLE);
}


/** 
* @brief  配置LPTIM计数器按照预期模式开始计数
* @param  operate_mode 计数模式选择
*             @arg LPTIM_COUNT_CONTINUOUS:    连续计数模式
*             @arg LPTIM_COUNT_SINGLE:        单次计数模式
* @note   必须使能LPTIM后才能启动计数器计数
* @retval 无
*/
__STATIC_INLINE void std_lptim_start_counter(uint32_t operate_mode)
{
    MODIFY_REG(LPTIM1->CR, (LPTIM_CR_CNTSTRT | LPTIM_CR_SNGSTRT), operate_mode);
}


/** 
* @brief  设置LPTIM自动重装载值
* @param  auto_reload 自动重装载值（该变量的范围为0x0~0xFFFF）
* @retval 无
*/
__STATIC_INLINE void std_lptim_set_auto_reload(uint32_t auto_reload)
{
    LPTIM1->ARR = auto_reload;
}


/** 
* @brief  获取LPTIM自动重装载值
* @retval uint32_t 自动重装载值（该变量的范围为0x0~0xFFFF）
*/
__STATIC_INLINE uint32_t std_lptim_get_auto_reload(void)
{
    return (LPTIM1->ARR);
}


/** 
* @brief  获取LPTIM计数值
* @note   当LPTIM为异步时钟计数时，为确保获取正确的计数值，需要确保两次读取的计数值一致。
* @retval uint32_t LPTIM计数值（该变量的范围为0x0~0xFFFF）
*/
__STATIC_INLINE uint32_t std_lptim_get_count(void)
{
    return (LPTIM1->CNT);
}


/** 
* @brief  设置LPTIM预分频器分频系数
* @param  prescaler 预分频系数选择
*             @arg LPTIM_PRESCALER_DIV1
*             @arg LPTIM_PRESCALER_DIV2
*             @arg LPTIM_PRESCALER_DIV4
*             @arg ...
*             @arg LPTIM_PRESCALER_DIV128
* @retval 无
*/
__STATIC_INLINE void std_lptim_set_prescaler(uint32_t prescaler)
{
    MODIFY_REG(LPTIM1->CFG, LPTIM_CFG_PRESC, prescaler);
}


/** 
* @brief  获取LPTIM预分频器分频系数
* @retval uint32_t 预分频系数选择
*             @arg LPTIM_PRESCALER_DIV1
*             @arg LPTIM_PRESCALER_DIV2
*             @arg LPTIM_PRESCALER_DIV4
*             @arg ...
*             @arg LPTIM_PRESCALER_DIV128
*/
__STATIC_INLINE uint32_t std_lptim_get_prescaler(void)
{
    return (LPTIM1->CFG & LPTIM_CFG_PRESC);
}


/** 
* @brief  使能LPTIM级联触发
* @retval 无
*/
__STATIC_INLINE void std_lptim_internal_trigger_enable(void)
{
    LPTIM1->CFG |= LPTIM_CFG_ITREN;
}


/** 
* @brief  禁止LPTIM级联触发
* @retval 无
*/
__STATIC_INLINE void std_lptim_internal_trigger_disable(void)
{
    LPTIM1->CFG &= ~LPTIM_CFG_ITREN;
}


/** 
* @brief  使能LPTIM中断
* @param  interrupt LPTIM中断源
*             @arg LPTIM_INTERRUPT_ARRM:      自动重载匹配中断
*             @arg LPTIM_INTERRUPT_ITRF:      级联触发中断使能
* @retval 无
*/
__STATIC_INLINE void std_lptim_interrupt_enable(uint32_t interrupt)
{
    LPTIM1->IER |= interrupt;
}


/** 
* @brief  禁止LPTIM中断
* @param  interrupt LPTIM中断源
*             @arg LPTIM_INTERRUPT_ARRM:      自动重载匹配中断
*             @arg LPTIM_INTERRUPT_ITRF:      级联触发中断使能
* @retval 无
*/
__STATIC_INLINE void std_lptim_interrupt_disable(uint32_t interrupt)
{
    LPTIM1->IER &= (~interrupt);
}


/** 
* @brief  获取LPTIM中断状态
* @param  interrupt LPTIM中断源
*             @arg LPTIM_INTERRUPT_ARRM:      自动重载匹配中断
*             @arg LPTIM_INTERRUPT_ITRF:      级联触发中断使能
* @retval uint32_t LPTIM中断源使能状态
*             @arg 非0: 使能
*             @arg 0:   禁止
*/
__STATIC_INLINE uint32_t std_lptim_get_interrupt_status(uint32_t interrupt)
{
    return (LPTIM1->IER & interrupt);
}


/** 
* @brief  获取LPTIM状态标志位
* @param  flag LPTIM状态标志位
*             @arg LPTIM_FLAG_ARRM:        自动重载匹配标志
*             @arg LPTIM_FLAG_ITRF:        级联触发标志
* @retval uint32_t LPTIM标志位的状态
*             @arg 非0: 标志位置位
*             @arg 0:   标志位清除
*/
__STATIC_INLINE uint32_t std_lptim_get_flag(uint32_t flag)
{
    return (LPTIM1->ISR & flag);
}


/** 
* @brief  清除LPTIM状态标志位
* @param  flag LPTIM状态标志位
*             @arg LPTIM_CLEAR_ARRM:        自动重载匹配标志
*             @arg LPTIM_CLEAR_ITRF:        级联触发标志清零
* @retval 无
*/
__STATIC_INLINE void std_lptim_clear_flag(uint32_t flag)
{
    LPTIM1->ICR = flag;
}


/* LPTIM去初始化函数 */
void std_lptim_deinit(void);

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
     
#endif /* CIU32F003_STD_LPTIM_H */

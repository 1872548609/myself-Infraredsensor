/************************************************************************************************/
/**
* @file               ciu32f003_std_irtim.h
* @author             MCU Ecosystem Development Team
* @brief              IRTIM STD库驱动头文件。
*                     提供IRTIM相关的STD库操作函数声明、数据类型以及常量的定义。                         
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef CIU32F003_STD_IRTIM_H
#define CIU32F003_STD_IRTIM_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup IRTIM IRTIM
* @brief 红外控制模块的STD库驱动
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
* @defgroup IRTIM_Constants  IRTIM Constants
* @brief    IRTIM常量定义及宏定义
* @{
*
*/
/************************************************************************************************/

/* IRTIM 调制信号源选择  */    
#define ITRIM_SIGNAL_SOURCE_TIM3_OC1                       IRTIM_CR_IR_MODE_TIM3_OC1                /**< 调制信号源于TIM3_OC1  */
#define ITRIM_SIGNAL_SOURCE_UART1_TX                       IRTIM_CR_IR_MODE_UART1_TX                /**< 调制信号源于UART1_TX  */
#define ITRIM_SIGNAL_SOURCE_UART2_TX                       IRTIM_CR_IR_MODE_UART2_TX                /**< 调制信号源于UART2_TX  */
                    
/* IRTIM 输出信号极性选择 */
#define IRTIM_POLARITY_DIRECT                              (0x00000000U)                            /**< IRTIM 输出信号未反相  */
#define IRTIM_POLARITY_INVERSE                             IRTIM_CR_IR_POL                          /**< IRTIM 输出信号反相    */
     
     
/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup IRTIM_External_Functions IRTIM External Functions
* @brief    IRTIM对外函数
* @{
*
*/
/************************************************************************************************/
/** 
* @brief  配置IRTIM调制信号源
* @param  source 调制信号源选择
*             @arg ITRIM_SIGNAL_SOURCE_TIM3_OC1： 调制信号源为TIM3的OC1
*             @arg ITRIM_SIGNAL_SOURCE_UART1_TX： 调制信号源为UART1
*             @arg ITRIM_SIGNAL_SOURCE_UART2_TX： 调制信号源为UART2
* @retval 无
*/
__STATIC_INLINE void std_irtim_set_signal_source(uint32_t source)
{
    MODIFY_REG(IRTIM->CR, IRTIM_CR_IR_MODE, source);
}

/**
* @brief  获取IRTIM调制信号源
* @retval uint32_t 调制信号源
*             @arg ITRIM_SIGNAL_SOURCE_TIM3_OC1： 调制信号源为TIM3的OC1
*             @arg ITRIM_SIGNAL_SOURCE_UART1_TX： 调制信号源为UART1
*             @arg ITRIM_SIGNAL_SOURCE_UART2_TX： 调制信号源为UART2
*/
__STATIC_INLINE uint32_t std_irtim_get_signal_source(void)
{
    return(IRTIM->CR & IRTIM_CR_IR_MODE);
}

/**
* @brief  IR_OUT输出信号极性选择
* @param  polarity 输出信号极性
*             @arg IRTIM_POLARITY_DIRECT:  输出信号未反相
*             @arg IRTIM_POLARITY_INVERSE: 输出信号反相
* @retval 无
*/
__STATIC_INLINE void std_irtim_set_polarity(uint32_t polarity)
{
    MODIFY_REG(IRTIM->CR, IRTIM_CR_IR_POL, polarity);
}

/**
* @brief  获取IR_OUT输出信号极性状态
* @retval uint32_t 输出信号极性
*             @arg IRTIM_POLARITY_DIRECT:  输出信号未反相
*             @arg IRTIM_POLARITY_INVERSE: 输出信号反相
*/
__STATIC_INLINE uint32_t std_irtim_get_polarity(void)
{
    return(IRTIM->CR & IRTIM_CR_IR_POL);
}



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

#endif /* CIU32F003_STD_IRTIM_H */

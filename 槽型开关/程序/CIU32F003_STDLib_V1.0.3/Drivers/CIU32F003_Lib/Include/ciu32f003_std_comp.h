/************************************************************************************************/
/**
* @file               ciu32f003_std_comp.h
* @author             MCU Ecosystem Development Team
* @brief              COMP STD库驱动头文件。
*                     提供COMP相关的STD库操作函数声明、数据类型以及常量的定义。                         
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef CIU32F003_STD_COMP_H
#define CIU32F003_STD_COMP_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup COMP COMP
* @brief 比较器的STD库驱动
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
* @defgroup COMP_Constants COMP Constants 
* @brief  COMP常量定义及宏定义
* @{
*
*/
/************************************************************************************************/
     
/* 比较器内部参考电压VDDA 分压 */     
#define COMP_VDDA_DIV_1DIV16             COMP_CR_VCDIV_1DIV16         /**< VDDA 分压选择1/16    */
#define COMP_VDDA_DIV_2DIV16             COMP_CR_VCDIV_2DIV16         /**< VDDA 分压选择2/16    */     
#define COMP_VDDA_DIV_3DIV16             COMP_CR_VCDIV_3DIV16         /**< VDDA 分压选择3/16    */     
#define COMP_VDDA_DIV_4DIV16             COMP_CR_VCDIV_4DIV16         /**< VDDA 分压选择4/16    */     
#define COMP_VDDA_DIV_5DIV16             COMP_CR_VCDIV_5DIV16         /**< VDDA 分压选择5/16    */
#define COMP_VDDA_DIV_6DIV16             COMP_CR_VCDIV_6DIV16         /**< VDDA 分压选择6/16    */
#define COMP_VDDA_DIV_7DIV16             COMP_CR_VCDIV_7DIV16         /**< VDDA 分压选择7/16    */
#define COMP_VDDA_DIV_8DIV16             COMP_CR_VCDIV_8DIV16         /**< VDDA 分压选择8/16    */
#define COMP_VDDA_DIV_9DIV16             COMP_CR_VCDIV_9DIV16         /**< VDDA 分压选择9/16    */     
#define COMP_VDDA_DIV_10DIV16            COMP_CR_VCDIV_10DIV16        /**< VDDA 分压选择10/16   */     
#define COMP_VDDA_DIV_11DIV16            COMP_CR_VCDIV_11DIV16        /**< VDDA 分压选择11/16   */     
#define COMP_VDDA_DIV_12DIV16            COMP_CR_VCDIV_12DIV16        /**< VDDA 分压选择12/16   */
#define COMP_VDDA_DIV_13DIV16            COMP_CR_VCDIV_13DIV16        /**< VDDA 分压选择13/16   */

/* 比较器正相输入 */
#define COMP_INPSEL_IO1                   COMP_CSR_INP_IO1            /**< 正相输入 COMP1 = PB0, COMP2 = PA3 */
#define COMP_INPSEL_IO2                   COMP_CSR_INP_IO2            /**< 正相输入 COMP1 = PB1, COMP2 = PA4 */

/* 比较器反相输入 */
#define COMP_INMSEL_INVREF                COMP_CSR_INM_INT_VREF       /**< 反相输入 内部参考电压 */
#define COMP_INMSEL_IO                    COMP_CSR_INM_IO             /**< 反相输入 COMP1 = PB1, COMP2 = PA4 */                      

/* 比较器正相输入模式选择，可用于选择窗口比较器功能 */
#define COMP_INPMODE_EACH_INPUT           (0x00000000U)               /**< 正相输入各自独立，由各自inpsel决定 */
#define COMP_INPMODE_COMMON_INPUT         COMP_CSR_INPMOD             /**< 正相输入相互连接，即窗口比较器模式 */

/* 比较器滤波时间 */
#define COMP_FLTIME_1CYCLE                COMP_CSR_FLTIME_1CYCLE      /**< 滤波时间 1个CYCLE    */
#define COMP_FLTIME_3CYCLE                COMP_CSR_FLTIME_3CYCLE      /**< 滤波时间 3个CYCLE    */
#define COMP_FLTIME_7CYCLE                COMP_CSR_FLTIME_7CYCLE      /**< 滤波时间 7个CYCLE    */
#define COMP_FLTIME_15CYCLE               COMP_CSR_FLTIME_15CYCLE     /**< 滤波时间 15个CYCLE   */
#define COMP_FLTIME_31CYCLE               COMP_CSR_FLTIME_31CYCLE     /**< 滤波时间 31个CYCLE   */
#define COMP_FLTIME_63CYCLE               COMP_CSR_FLTIME_63CYCLE     /**< 滤波时间 63个CYCLE   */
#define COMP_FLTIME_255CYCLE              COMP_CSR_FLTIME_255CYCLE    /**< 滤波时间 255个CYCLE  */
#define COMP_FLTIME_1023CYCLE             COMP_CSR_FLTIME_1023CYCLE   /**< 滤波时间 1023个CYCLE */

/* 比较器输出极性 */
#define COMP_OUTPOL_NON_INVERTED          (0x00000000U)               /**< 输出状态不反相 */
#define COMP_OUTPOL_INVERTED              COMP_CSR_POL                /**< 输出状态反相   */

/* 比较器输出选择 */
#define COMP_OUTMODE_EACH_OUT             (0x00000000U)               /**< 输出为其原始电压比较结果         */
#define COMP_OUTMODE_COMMON_XOR_OUT       COMP_CSR_OUTMOD             /**< 输出为比较器1、2电压比较结果异或 */

/* 比较器输出结果 */
#define COMP_OUTPUT_LEVEL_LOW             (0x00000000UL)              /**< 比较器输出电平低 */
#define COMP_OUTPUT_LEVEL_HIGH            (0x00000001UL)              /**< 比较器输出电平高 */

/* 比较器内部参考电压源 */
#define COMP_REFERENCE_VBGR               (0x00000000UL)              /**< 比较器内部参考电压源选择VBGR         */
#define COMP_REFERENCE_VDDA_DIV           COMP_CR_VCSEL               /**< 比较器内部参考电压源选择VDDA 16级分压 */

/* COMP启动稳定时间 */
#define COMP_EN_DELAY                     (1U)                        /**< COMP使能启动时间 */

/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup COMP_External_Functions COMP External Functions
* @brief    COMP对外函数
* @{
*
*/
/************************************************************************************************/
/** 
* @brief  使能COMP
* @param  compx COMP外设
* @retval 无
*/
__STATIC_INLINE void std_comp_enable(COMP_t *compx)
{
    compx->CSR |= (COMP_CSR_EN);
}

/** 
* @brief  禁止COMP
* @param  compx COMP外设
* @retval 无
*/
__STATIC_INLINE void std_comp_disable(COMP_t *compx)
{
    compx->CSR &= (~COMP_CSR_EN);
}

/** 
* @brief  设置COMP反相输入
* @param  compx COMP外设
* @param  input_minus 反相输入选择
*             @arg COMP_INMSEL_INVREF
*             @arg COMP_INMSEL_IO
* @retval 无
*/
__STATIC_INLINE void std_comp_set_input_minus(COMP_t *compx, uint32_t input_minus)
{
    MODIFY_REG(compx->CSR, COMP_CSR_INM, input_minus);
}

/** 
* @brief  获取COMP反相输入
* @param  compx COMP外设
* @retval uint32_t 返回反相输入
*             @arg COMP_INMSEL_INVREF
*             @arg COMP_INMSEL_IO
*/
__STATIC_INLINE uint32_t std_comp_get_input_minus(COMP_t *compx)
{
    return(compx->CSR & COMP_CSR_INM);
}

/** 
* @brief  设置COMP正相输入
* @param  compx COMP外设
* @param  input_plus 正相输入选择
*             @arg COMP_INPSEL_IO1
*             @arg COMP_INPSEL_IO2
* @retval 无
*/
__STATIC_INLINE void std_comp_set_input_plus(COMP_t *compx, uint32_t input_plus)
{
    MODIFY_REG(compx->CSR, COMP_CSR_INP, input_plus);
}

/** 
* @brief  获取COMP正相输入
* @param  compx COMP外设
* @retval uint32_t 返回正相输入
*             @arg COMP_INPSEL_IO1
*             @arg COMP_INPSEL_IO2
*/
__STATIC_INLINE uint32_t std_comp_get_input_plus(COMP_t *compx)
{
    return(compx->CSR & COMP_CSR_INP);
}

/** 
* @brief  设置COMP正相输入模式
* @param  compx COMP外设
* @param  input_mode 正相输入模式选择
*             @arg COMP_INPMODE_EACH_INPUT
*             @arg COMP_INPMODE_COMMON_INPUT
* @retval 无
*/
__STATIC_INLINE void std_comp_set_input_plus_mode(COMP_t *compx, uint32_t input_mode)
{
    MODIFY_REG(compx->CSR, COMP_CSR_INPMOD, input_mode);
}

/** 
* @brief  设置COMP输出模式
* @param  compx COMP外设
* @param  output_mode 输出模式选择
*             @arg COMP_OUTMODE_EACH_OUT
*             @arg COMP_OUTMODE_COMMON_XOR_OUT
* @retval 无
*/
__STATIC_INLINE void std_comp_set_output_mode(COMP_t *compx, uint32_t output_mode)
{
    MODIFY_REG(compx->CSR, COMP_CSR_OUTMOD, output_mode);
}

/** 
* @brief  设置COMP输出极性
* @param  compx COMP外设
* @param  output_polarity 输出极性选择
*             @arg COMP_OUTPOL_NON_INVERTED
*             @arg COMP_OUTPOL_INVERTED
* @retval 无
*/
__STATIC_INLINE void std_comp_set_output_polarity(COMP_t *compx, uint32_t output_polarity)
{
    MODIFY_REG(compx->CSR, COMP_CSR_POL, output_polarity);
}

/** 
* @brief  获取COMP输出极性
* @param  compx COMP外设
* @retval uint32_t 返回输出极性
*             @arg COMP_OUTPOL_NON_INVERTED
*             @arg COMP_OUTPOL_INVERTED
*/
__STATIC_INLINE uint32_t std_comp_get_output_polarity(COMP_t *compx)
{
    return(compx->CSR & COMP_CSR_POL);
}

/** 
* @brief  使能COMP的输入迟滞
* @retval 无
*/
__STATIC_INLINE void std_comp_input_hysteresis_enable(void)
{
    COMP_COMMON->CR |= COMP_CR_HYST;
}

/** 
* @brief  禁止COMP的输入迟滞
* @retval 无
*/
__STATIC_INLINE void std_comp_input_hysteresis_disable(void)
{
    COMP_COMMON->CR &= ~COMP_CR_HYST;
}

/** 
* @brief  设置COMP输出滤波时间
* @param  compx COMP外设
* @param  filter_time 滤波时间选择
*             @arg COMP_FLTIME_1CYCLE
*             @arg COMP_FLTIME_3CYCLE
*             @arg ...
*             @arg COMP_FLTIME_1023CYCLE
* @retval 无
*/
__STATIC_INLINE void std_comp_set_output_filter_time(COMP_t *compx, uint32_t filter_time)
{
    MODIFY_REG(compx->CSR, COMP_CSR_FLTIME, filter_time);
}

/** 
* @brief  COMP输出滤波使能
* @param  compx COMP外设
* @retval 无
*/
__STATIC_INLINE void std_comp_output_filter_enable(COMP_t *compx)
{
    compx->CSR |= (COMP_CSR_FLTEN);
}

/** 
* @brief  COMP输出滤波禁止
* @param  compx COMP外设
* @retval 无
*/
__STATIC_INLINE void std_comp_output_filter_disable(COMP_t *compx)
{
    compx->CSR &= (~COMP_CSR_FLTEN);
}

/** 
* @brief  获取COMP使能状态
* @param  compx COMP外设
* @retval uint32_t 返回判断结果
*             @arg 非0： 表示COMP已使能
*             @arg 0：   表示COMP未使能
*/
__STATIC_INLINE uint32_t std_comp_get_enable_status(COMP_t *compx)
{
    return (compx->CSR & COMP_CSR_EN);
}

/** 
* @brief  获取COMP输出结果
* @param  compx COMP外设
* @retval uint32_t 返回COMP输出结果
*             @arg COMP_OUTPUT_LEVEL_LOW
*             @arg COMP_OUTPUT_LEVEL_HIGH
*/
__STATIC_INLINE uint32_t std_comp_get_output_result(COMP_t *compx)
{
    return ((compx->CSR & COMP_CSR_VAL) >> COMP_CSR_VAL_POS);
}

/** 
* @brief  COMP参考电压源设置
*             @arg COMP_REFERENCE_VBGR
*             @arg COMP_REFERENCE_VDDA_DIV
* @note   VDDA分压作为参考电压源，需配置VDDA分压参数;
* @retval 无
*/
__STATIC_INLINE void std_comp_set_reference_source(uint32_t reference_voltage)
{
    MODIFY_REG(COMP_COMMON->CR, COMP_CR_VCSEL, reference_voltage);
}

/** 
* @brief  获取COMP参考电压源
* @retval uint32_t  返回参考电源源
*             @arg COMP_REFERENCE_VBGR
*             @arg COMP_REFERENCE_VDDA_DIV
*/
__STATIC_INLINE uint32_t std_comp_get_reference_source(void)
{
    return (COMP_COMMON->CR & COMP_CR_VCSEL);
}

/** 
* @brief  设置VDDA 16级分压
* @param  vdda_div VDDA 16级分压
*             @arg COMP_VDDA_DIV_1DIV16
*             @arg COMP_VDDA_DIV_2DIV16
*             @arg ...
*             @arg COMP_VDDA_DIV_13DIV16
* @retval 无
*/
__STATIC_INLINE void std_comp_set_ref_vdda_div(uint32_t vdda_div)
{
    MODIFY_REG(COMP_COMMON->CR, COMP_CR_VCDIV, vdda_div);
}

/** 
* @brief  获取VDDA 16级分压
* @retval uint32_t 返回 VDDA 16级分压
*             @arg COMP_VDDA_DIV_1DIV16
*             @arg COMP_VDDA_DIV_2DIV16
*             @arg ...
*             @arg COMP_VDDA_DIV_13DIV16
*/
__STATIC_INLINE uint32_t std_comp_get_ref_vdda_div(void)
{
    return (COMP_COMMON->CR & COMP_CR_VCDIV);
}

/** 
* @brief  配置COMP的正相输入和反向输入
* @param  compx COMP外设
* @param  input_plus 正相输入选择
*             @arg COMP_INPSEL_IO1
*             @arg COMP_INPSEL_IO2
* @param  input_minis 反向相输入选择
*             @arg COMP_INMSEL_INVREF
*             @arg COMP_INMSEL_IO
*
* @retval 无
*/
__STATIC_INLINE void std_comp_input_config(COMP_t *compx, uint32_t input_plus, uint32_t input_minis)
{
    MODIFY_REG(compx->CSR,
               (COMP_CSR_INP | COMP_CSR_INM),
               (input_plus | input_minis));
}

/* COMP去初始化函数 */
void std_comp_deinit(COMP_t *compx);

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

#endif /* CIU32F003_STD_COMP_H */

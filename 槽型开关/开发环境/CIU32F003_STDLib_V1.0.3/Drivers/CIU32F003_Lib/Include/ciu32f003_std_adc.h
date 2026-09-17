/************************************************************************************************/
/**
* @file               ciu32f003_std_adc.h
* @author             MCU Ecosystem Development Team
* @brief              ADC STD库驱动头文件。
*                     提供ADC相关的STD库操作函数声明、数据类型以及常量的定义。                         
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef CIU32F003_STD_ADC_H
#define CIU32F003_STD_ADC_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup ADC ADC
* @brief 模数转换器的STD库驱动
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
* @defgroup ADC_Constants  ADC Constants
* @brief    ADC常量定义及宏定义
* @{
*
*/
/************************************************************************************************/
/* ADC_CK时钟分频系数 */
#define ADC_CK_DIV1                          ADC_CFG2_PRESC_DIV1                            /**< ADC_CK时钟: 不分频 */
#define ADC_CK_DIV2                          ADC_CFG2_PRESC_DIV2                            /**< ADC_CK时钟: 2分频  */
#define ADC_CK_DIV3                          ADC_CFG2_PRESC_DIV3                            /**< ADC_CK时钟: 3分频  */
#define ADC_CK_DIV4                          ADC_CFG2_PRESC_DIV4                            /**< ADC_CK时钟: 4分频  */
#define ADC_CK_DIV8                          ADC_CFG2_PRESC_DIV8                            /**< ADC_CK时钟: 8分频  */
#define ADC_CK_DIV16                         ADC_CFG2_PRESC_DIV16                           /**< ADC_CK时钟: 16分频 */
#define ADC_CK_DIV32                         ADC_CFG2_PRESC_DIV32                           /**< ADC_CK时钟: 32分频 */
#define ADC_CK_DIV64                         ADC_CFG2_PRESC_DIV64                           /**< ADC_CK时钟: 64分频 */

/* ADC转换模式 */
#define ADC_SINGLE_CONVER_MODE               ADC_CFG1_CONV_MOD_SINGLE                       /**< ADC单次扫描转换 */
#define ADC_CONTINUOUS_CONVER_MODE           ADC_CFG1_CONV_MOD_CONTINUOUS                   /**< ADC循环扫描转换 */
#define ADC_DISCONTINUOUS_CONVER_MODE        ADC_CFG1_CONV_MOD_DISCONTINUOUS                /**< ADC循环间断转换 */

/* ADC通道扫描方向 */
#define ADC_SCAN_DIR_FORWARD                 (0x00000000U)                                  /**< ADC转换通道: 正向扫描 */
#define ADC_SCAN_DIR_BACKWARD                ADC_CFG1_SDIR                                  /**< ADC转换通道: 反向扫描 */

/* ADC转换触发模式: 触发沿 */
#define ADC_TRIG_SW                          ADC_CFG1_TRIGEN_SW                             /**< 软件触发方式                 */
#define ADC_TRIG_HW_EDGE_RISING              ADC_CFG1_TRIGEN_HW_EDGE_RISING                 /**< 外部硬件触发使能: 上升沿触发 */
#define ADC_TRIG_HW_EDGE_FALLING             ADC_CFG1_TRIGEN_HW_EDGE_FALLING                /**< 外部硬件触发使能: 下降沿触发 */
#define ADC_TRIG_HW_EDGE_BOTH                ADC_CFG1_TRIGEN_HW_EDGE_BOTH                   /**< 外部硬件触发使能: 双沿触发   */

/*  ADC转换触发源 */
#define ADC_EXTRIG_TIM1_TRGO                 ADC_CFG1_TRIG_TIM1_TRGO                        /**< ADC触发源: TIM1 TRGO      */
#define ADC_EXTRIG_TIM1_OC4_ADC              ADC_CFG1_TRIG_TIM1_OC4_ADC                     /**< ADC触发源: TIM1 OC4       */
#define ADC_EXTRIG_TIM3_TRGO                 ADC_CFG1_TRIG_TIM3_TRGO                        /**< ADC触发源: TIM3 TRGO      */
#define ADC_EXTRIG_EXTI7                     ADC_CFG1_TRIG_EXTI7                            /**< ADC触发源: 外部中断EXTI_7 */

/* ADC转换溢出后ADC_DR寄存器数据存储方式 */
#define ADC_OVRN_MODE_PRESERVED              (0x00000000U)                                  /**< ADC_DR寄存器保留原数据 */
#define ADC_OVRN_MODE_OVERWRITTEN            ADC_CFG1_OVRN_MOD                              /**< ADC_DR寄存器覆盖新数据 */

/* ADC采样时间参数 */
#define ADC_SAMPTIME_3CYCLES                 ADC_SAMPT_SAMPT_3CYCLES                        /**< 采样时间为3个时钟周期    */
#define ADC_SAMPTIME_7CYCLES                 ADC_SAMPT_SAMPT_7CYCLES                        /**< 采样时间为7个时钟周期    */
#define ADC_SAMPTIME_12CYCLES                ADC_SAMPT_SAMPT_12CYCLES                       /**< 采样时间为12个时钟周期   */
#define ADC_SAMPTIME_19CYCLES                ADC_SAMPT_SAMPT_19CYCLES                       /**< 采样时间为19个时钟周期   */
#define ADC_SAMPTIME_39CYCLES                ADC_SAMPT_SAMPT_39CYCLES                       /**< 采样时间为39个时钟周期   */
#define ADC_SAMPTIME_79CYCLES                ADC_SAMPT_SAMPT_79CYCLES                       /**< 采样时间为79个时钟周期   */
#define ADC_SAMPTIME_119CYCLES               ADC_SAMPT_SAMPT_119CYCLES                      /**< 采样时间为119个时钟周期  */
#define ADC_SAMPTIME_159CYCLES               ADC_SAMPT_SAMPT_159CYCLES                      /**< 采样时间为159个时钟周期  */
#define ADC_SAMPTIME_239CYCLES               ADC_SAMPT_SAMPT_239CYCLES                      /**< 采样时间为239个时钟周期  */
#define ADC_SAMPTIME_319CYCLES               ADC_SAMPT_SAMPT_319CYCLES                      /**< 采样时间为319个时钟周期  */
#define ADC_SAMPTIME_479CYCLES               ADC_SAMPT_SAMPT_479CYCLES                      /**< 采样时间为479个时钟周期  */
#define ADC_SAMPTIME_639CYCLES               ADC_SAMPT_SAMPT_639CYCLES                      /**< 采样时间为639个时钟周期  */
#define ADC_SAMPTIME_959CYCLES               ADC_SAMPT_SAMPT_959CYCLES                      /**< 采样时间为959个时钟周期  */
#define ADC_SAMPTIME_1279CYCLES              ADC_SAMPT_SAMPT_1279CYCLES                     /**< 采样时间为1279个时钟周期 */
#define ADC_SAMPTIME_1919CYCLES              ADC_SAMPT_SAMPT_1919CYCLES                     /**< 采样时间为1919个时钟周期 */

/* ADC转换通道 */
#define ADC_CHANNEL_NONE                     (0x00000000U)                                  /**< ADC 转换通道清除     */ 
#define ADC_CHANNEL_0                        ADC_CHCFG_CHN0                                 /**< ADC 转换通道IN0      */
#define ADC_CHANNEL_1                        ADC_CHCFG_CHN1                                 /**< ADC 转换通道IN1      */
#define ADC_CHANNEL_2                        ADC_CHCFG_CHN2                                 /**< ADC 转换通道IN2      */
#define ADC_CHANNEL_3                        ADC_CHCFG_CHN3                                 /**< ADC 转换通道IN3      */
#define ADC_CHANNEL_4                        ADC_CHCFG_CHN4                                 /**< ADC 转换通道IN4      */
#define ADC_CHANNEL_5                        ADC_CHCFG_CHN5                                 /**< ADC 转换通道IN5      */
#define ADC_CHANNEL_6                        ADC_CHCFG_CHN6                                 /**< ADC 转换通道IN6      */
#define ADC_CHANNEL_7                        ADC_CHCFG_CHN7                                 /**< ADC 转换通道IN7      */
#define ADC_CHANNEL_8                        ADC_CHCFG_CHN8                                 /**< ADC 转换通道IN8      */
#define ADC_CHANNEL_VBGR                     ADC_CHCFG_CHN8                                 /**< ADC 内部转换通道VBGR */
#define ADC_CHANNEL_ALL                      ADC_CHCFG_CHN                                  /**< ADC 全部转换通道     */ 

/* ADC模拟看门狗监测通道 */
#define ADC_AWDG_CHANNEL_NONE                (0x00000000U)                                  /**< ADC 模拟看门狗监测通道清除 */ 
#define ADC_AWDG_CHANNEL_0                   ADC_AWDGCR_CHN0                                /**< ADC 模拟看门狗监测通道0    */ 
#define ADC_AWDG_CHANNEL_1                   ADC_AWDGCR_CHN1                                /**< ADC 模拟看门狗监测通道1    */ 
#define ADC_AWDG_CHANNEL_2                   ADC_AWDGCR_CHN2                                /**< ADC 模拟看门狗监测通道2    */ 
#define ADC_AWDG_CHANNEL_3                   ADC_AWDGCR_CHN3                                /**< ADC 模拟看门狗监测通道3    */ 
#define ADC_AWDG_CHANNEL_4                   ADC_AWDGCR_CHN4                                /**< ADC 模拟看门狗监测通道4    */ 
#define ADC_AWDG_CHANNEL_5                   ADC_AWDGCR_CHN5                                /**< ADC 模拟看门狗监测通道5    */ 
#define ADC_AWDG_CHANNEL_6                   ADC_AWDGCR_CHN6                                /**< ADC 模拟看门狗监测通道6    */ 
#define ADC_AWDG_CHANNEL_7                   ADC_AWDGCR_CHN7                                /**< ADC 模拟看门狗监测通道7    */ 
#define ADC_AWDG_CHANNEL_8                   ADC_AWDGCR_CHN8                                /**< ADC 模拟看门狗监测通道8    */ 
#define ADC_AWDG_CHANNEL_VBGR                ADC_AWDGCR_CHN8                                /**< ADC 模拟看门狗监测通道VBGR */ 
#define ADC_AWDG_CHANNEL_ALL                 ADC_AWDGCR_CHN                                 /**< ADC 全部转换通道           */ 

/* ADC内部通道使能 */
#define ADC_INTERNAL_CHANNEL_VBGREN          ADC_CFG2_VBGREN                                /**< ADC 内部通道VBGR使能 */ 

/* ADC中断源定义：IER中断寄存器 */
#define ADC_INTERRUPT_EOSAMP                 ADC_IER_EOSAMPIE                               /**< ADC采样结束中断           */
#define ADC_INTERRUPT_EOC                    ADC_IER_EOCIE                                  /**< ADC单通道转换完成中断     */
#define ADC_INTERRUPT_EOS                    ADC_IER_EOSIE                                  /**< ADC通道序列转换完成中断   */
#define ADC_INTERRUPT_OVRN                   ADC_IER_OVRNIE                                 /**< ADC数据溢出中断           */
#define ADC_INTERRUPT_AWDG                   ADC_IER_AWDGIE                                 /**< ADC模拟看门狗电压监控中断 */
#define ADC_INTERRUPT_EOCAL                  ADC_IER_EOCALIE                                /**< ADC校准结束中断           */

/* ADC状态定义：ISR状态寄存器 */
#define ADC_FLAG_EOSAMP                      ADC_ISR_EOSAMP                                 /**< ADC采样结束状态           */
#define ADC_FLAG_EOC                         ADC_ISR_EOC                                    /**< ADC单通道转换完成状态     */
#define ADC_FLAG_EOS                         ADC_ISR_EOS                                    /**< ADC通道序列转换完成状态   */
#define ADC_FLAG_OVRN                        ADC_ISR_OVRN                                   /**< ADC数据溢出状态           */
#define ADC_FLAG_AWDG                        ADC_ISR_AWDG                                   /**< ADC模拟看门狗监控电压状态 */
#define ADC_FLAG_EOCAL                       ADC_ISR_EOCAL                                  /**< ADC校准状态               */
#define ADC_FLAG_ALL                         (ADC_ISR_EOSAMP | ADC_ISR_EOC | ADC_ISR_EOS \
                                              | ADC_ISR_OVRN | ADC_ISR_AWDG | ADC_ISR_EOCAL)/**< ADC全部状态               */
                                              
/* ADC工作模式定义 */
#define ADC_MODE_INTERVAL                    (0x00000000U)                                  /**< ADC间歇工作模式           */
#define ADC_MODE_NORMAL                      ADC_CFG3_MODE                                  /**< ADC正常工作模式           */
                                              

/* VBGR校准参数定义 */  
#define VBGR_CAL_ADDR                        ((uint16_t *)(BGR_CAL))                        /**< VBGR校准参数存储地址       */
#define VBGR_CAL_VREF                        (3300U)                                        /**< VBGR校准采用参考电压：3.3V */

/* 等待内部通道VBGR的启动稳定。稳定时间最大值参考CIU32F003的数据手册(tADC_BUF参数) */
#define ADC_VBGR_CHANNEL_DELAY               (22U)                                          /**< VBGR通道启动稳定时间：22us */

/* ADC使能稳定时间，需等待1us稳定时间 */
#define ADC_EN_DELAY                         (1U)                                           /**< ADC使能稳定时间 */

/* ADC最大采样值 */
#define ADC_CONVER_SCALE                     (4095U)                                        /**< ADC最大采样值，用于ADC电压转换计算 */

/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup ADC_External_Functions ADC External Functions
* @brief    ADC对外函数
* @{
*
*/
/************************************************************************************************/
/** 
* @brief  使能ADC
* @retval 无
*/
__STATIC_INLINE void std_adc_enable(void)
{
    ADC->CR = (ADC_CR_ADEN);
}

/** 
* @brief  禁止ADC
* @retval 无
*/
__STATIC_INLINE void std_adc_disable(void)
{
    ADC->CR = (ADC_CR_ADDIS);
}

/** 
* @brief  获取ADC使能位状态
* @retval uint32_t 返回逻辑表达式的判断结果
*             @arg 非0： 表示ADC处于使能状态
*             @arg 0：表示ADC处于禁止状态
*/
__STATIC_INLINE uint32_t std_adc_get_enable_status(void)
{
    return ((ADC->CR & ADC_CR_ADEN));
}

/** 
* @brief  使能ADC校准
* @note   当ADEN=1，ADC稳定后，且START=0、STOP=0、ADDIS=0，才允许通过软件将CALEN位置1
* @retval 无
*/
__STATIC_INLINE void std_adc_calibration_enable(void)
{
    ADC->CR = (ADC_CR_CALEN);
}

/** 
* @brief  启动ADC转换 
* @retval 无
*/
__STATIC_INLINE void std_adc_start_conversion(void)
{
    ADC->CR = (ADC_CR_START);
}

/** 
* @brief  获取ADC工作状态
* @retval uint32_t 返回ADC工作状态
*             @arg 非0： 表示ADC正处于工作状态
*             @arg 0：表示ADC处于未工作状态
*/
__STATIC_INLINE uint32_t std_adc_get_conversion_status(void)
{
    return ((ADC->CR & ADC_CR_START));
}

/** 
* @brief  ADC停止转换
* @note   仅当START=1且ADDIS=0时，软件将STOP位置1生效
* @retval 无
*/
__STATIC_INLINE void std_adc_stop_conversion(void)
{
    ADC->CR = (ADC_CR_STOP);
}

/** 
* @brief  ADC中断使能
* @param  interrupt ADC中断源   
*             @arg ADC_INTERRUPT_EOSAMP
*             @arg ADC_INTERRUPT_EOC
*             @arg ADC_INTERRUPT_EOS
*             @arg ADC_INTERRUPT_OVRN
*             @arg ADC_INTERRUPT_AWDG
*             @arg ADC_INTERRUPT_EOCAL
* @retval 无
*/
__STATIC_INLINE void std_adc_interrupt_enable(uint32_t interrupt)
{
    ADC->IER |= (interrupt);
}

/** 
* @brief  ADC中断禁止
* @param  interrupt ADC中断源   
*             @arg ADC_INTERRUPT_EOSAMP
*             @arg ADC_INTERRUPT_EOC
*             @arg ADC_INTERRUPT_EOS
*             @arg ADC_INTERRUPT_OVRN
*             @arg ADC_INTERRUPT_AWDG
*             @arg ADC_INTERRUPT_EOCAL
* @retval 无
*/
__STATIC_INLINE void std_adc_interrupt_disable(uint32_t interrupt)
{
    ADC->IER &= (~interrupt);
}

/** 
* @brief  获取ADC中断源使能状态
* @param  interrupt ADC中断源信息 
*             @arg ADC_INTERRUPT_EOSAMP
*             @arg ADC_INTERRUPT_EOC
*             @arg ADC_INTERRUPT_EOS
*             @arg ADC_INTERRUPT_OVRN
*             @arg ADC_INTERRUPT_AWDG
*             @arg ADC_INTERRUPT_EOCAL
* @retval uint32_t 返回中断源使能状态
*             @arg 非0： 表示指定的中断使能
*             @arg 0：表示指定的中断未使能
*/

__STATIC_INLINE uint32_t std_adc_get_interrupt_enable(uint32_t interrupt)
{
    return((ADC->IER & (interrupt)));
}

/** 
* @brief  获取ADC标志状态
* @param  flag 获取ADC标志
*             @arg ADC_FLAG_EOSAMP
*             @arg ADC_FLAG_EOC
*             @arg ADC_FLAG_EOS
*             @arg ADC_FLAG_OVRN
*             @arg ADC_FLAG_AWDG
*             @arg ADC_FLAG_EOCAL
* @retval uint32_t 返回标志位状态
*             @arg 非0：表示当前标志为置位状态
*             @arg 0：表示当前标志为清除状态
*/
__STATIC_INLINE uint32_t std_adc_get_flag(uint32_t flag)
{
    return((ADC->ISR & (flag)));
}

/** 
* @brief  清除ADC标志
* @param  flag 清除ADC标志
*             @arg ADC_FLAG_EOSAMP
*             @arg ADC_FLAG_EOC
*             @arg ADC_FLAG_EOS
*             @arg ADC_FLAG_OVRN
*             @arg ADC_FLAG_AWDG
*             @arg ADC_FLAG_EOCAL
* @retval 无
*/
__STATIC_INLINE void std_adc_clear_flag(uint32_t flag)
{
    ADC->ISR = (flag);
}

/** 
* @brief  配置ADC转换模式
* @param  conversion_mode 转换模式选择
*             @arg ADC_SINGLE_CONVER_MODE
*             @arg ADC_CONTINUOUS_CONVER_MODE
*             @arg ADC_DISCONTINUOUS_CONVER_MODE
* @note   当START=0时，软件对此位域执行写操作生效
* @retval 无
*/
__STATIC_INLINE void std_adc_conversion_mode_config(uint32_t conversion_mode)
{
    MODIFY_REG(ADC->CFG1, ADC_CFG1_CONV_MOD, conversion_mode);
}

/** 
* @brief  ADC通道使能
* @param  channel 通道号选择
*             @arg ADC_CHANNEL_0
*             @arg ADC_CHANNEL_1
*             @arg ...
*             @arg ADC_CHANNEL_ALL
* @retval 无
*/
__STATIC_INLINE void std_adc_fix_sequence_channel_enable(uint32_t channel)
{
    ADC->CHCFG |= (channel);
}

/** 
* @brief  ADC通道禁止
* @param  channel 禁止通道选择
*             @arg ADC_CHANNEL_0
*             @arg ADC_CHANNEL_1
*             @arg ...
*             @arg ADC_CHANNEL_ALL
* @retval 无
*/
__STATIC_INLINE void std_adc_fix_sequence_channel_disable(uint32_t channel)
{
    ADC->CHCFG &= (~channel);
}

/** 
* @brief  配置ADC触发源为软件触发
* @retval 无
*/
__STATIC_INLINE void std_adc_trig_sw(void)
{
    MODIFY_REG(ADC->CFG1, ADC_CFG1_TRIGEN, ADC_TRIG_SW);
}

/** 
* @brief  配置ADC触发信号源和极性
* @param  trig_edge 触发极性选择
*             @arg ADC_TRIG_SW(软件触发方式)
*             @arg ADC_TRIG_HW_EDGE_RISING
*             @arg ADC_TRIG_HW_EDGE_FALLING
*             @arg ADC_TRIG_HW_EDGE_BOTH
* @param  trig_source 外部触发源性选择
*             @arg ADC_EXTRIG_TIM1_TRGO
*             @arg ADC_EXTRIG_TIM1_OC4_ADC
*             @arg ADC_EXTRIG_TIM3_TRGO
*             @arg ADC_EXTRIG_EXTI7
* @note   当选择ADC_TRIG_SW(软件触发方式)时，触发源参数无意义。
* @retval 无
*/
__STATIC_INLINE void std_adc_trig_config(uint32_t trig_edge,uint32_t trig_source)
{
    MODIFY_REG(ADC->CFG1, (ADC_CFG1_TRIGEN | ADC_CFG1_TRIG_SEL), (trig_edge | trig_source));
}

/** 
* @brief  ADC采样时间参数1配置
* @param  sample_time 采样时间选择
*           @arg ADC_SAMPTIME_3CYCLES
*           @arg ...
*           @arg ADC_SAMPTIME_1919CYCLES 
* @retval 无
*/
__STATIC_INLINE void std_adc_sampt_time_config(uint32_t sample_time)
{
    MODIFY_REG(ADC->SAMPT, ADC_SAMPT_SAMPT, sample_time);
}

/** 
* @brief  配置ADC_CK时钟分频
* @param  presc ADC_CK时钟源分频系数
*             @arg  ADC_CK_DIV1： ADC_CK 不分频
*             @arg  ADC_CK_DIV2： ADC_CK 2分频
*             @arg  ADC_CK_DIV3： ADC_CK 3分频
*             @arg ...
*             @arg  ADC_CK_DIV64：ADC_CK 64分频
* @retval 无
*/
__STATIC_INLINE void std_adc_clock_config(uint32_t presc)
{
    MODIFY_REG(ADC->CFG2, ADC_CFG2_PRESC, presc);
}

/** 
* @brief  配置ADC校准系数
* @param  calibration_factor 参数范围为0x00~0x3F
* @retval 无
*/
__STATIC_INLINE void std_adc_calibration_factor_config(uint32_t calibration_factor)
{
    ADC->CALFACT = calibration_factor;
}

/** 
* @brief  获取ADC校准系数
* @retval uint16_t ADC校准系数
*/
__STATIC_INLINE uint16_t std_adc_get_calibration_factor(void)
{
    return ((uint16_t)(ADC->CALFACT));
}

/** 
* @brief  使能ADC等待模式
* @note   用于避免数据未及时读取，转换溢出
* @retval 无
*/
__STATIC_INLINE void std_adc_wait_mode_enable(void)
{
    ADC->CFG1 |= (ADC_CFG1_WAIT_MOD);
}

/** 
* @brief  禁止ADC等待模式
* @retval 无
*/
__STATIC_INLINE void std_adc_wait_mode_disable(void)
{
    ADC->CFG1 &= (~ADC_CFG1_WAIT_MOD);
}

/** 
* @brief  配置通道序列扫描方向
* @param  dir ADC通道扫描方向
*             @arg ADC_SCAN_DIR_FORWARD
*             @arg ADC_SCAN_DIR_BACKWARD
* @retval 无
*/
__STATIC_INLINE void std_adc_scan_direction_config(uint32_t dir)
{
    MODIFY_REG(ADC->CFG1, ADC_CFG1_SDIR, dir);
}

/** 
* @brief  配置ADC数据溢出管理方式
* @param  ovrn_mode ADC数据溢出管理方式
*             @arg ADC_OVRN_MODE_PRESERVED
*             @arg ADC_OVRN_MODE_OVERWRITTEN
* @retval 无
*/
__STATIC_INLINE void std_adc_ovrn_mode_config(uint32_t ovrn_mode)
{
    MODIFY_REG(ADC->CFG1, ADC_CFG1_OVRN_MOD, ovrn_mode);
}

/** 
* @brief  ADC内部通道VBGR使能
* @retval 无
*/
__STATIC_INLINE void std_adc_internal_channel_vbgr_enable(void)
{
    ADC->CFG2 |= (ADC_INTERNAL_CHANNEL_VBGREN);
}

/** 
* @brief  ADC内部通道VBGR禁止
* @retval 无
*/
__STATIC_INLINE void std_adc_internal_channel_vbgr_disable(void)
{
    ADC->CFG2 &= (~ADC_INTERNAL_CHANNEL_VBGREN);
}

/** 
* @brief  选择ADC模拟看门狗监控通道
* @param  channel ADC看门狗监控通道选择
*             @arg ADC_AWDG_CHANNEL_NONE
*             @arg ADC_AWDG_CHANNEL_0
*             @arg ...
*             @arg ADC_AWDG_CHANNEL_ALL
* @retval 无
*/
__STATIC_INLINE void std_adc_analog_watchdog_monit_channel(uint32_t channel)
{
    MODIFY_REG(ADC->AWDGCR, ADC_AWDGCR_CHN, channel);
}

/** 
* @brief  配置看门狗监控通道电压阈值
* @param  high_threshold 阈值上限选择范围为 0x000~0xFFF
* @param  low_threshold  阈值下限选择范围为 0x000~0xFFF
* @retval 无
*/
__STATIC_INLINE void std_adc_analog_watchdog_thresholds_config(uint32_t high_threshold, uint32_t low_threshold)
{
    MODIFY_REG(ADC->AWDGTR, (ADC_AWDGTR_AWDG_LT | ADC_AWDGTR_AWDG_HT), (high_threshold << ADC_AWDGTR_AWDG_HT_POS) | (low_threshold));
}

/** 
* @brief  获取ADC采样值
* @retval uint16_t ADC转换值
*/
__STATIC_INLINE uint16_t std_adc_get_conversion_value(void)
{
    return ((uint16_t)(ADC->DR));
}

/** 
* @brief  获取校准后的参考电压值
* @param  vbgr_sample_data ADC采集BGR的转换值
* @retval uint32_t 校准后的参考电压值
*/
__STATIC_INLINE uint32_t std_adc_calc_vref_voltage(uint16_t vbgr_sample_data)
{
    return ((VBGR_CAL_VREF * (*VBGR_CAL_ADDR))/vbgr_sample_data);
}

/** 
* @brief  配置ADC工作模式
* @param  mode_sel ADC工作模式
*             @arg ADC_MODE_INTERVAL
*             @arg ADC_MODE_NORMAL
* @retval 无
*/
__STATIC_INLINE void std_adc_mode_config(uint32_t mode_sel)
{
    MODIFY_REG(ADC->CFG3, ADC_CFG3_MODE_MASK, mode_sel);
}

/* ADC去初始化函数 */
void std_adc_deinit(void);

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

#endif /* CIU32F003_STD_ADC_H */

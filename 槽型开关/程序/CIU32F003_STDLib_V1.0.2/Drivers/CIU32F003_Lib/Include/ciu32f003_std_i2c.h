/************************************************************************************************/
/**
* @file               ciu32f003_std_i2c.h
* @author             MCU Ecosystem Development Team
* @brief              I2C STD库驱动头文件。
*                     提供I2C相关的STD库操作函数声明、数据类型以及常量的定义。                         
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef CIU32F003_STD_I2C_H
#define CIU32F003_STD_I2C_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup I2C I2C
* @brief I2C接口的STD库驱动
* @{
*/
/************************************************************************************************/


#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------includes--------------------------------------------------*/
#include "ciu32f003_std_common.h"

/*--------------------------------------------define--------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup I2C_Constants I2C Constants
* @brief    I2C常量定义及宏定义
* @{
*
*/
/************************************************************************************************/
/* 数字滤波器配置 */
#define I2C_DIGITALFILTER_DISABLE       I2C_CR1_DNF_DISABLE         /**< 数字滤波器禁止             */
#define I2C_DIGITALFILTER_1CLK          I2C_CR1_DNF_1CLK            /**< 滤除小于1个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_2CLK          I2C_CR1_DNF_2CLK            /**< 滤除小于2个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_3CLK          I2C_CR1_DNF_3CLK            /**< 滤除小于3个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_4CLK          I2C_CR1_DNF_4CLK            /**< 滤除小于4个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_5CLK          I2C_CR1_DNF_5CLK            /**< 滤除小于5个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_6CLK          I2C_CR1_DNF_6CLK            /**< 滤除小于6个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_7CLK          I2C_CR1_DNF_7CLK            /**< 滤除小于7个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_8CLK          I2C_CR1_DNF_8CLK            /**< 滤除小于8个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_9CLK          I2C_CR1_DNF_9CLK            /**< 滤除小于9个I2C_KCLK的噪声  */
#define I2C_DIGITALFILTER_10CLK         I2C_CR1_DNF_10CLK           /**< 滤除小于10个I2C_KCLK的噪声 */
#define I2C_DIGITALFILTER_11CLK         I2C_CR1_DNF_11CLK           /**< 滤除小于11个I2C_KCLK的噪声 */
#define I2C_DIGITALFILTER_12CLK         I2C_CR1_DNF_12CLK           /**< 滤除小于12个I2C_KCLK的噪声 */
#define I2C_DIGITALFILTER_13CLK         I2C_CR1_DNF_13CLK           /**< 滤除小于13个I2C_KCLK的噪声 */
#define I2C_DIGITALFILTER_14CLK         I2C_CR1_DNF_14CLK           /**< 滤除小于14个I2C_KCLK的噪声 */
#define I2C_DIGITALFILTER_15CLK         I2C_CR1_DNF_15CLK           /**< 滤除小于15个I2C_KCLK的噪声 */

/* I2C 中断源配置 */
#define I2C_INTERRUPT_ERR               I2C_CR1_ERRIE               /**< 错误中断        */
#define I2C_INTERRUPT_BUF               I2C_CR1_BUFIE               /**< 缓冲器中断      */
#define I2C_INTERRUPT_EVT               I2C_CR1_EVTIE               /**< 事件中断        */

/* I2C 状态标志位 */
#define I2C_FLAG_TXE                    I2C_ISR_TXE                 /**< 发送数据寄存器为空标志    */
#define I2C_FLAG_TXIS                   I2C_ISR_TXIS                /**< 发送中断状态标志          */
#define I2C_FLAG_RXNE                   I2C_ISR_RXNE                /**< 接收数据寄存器非空标志    */
#define I2C_FLAG_ADDR                   I2C_ISR_ADDR                /**< 地址匹配标志              */
#define I2C_FLAG_NACK                   I2C_ISR_NACKF               /**< 接收NACK标志              */
#define I2C_FLAG_STOP                   I2C_ISR_STOPF               /**< 停止位检测标志            */
#define I2C_FLAG_BERR                   I2C_ISR_BERR                /**< 总线错误标志              */
#define I2C_FLAG_OVR                    I2C_ISR_OVR                 /**< 溢出标志                  */
#define I2C_FLAG_BUSY                   I2C_ISR_BUSY                /**< 总线被占用标志            */
#define I2C_FLAG_DIR                    I2C_ISR_DIR                 /**< 数据传输方向标志          */

/* I2C 清除状态位 */
#define I2C_CLEAR_ADDR                  I2C_ICR_ADDRCF              /**< 清除地址匹配标志          */
#define I2C_CLEAR_NACK                  I2C_ICR_NACKCF              /**< 清除NACK标志              */
#define I2C_CLEAR_STOP                  I2C_ICR_STOPCF              /**< 清除停止位检测标志        */
#define I2C_CLEAR_BERR                  I2C_ICR_BERRCF              /**< 清除总线错误标志          */
#define I2C_CLEAR_OVR                   I2C_ICR_OVRCF               /**< 清除溢出标志              */

/* I2C传输方向 */
#define I2C_DIR_RX                      (0x00000000U)               /**< I2C接收数据               */
#define I2C_DIR_TX                      I2C_ISR_DIR                 /**< I2C发送数据               */

/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup I2C_External_Functions I2C External Functions
* @brief    I2C对外函数
* @{
*
*/
/************************************************************************************************/
/** 
* @brief  使能I2C接口
* @retval 无
*/
__STATIC_INLINE void std_i2c_enable(void)
{
    I2C1->CR1 |= I2C_CR1_PE;
}


/** 
* @brief  禁止I2C接口
* @retval 无
*/
__STATIC_INLINE void std_i2c_disable(void)
{
    I2C1->CR1 &= (~I2C_CR1_PE);
}


/** 
* @brief  配置数字滤波器
* @param  digital_filter 数字滤波器配置参数
*             @arg I2C_DIGITALFILTER_DISABLE: 禁止数字滤波器
*             @arg I2C_DIGITALFILTER_1CLK:    滤波脉宽小于1 * I2C_KCLK
*             @arg I2C_DIGITALFILTER_2CLK:    滤波脉宽小于2 * I2C_KCLK
*             @arg ...
*             @arg I2C_DIGITALFILTER_15CLK:   滤波脉宽小于15 * I2C_KCLK
* @note   滤波器配置只能在I2C禁止的状态下条件下有效
* @retval 无
*/
__STATIC_INLINE void std_i2c_digital_filter_config(uint32_t digital_filter)
{
    MODIFY_REG(I2C1->CR1, I2C_CR1_DNF, digital_filter);
}


/** 
* @brief  I2C从模式时钟延长功能开启
* @retval 无
*/
__STATIC_INLINE void std_i2c_clock_stretch_enable(void)
{
    I2C1->CR1 &= (~I2C_CR1_NOSTRETCH);
}


/** 
* @brief  I2C从模式时钟延长功能禁止
* @retval 无
*/
__STATIC_INLINE void std_i2c_clock_stretch_disable(void)
{
    I2C1->CR1 |= I2C_CR1_NOSTRETCH;
}


/** 
* @brief  I2C广播地址应答使能
* @retval 无
*/
__STATIC_INLINE void std_i2c_general_call_address_enable(void)
{
    I2C1->CR1 |= I2C_CR1_GCEN;
}


/** 
* @brief  I2C广播地址应答禁止
* @retval 无
*/
__STATIC_INLINE void std_i2c_general_call_address_disable(void)
{
    I2C1->CR1 &= (~I2C_CR1_GCEN);
}


/** 
* @brief  设置I2C从模式地址1
* @param  dev_address I2C设备地址1（该变量的范围在0x0~0x7F之间）
* @retval 无
*/
__STATIC_INLINE void std_i2c_device_address1_config(uint32_t dev_address)
{
    I2C1->ADDR1 = dev_address;
}


/** 
* @brief  使能I2C中断
* @param  interrupt 使能I2C中断源选择
*             @arg I2C_INTERRUPT_ERR:  I2C错误中断
*             @arg I2C_INTERRUPT_BUF:  I2C缓冲器中断
*             @arg I2C_INTERRUPT_EVT:  I2C事件中断
* @retval 无
*/
__STATIC_INLINE void std_i2c_interrupt_enable(uint32_t interrupt)
{
    I2C1->CR1 |= interrupt;
}


/** 
* @brief  禁止I2C中断
* @param  interrupt I2C中断选择
*             @arg I2C_INTERRUPT_ERR:  I2C错误中断
*             @arg I2C_INTERRUPT_BUF:  I2C缓冲器中断
*             @arg I2C_INTERRUPT_EVT:  I2C事件中断
* @retval 无
*/
__STATIC_INLINE void std_i2c_interrupt_disable(uint32_t interrupt)
{
    I2C1->CR1 &= (~interrupt);
}


/** 
* @brief  获取I2C中断使能状态
* @param  interrupt I2C中断选择
*             @arg I2C_INTERRUPT_ERR:  I2C错误中断
*             @arg I2C_INTERRUPT_BUF:  I2C缓冲器中断
*             @arg I2C_INTERRUPT_EVT:  I2C事件中断
* @retval uint32_t 返回选择的I2C中断使能状态
*             @arg 非0: 使能
*             @arg 0:   禁止
*/
__STATIC_INLINE uint32_t std_i2c_get_interrupt_enable(uint32_t interrupt)
{
    return (I2C1->CR1 & interrupt);
}


/** 
* @brief  获取I2C状态标志位或数据传输方向
* @param  flag I2C状态标志位选择
*             @arg I2C_FLAG_TXE:      I2C发送数据寄存器为空
*             @arg I2C_FLAG_TXIS:     I2C发送中断状态
*             @arg I2C_FLAG_RXNE:     I2C接收数据寄存器非空
*             @arg ...
*             @arg I2C_FLAG_DIR:      I2C数据传输方向标志
* @retval uint32_t 返回选择的I2C状态标志位状态
*             @arg 非0: 标志位置位
*             @arg 0:   标志位清除
*/
__STATIC_INLINE uint32_t std_i2c_get_flag(uint32_t flag)
{
    return (I2C1->ISR & flag);
}


/** 
* @brief  清除I2C状态标志位
* @param  flag I2C状态标志位选择
*             @arg I2C_CLEAR_ADDR:     清除ADDR标志
*             @arg I2C_CLEAR_NACK:     清除NACKF标志
*             @arg I2C_CLEAR_STOP:     清除STOPF标志
*             @arg I2C_CLEAR_BERR:     清除BERR标志
*             @arg I2C_CLEAR_OVR:      清除OVR标志
* @retval 无
*/
__STATIC_INLINE void std_i2c_clear_flag(uint32_t flag)
{
    I2C1->ICR = flag;
}


/** 
* @brief  配置从模式下地址匹配或接收下个数据时返回NACK应答
* @retval 无
*/
__STATIC_INLINE void std_i2c_set_next_data_nack(void)
{
    I2C1->CR2 = I2C_CR2_NACK;
}


/** 
* @brief  读数据寄存器
* @retval uint8_t 接收数据寄存器中的值（该值范围为0x00~0xFF）
*/
__STATIC_INLINE uint8_t std_i2c_receive_byte(void)
{
    return (uint8_t)(I2C1->RDR);
}


/** 
* @brief  写数据寄存器
* @param  send_data 写入I2C TDR寄存器中的值（该变量范围为0x00~0xFF）
* @retval 无
*/
__STATIC_INLINE void std_i2c_transmit_byte(uint8_t send_data)
{
    I2C1->TDR = send_data;
}


/** 
* @brief  清除发送数据寄存器
* @retval 无
*/
__STATIC_INLINE void std_i2c_clear_tx_data(void)
{
    I2C1->ISR = I2C_ISR_TXE;
}


/* I2C去初始化函数 */
void std_i2c_deinit(void);

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

#endif /* CIU32F003_STD_I2C_H */

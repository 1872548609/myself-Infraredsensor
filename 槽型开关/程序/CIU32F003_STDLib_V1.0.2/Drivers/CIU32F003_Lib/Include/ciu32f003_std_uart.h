/************************************************************************************************/
/**
* @file               ciu32f003_std_uart.h
* @author             MCU Ecosystem Development Team
* @brief              UART STD库驱动头文件。
*                     提供UART相关的STD库操作函数声明、数据类型以及常量的定义。                         
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef CIU32F003_STD_UART_H
#define CIU32F003_STD_UART_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup UART UART
* @brief 通用异步收发器的STD库驱动
* @{
*/
/************************************************************************************************/

#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------includes--------------------------------------------------*/
#include "ciu32f003_std_common.h"

/*------------------------------------type define-----------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup UART_Types UART Types 
* @brief  UART数据类型定义
* @{
*
*/
/************************************************************************************************/

/**
* @brief  UART初始化配置结构体定义
*/
typedef struct
{
    uint32_t direction;                                      /**< UART 传输模式(传输方向)
                                                                      @arg UART_DIRECTION_SEND ...                     */
    uint32_t baudrate;                                       /**< UART 波特率                                          */
    
    uint32_t wordlength;                                     /**< UART 数据帧字长
                                                                      @arg UART_WORDLENGTH_8BITS ...                   */
    uint32_t stopbits;                                       /**< UART 数据帧停止位长度
                                                                      @arg UART_STOPBITS_1 ...                         */
    uint32_t parity;                                         /**< UART 数据帧奇偶校验
                                                                      @arg UART_PARITY_NONE ...                        */
}std_uart_init_t;

/**
* @}
*/

/*--------------------------------------------define--------------------------------------------*/
/************************************************************************************************/
/**
* @defgroup UART_Constants UART Constants
* @brief    UART常量定义及宏定义
* @{
*
*/
/************************************************************************************************/

/* UART 数据帧长度 */
#define  UART_WORDLENGTH_8BITS                 UART_CR1_WL8BITS                     /**< 8-bits 字长 */
#define  UART_WORDLENGTH_9BITS                 UART_CR1_WL9BITS                     /**< 9-bits 字长 */

/* UART 数据帧奇偶校验 */
#define  UART_PARITY_NONE                      (0x00000000U)                         /**< 无校验 */
#define  UART_PARITY_EVEN                       UART_CR1_PEN                         /**< 偶校验 */
#define  UART_PARITY_ODD                       (UART_CR1_PEN | UART_CR1_PTS)         /**< 奇校验 */

/* UART传输模式 */
#define  UART_DIRECTION_NONE                   (0x00000000U)                         /**< 未使能发送器和接收器          */
#define  UART_DIRECTION_SEND                   UART_CR1_TE                           /**< 单发送模式(仅使能发送器)      */
#define  UART_DIRECTION_RECEIVE                UART_CR1_RE                           /**< 单接收模式(仅使能接收器)      */
#define  UART_DIRECTION_SEND_RECEIVE           (UART_CR1_TE |UART_CR1_RE)            /**< 发送/接收(使能发送器和接收器) */

/* UART 数据帧大小端 */
#define  UART_DATA_ORDER_LSBFIRST              (0x00000000U)                         /**< 低有效位优先 */
#define  UART_DATA_ORDER_MSBFIRST              UART_CR2_MSBFIRST                     /**< 高有效位优先 */

/* UART 数据帧停止位 */
#define  UART_STOPBITS_1                       UART_CR2_STOPBIT_1                    /**< 1bit停止位    */
#define  UART_STOPBITS_2                       UART_CR2_STOPBIT_2                    /**< 2bits停止位   */

/* UART 数据帧采样策略 */
#define  UART_SAMPLE_THREE_BIT                 (0x00000000U)                         /**< Three-bit采样，支持噪声检测 */
#define  UART_SAMPLE_ONE_BIT                   UART_CR3_OBS                          /**< One-bit采样，不支持噪声检测 */

/*UART中断使能位 */
#define  UART_CR1_INTERRUPT_RXNE               UART_CR1_RXNEIE                       /**< UART 接收数据寄存器非空中断使能                   */
#define  UART_CR1_INTERRUPT_TC                 UART_CR1_TCIE                         /**< UART 发送完成中断使能                             */
#define  UART_CR1_INTERRUPT_TXE                UART_CR1_TXEIE                        /**< UART 发送数据寄存器空中断使能                     */
#define  UART_CR1_INTERRUPT_PE                 UART_CR1_PEIE                         /**< UART 奇偶校验错误中断使能                         */

/* UART中断标志清除位 */
#define  UART_CLEAR_PE                         UART_ICR_PECF                         /**< 奇偶校验错误标志清除 */
#define  UART_CLEAR_FE                         UART_ICR_FECF                         /**< 帧错误标志清除       */
#define  UART_CLEAR_NOISE                      UART_ICR_NOISECF                      /**< 噪声错误标志清除     */
#define  UART_CLEAR_ORE                        UART_ICR_ORECF                        /**< 接收溢出错误标志清除 */
#define  UART_CLEAR_TC                         UART_ICR_TCCF                         /**< 发送完成标志清除     */

/* UART 状态标志 */
#define  UART_FLAG_RECEIVE_BUSY                UART_ISR_BUSY                         /**< UART 接收忙(接收管脚上有数据传输)          */
#define  UART_FLAG_TXE                         UART_ISR_TXE                          /**< UART 发送数据寄存器空标志                  */
#define  UART_FLAG_TC                          UART_ISR_TC                           /**< UART 发送完成标志                          */
#define  UART_FLAG_RXNE                        UART_ISR_RXNE                         /**< UART 接收数据寄存器非空标志                */
#define  UART_FLAG_ORE                         UART_ISR_ORE                          /**< UART 接收溢出错误标志                      */
#define  UART_FLAG_NOISE                       UART_ISR_NOISE                        /**< UART 噪声错误标志                          */
#define  UART_FLAG_FE                          UART_ISR_FE                           /**< UART 帧错误标志                            */
#define  UART_FLAG_PE                          UART_ISR_PE                           /**< UART 奇偶校验错误标志                      */
#define  UART_FLAG_ERR                         (UART_FLAG_ORE | UART_FLAG_NOISE \
                                                              | UART_FLAG_FE)        /**< UART 错误标志（上溢错误、噪声错误、帧错误）*/       

/**
* @}
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup UART_External_Functions UART External Functions
* @brief    UART对外函数
* @{
*
*/
/************************************************************************************************/

/** 
* @brief  使能UART
* @param  uartx UART外设 
* @retval 无
*/
__STATIC_INLINE void std_uart_enable(UART_t *uartx)
{
    uartx->CR1 |= (UART_CR1_UE);    
}

/** 
* @brief  禁止UART
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_disable(UART_t *uartx)
{
    uartx->CR1 &= (~UART_CR1_UE);
}

/** 
* @brief  设置UART字符长度
* @param  uartx UART外设
* @param  word_length UART 字符长度
*             @arg UART_WORDLENGTH_8BITS
*             @arg UART_WORDLENGTH_9BITS 
* @retval 无
*/
__STATIC_INLINE void std_uart_set_word_length(UART_t *uartx, uint32_t word_length)
{
    MODIFY_REG(uartx->CR1, UART_CR1_WL, word_length);
}

/** 
* @brief  获取UART字符长度
* @param  uartx UART外设
* @retval uint32_t UART 字符长度
*             @arg UART_WORDLENGTH_8BITS
*             @arg UART_WORDLENGTH_9BITS 
*/
__STATIC_INLINE uint32_t std_uart_get_word_length(UART_t *uartx)
{
    return (uartx->CR1 & UART_CR1_WL);
}

/** 
* @brief  设置UART奇偶校验
* @param  uartx UART外设
* @param  parity UART奇偶校验项
*             @arg UART_PARITY_NONE
*             @arg UART_PARITY_EVEN
*             @arg UART_PARITY_ODD 
* @retval 无
*/
__STATIC_INLINE void std_uart_set_parity(UART_t *uartx,uint32_t parity)
{
    MODIFY_REG(uartx->CR1,(UART_CR1_PTS | UART_CR1_PEN), parity);
} 

/** 
* @brief  获取UART奇偶校验
* @param  uartx UART外设
* @retval uint32_t UART奇偶校验项
*             @arg UART_PARITY_NONE
*             @arg UART_PARITY_EVEN
*             @arg UART_PARITY_ODD 
*/
__STATIC_INLINE uint32_t std_uart_get_parity(UART_t *uartx )
{
    return(uartx->CR1 & (UART_CR1_PTS | UART_CR1_PEN));
} 

/** 
* @brief  设置UART停止位
* @param  uartx UART外设
* @param  stopbits UART停止位位数 
*             @arg UART_STOPBITS_1     
*             @arg UART_STOPBITS_2    
* @retval 无
*/
__STATIC_INLINE void std_uart_set_stopbits(UART_t *uartx, uint32_t stopbits)
{
    MODIFY_REG(uartx->CR2, UART_CR2_STOPBIT, stopbits);
}

/** 
* @brief  获取UART停止位
* @param  uartx UART外设
* @retval uint32_t UART停止位位数 
*             @arg UART_STOPBITS_1    
*             @arg UART_STOPBITS_2    
*/
__STATIC_INLINE uint32_t std_uart_get_stopbits(UART_t *uartx)
{
    return(uartx->CR2 & UART_CR2_STOPBIT);
}

/** 
* @brief  设置UART字符大小端
* @param  uartx UART外设
* @param  data_order UART 字符帧格式大小端
*             @arg UART_DATA_ORDER_LSBFIRST
*             @arg UART_DATA_ORDER_MSBFIRST
* @retval 无
*/
__STATIC_INLINE void std_uart_set_data_order(UART_t *uartx, uint32_t data_order)
{
    MODIFY_REG(uartx->CR2, UART_CR2_MSBFIRST, data_order);
}

/** 
* @brief  获取UART字符大小端
* @param  uartx UART外设
* @retval uint32_t UART 字符帧格式大小端
*             @arg UART_DATA_ORDER_LSBFIRST
*             @arg UART_DATA_ORDER_MSBFIRST
*/
__STATIC_INLINE uint32_t std_uart_get_data_order(UART_t *uartx)
{
    return(uartx->CR2 & UART_CR2_MSBFIRST);
}

/** 
* @brief  设置UART采样策略
* @param  uartx UART外设
* @param  sample_method UART采样策略
*             @arg UART_SAMPLE_THREE_BIT
*             @arg UART_SAMPLE_ONE_BIT
* @retval 无
*/
__STATIC_INLINE void std_uart_set_sample_method(UART_t *uartx,uint32_t sample_method)
{
    MODIFY_REG(uartx->CR3,UART_CR3_OBS, sample_method);
}

/** 
* @brief  获取UART采样策略
* @param  uartx UART外设
* @retval uint32_t UART采样策略
*             @arg UART_SAMPLE_THREE_BIT
*             @arg UART_SAMPLE_ONE_BIT
*/
__STATIC_INLINE uint32_t std_uart_get_sample_method(UART_t *uartx)
{
    return (uartx->CR3 & UART_CR3_OBS);
}

/** 
* @brief  设置UART传输方向
* @param  uartx UART外设
* @param  direction UART传输方向
*             @arg UART_DIRECTION_NONE
*             @arg UART_DIRECTION_SEND                                                                                        
*             @arg UART_DIRECTION_RECEIVE                                                      
*             @arg UART_DIRECTION_SEND_RECEIVE                                                                                                               
* @retval 无
*/
__STATIC_INLINE void std_uart_set_transfer_direction(UART_t *uartx, uint32_t direction)
{
    MODIFY_REG(uartx->CR1, UART_CR1_TE|UART_CR1_RE, direction);
}

/** 
* @brief  获取 UART 传输方向控制
* @param  uartx UART外设
* @retval uint32_t 传输方向
*             @arg UART_DIRECTION_NONE  
*             @arg UART_DIRECTION_SEND                                                                                        
*             @arg UART_DIRECTION_RECEIVE                                                      
*             @arg UART_DIRECTION_SEND_RECEIVE                                                                                                               
*/
__STATIC_INLINE uint32_t std_uart_get_transfer_direction(UART_t *uartx)
{
    return(uartx->CR1 & (UART_CR1_TE|UART_CR1_RE));
}

/** 
* @brief  使能UART引脚交换
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_pin_swap_enable(UART_t *uartx)
{
    uartx->CR2 |= (UART_CR2_SWAP);
} 

/** 
* @brief  禁止UART引脚交换
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_pin_swap_disable(UART_t *uartx)
{
    uartx->CR2 &= (~UART_CR2_SWAP);
} 

/** 
* @brief  使能UART RX引脚电平反向
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_rx_level_invert_enable(UART_t *uartx)
{
    uartx->CR2 |= (UART_CR2_RXIVC);
} 

/** 
* @brief  禁止UART RX引脚电平反向
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_rx_level_invert_disable(UART_t *uartx)
{
    uartx->CR2 &= (~UART_CR2_RXIVC);
} 

/** 
* @brief  使能UART TX引脚电平反向
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_tx_level_invert_enable(UART_t *uartx)
{
    uartx->CR2 |= (UART_CR2_TXIVC);
} 

/** 
* @brief  禁止UART TX引脚电平反向
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_tx_level_invert_disable(UART_t *uartx)
{
    uartx->CR2 &= (~UART_CR2_TXIVC);
} 

/** 
* @brief  使能UART数据极性反向
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_data_invert_enable(UART_t *uartx)
{
    uartx->CR2 |= (UART_CR2_DATAIVC);
} 

/** 
* @brief  禁止UART数据极性反向
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_data_invert_disable(UART_t *uartx)
{
    uartx->CR2 &= (~UART_CR2_DATAIVC);
}

/** 
* @brief  使能UART单线半双工模式
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_half_duplex_enable(UART_t *uartx)
{
    uartx->CR3 |= (UART_CR3_HDEN);
} 

/** 
* @brief  禁止UART单线半双工模式
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_half_duplex_disable(UART_t *uartx)
{
    uartx->CR3 &= (~UART_CR3_HDEN);
}

/** 
* @brief  禁止UART上溢检测
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_overrun_disable(UART_t *uartx)
{
    uartx->CR3 |= (UART_CR3_ORED);
}

/** 
* @brief  使能UART上溢检测
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_overrun_enable(UART_t *uartx)
{
    uartx->CR3 &= (~UART_CR3_ORED);
}

/** 
* @brief  使能UART CR1寄存器中控制的中断
* @param  uartx UART外设
* @param  interrupt UART中断源选择     
*             @arg UART_CR1_INTERRUPT_RXNE                             
*             @arg UART_CR1_INTERRUPT_TC                    
*             @arg UART_CR1_INTERRUPT_TXE                    
*             @arg UART_CR1_INTERRUPT_PE                                     
* @retval 无
*/
__STATIC_INLINE void std_uart_cr1_interrupt_enable(UART_t *uartx, uint32_t interrupt)
{
    uartx->CR1 |= (interrupt);
}

/** 
* @brief  禁止UART CR1寄存器中控制的中断
* @param  uartx UART外设
* @param  interrupt UART中断源选择
*             @arg UART_CR1_INTERRUPT_RXNE                                 
*             @arg UART_CR1_INTERRUPT_TC                    
*             @arg UART_CR1_INTERRUPT_TXE                    
*             @arg UART_CR1_INTERRUPT_PE                                     
* @retval 无
*/
__STATIC_INLINE void std_uart_cr1_interrupt_disable(UART_t *uartx, uint32_t interrupt)
{
    uartx->CR1 &= (~interrupt);
}

/** 
* @brief  获取UART CR1寄存器中控制的中断使能
* @param  uartx UART外设
* @param  interrupt UART中断源选择
*             @arg UART_CR1_INTERRUPT_RXNE                                
*             @arg UART_CR1_INTERRUPT_TC                    
*             @arg UART_CR1_INTERRUPT_TXE                    
*             @arg UART_CR1_INTERRUPT_PE                                     
* @retval uint32_t 返回选择的UART中断源使能状态
*             @arg 非0:  表示选择中断源使能
*             @arg 0:    表示选择中断源未使能
*/
__STATIC_INLINE uint32_t std_uart_get_cr1_interrupt_enable(UART_t *uartx, uint32_t interrupt)
{
    return (uartx->CR1 & interrupt);
}

/** 
* @brief  使能UART CR3寄存器中控制的 ERR 中断
* @param  uartx UART外设                                            
* @retval 无
*/
__STATIC_INLINE void std_uart_cr3_interrupt_err_enable(UART_t *uartx)
{
    uartx->CR3 |= (UART_CR3_EIE);
}

/** 
* @brief  禁止UART CR3寄存器中控制的 ERR 中断
* @param  uartx UART外设                                         
* @retval 无
*/
__STATIC_INLINE void std_uart_cr3_interrupt_err_disable(UART_t *uartx)
{
    uartx->CR3 &= (~UART_CR3_EIE);
}

/** 
* @brief  获取UART CR3寄存器中中断ERR 中断
* @param  uartx UART外设
* @retval uint32_t 返回选择的UART中断源使能状态
*             @arg 非0：  表示选择中断源EIE 使能
*             @arg 0：    表示选择中断源EIE 未使能
*/
__STATIC_INLINE uint32_t std_uart_get_cr3_interrupt_err_enable(UART_t *uartx)
{
     return (uartx->CR3 & UART_CR3_EIE);
}

/** 
* @brief  获取UART标志
* @param  uartx UART外设
* @param  flag UART标志
*             @arg UART_FLAG_RECEIVE_BUSY     
*             @arg UART_FLAG_TXE              
*             @arg UART_FLAG_TC               
*             @arg UART_FLAG_RXNE             
*             @arg UART_FLAG_ORE
*             @arg UART_FLAG_NOISE              
*             @arg UART_FLAG_FE               
*             @arg UART_FLAG_PE               
* @retval uint32_t UART标志置起状态
*             @arg 非0：  状态置起
*             @arg 0：    状态未置起
*/
__STATIC_INLINE uint32_t std_uart_get_flag(UART_t *uartx,uint32_t flag)
{
    return (uartx->ISR & flag);
}

/** 
* @brief  清除UART标志 
* @param  uartx UART外设
* @param  clear_flag UART标志信息
*             @arg UART_CLEAR_PE     
*             @arg UART_CLEAR_FE              
*             @arg UART_CLEAR_NOISE               
*             @arg UART_CLEAR_ORE             
*             @arg UART_CLEAR_TC                           
* @retval 无      
*/
__STATIC_INLINE void std_uart_clear_flag(UART_t *uartx, uint32_t clear_flag)
{
    uartx->ICR = (clear_flag);
}

/** 
* @brief  使能UART接收器
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_rx_enable(UART_t *uartx)
{
    uartx->CR1 |= (UART_CR1_RE);
} 

/** 
* @brief  禁止UART接收器
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_rx_disable(UART_t *uartx)
{
    uartx->CR1 &= (~UART_CR1_RE);
} 

/** 
* @brief  使能UART发送器
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_tx_enable(UART_t *uartx)
{
    uartx->CR1 |= (UART_CR1_TE);
} 

/** 
* @brief  禁止UART发送器
* @param  uartx UART外设
* @retval 无
*/
__STATIC_INLINE void std_uart_tx_disable(UART_t *uartx)
{
    uartx->CR1 &= (~UART_CR1_TE);
} 

/** 
* @brief  获取UART接收数据
* @param  uartx UART外设
* @retval uint32_t UART接收到的数据
*/
__STATIC_INLINE uint32_t std_uart_rx_read_data(UART_t *uartx)
{
    return (uartx->RDR);
}

/** 
* @brief  写入UART发送数据
* @param  uartx UART外设
* @param  data_value 发送数据
* @retval 无
*/
__STATIC_INLINE void std_uart_tx_write_data(UART_t *uartx, uint32_t data_value)
{
    uartx->TDR = data_value;
}

/** 
* @brief  写入UART 波特率分频寄存器
* @param  uartx UART外设
* @param  brr_value 波特率分频值范围： 0x10 ~ 0xFFFF
* @retval 无
*/
__STATIC_INLINE void std_uart_set_brr_value(UART_t *uartx, uint32_t brr_value)
{
    uartx->BRR = brr_value;
}

/** 
* @brief  获取UART波特率分频配置
* @param  uartx UART外设
* @retval uint32_t UART波特率分频的配置值
*/
__STATIC_INLINE uint32_t std_uart_get_brr_value(UART_t *uartx)
{
    return (uartx->BRR);
}

void std_uart_init(UART_t *uartx,std_uart_init_t *UART_init_param);
void std_uart_deinit(UART_t *uartx);
void std_uart_struct_init(std_uart_init_t *UART_init_struct);

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

#endif /* CIU32F003_STD_UART_H */

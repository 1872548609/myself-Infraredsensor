/************************************************************************************************/
/**
* @file               ciu32f003_std_rcc.h
* @author             MCU Ecosystem Development Team
* @brief              RCC STD库驱动头文件。
*                     提供RCC相关的STD库操作函数声明、数据类型以及常量的定义。                         
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*避免头文件重复引用*/
#ifndef CIU32F003_STD_RCC_H
#define CIU32F003_STD_RCC_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup RCC RCC
* @brief 复位和时钟控制器的STD库驱动
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
* @defgroup RCC_Constants RCC Constants 
* @brief  RCC常量定义及宏定义
* @{
*
*/
/************************************************************************************************/
/* 系统时钟源选择 */
#define RCC_SYSCLK_SRC_RCHDIV6         RCC_CFG_SYSW_RCHDIV6                         /**< 选择RCHDIV6作为系统时钟源   */
#define RCC_SYSCLK_SRC_RCHDIV3         RCC_CFG_SYSW_RCHDIV3                         /**< 选择RCHDIV3作为系统时钟源   */
#define RCC_SYSCLK_SRC_RCH             RCC_CFG_SYSW_RCH                             /**< 选择RCH作为系统时钟源       */
#define RCC_SYSCLK_SRC_RCL             RCC_CFG_SYSW_RCL                             /**< 选择RCL作为系统时钟源       */
#define RCC_SYSCLK_SRC_EXTCLK          RCC_CFG_SYSW_EXTCLK                          /**< 选择EXTCLK作为系统时钟源    */

/* 系统时钟源状态定义 */
#define RCC_SYSCLK_SRC_STATUS_RCHDIV6      RCC_CFG_SYSWS_RCHDIV6                    /**< 系统时钟为RCHDIV6  */
#define RCC_SYSCLK_SRC_STATUS_RCHDIV3      RCC_CFG_SYSWS_RCHDIV3                    /**< 系统时钟为RCHDIV3  */
#define RCC_SYSCLK_SRC_STATUS_RCH          RCC_CFG_SYSWS_RCH                        /**< 系统时钟为RCH      */
#define RCC_SYSCLK_SRC_STATUS_RCL          RCC_CFG_SYSWS_RCL                        /**< 系统时钟为RCL      */
#define RCC_SYSCLK_SRC_STATUS_EXTCLK       RCC_CFG_SYSWS_EXTCLK                     /**< 系统时钟为EXTCLK   */

/* AHB时钟分频参数定义 */
#define RCC_HCLK_DIV1                      RCC_CFG_HPRE_1                           /**< HCLK不分频         */
#define RCC_HCLK_DIV2                      RCC_CFG_HPRE_2                           /**< HCLK分频因子 = 2   */
#define RCC_HCLK_DIV4                      RCC_CFG_HPRE_4                           /**< HCLK分频因子 = 4   */
#define RCC_HCLK_DIV8                      RCC_CFG_HPRE_8                           /**< HCLK分频因子 = 8   */
#define RCC_HCLK_DIV16                     RCC_CFG_HPRE_16                          /**< HCLK分频因子 = 16  */
#define RCC_HCLK_DIV32                     RCC_CFG_HPRE_32                          /**< HCLK分频因子 = 32  */
#define RCC_HCLK_DIV64                     RCC_CFG_HPRE_64                          /**< HCLK分频因子 = 64  */
#define RCC_HCLK_DIV128                    RCC_CFG_HPRE_128                         /**< HCLK分频因子 = 128 */

/* APB时钟分频参数定义 */
#define RCC_PCLK_DIV1                      RCC_CFG_PPRE_1                           /**< APB总线时钟不分频        */
#define RCC_PCLK_DIV2                      RCC_CFG_PPRE_2                           /**< APB总线时钟分频因子 = 2  */
#define RCC_PCLK_DIV4                      RCC_CFG_PPRE_4                           /**< APB总线时钟分频因子 = 4  */
#define RCC_PCLK_DIV8                      RCC_CFG_PPRE_8                           /**< APB总线时钟分频因子 = 8  */
#define RCC_PCLK_DIV16                     RCC_CFG_PPRE_16                          /**< APB总线时钟分频因子 = 16 */
        
/* MCO时钟源参数定义 */                                                                            
#define RCC_MCO_SRC_DISABLE                RCC_CFG_MCOSEL_DISABLE                   /**< MCO输出无效         */
#define RCC_MCO_SRC_SYSCLK                 RCC_CFG_MCOSEL_SYSCLK                    /**< MCO选择SYSCLK输出   */
#define RCC_MCO_SRC_RCHDIV6                RCC_CFG_MCOSEL_RCHDIV6                   /**< MCO选择RCHDIV6输出  */
#define RCC_MCO_SRC_EXTCLK                 RCC_CFG_MCOSEL_EXTCLK                    /**< MCO选择EXTCLK输出   */
#define RCC_MCO_SRC_RCL                    RCC_CFG_MCOSEL_RCL                       /**< MCO选择RCL输出      */

/* MCO时钟分频参数定义 */
#define RCC_MCO_DIV1                       RCC_CFG_MCOPRE_DIV1                      /**< MCO不分频         */
#define RCC_MCO_DIV2                       RCC_CFG_MCOPRE_DIV2                      /**< MCO分频因子 = 2   */
#define RCC_MCO_DIV4                       RCC_CFG_MCOPRE_DIV4                      /**< MCO分频因子 = 4   */
#define RCC_MCO_DIV8                       RCC_CFG_MCOPRE_DIV8                      /**< MCO分频因子 = 8   */
#define RCC_MCO_DIV16                      RCC_CFG_MCOPRE_DIV16                     /**< MCO分频因子 = 16  */
#define RCC_MCO_DIV32                      RCC_CFG_MCOPRE_DIV32                     /**< MCO分频因子 = 32  */
#define RCC_MCO_DIV64                      RCC_CFG_MCOPRE_DIV64                     /**< MCO分频因子 = 64  */
#define RCC_MCO_DIV128                     RCC_CFG_MCOPRE_DIV128                    /**< MCO分频因子 = 128 */

/* RCC中断控制位定义 */
#define RCC_INTERRUPT_RCL_READY            RCC_IER_RCL_RDYIE                        /**< RCL Ready的中断控制位   */
#define RCC_INTERRUPT_RCH_READY            RCC_IER_RCH_RDYIE                        /**< RCH Ready的中断控制位   */

/* RCC中断标志位定义 */
#define RCC_FLAG_RCL_READY                 RCC_ISR_RCL_RDYF                         /**< RCL Ready的中断标志位   */
#define RCC_FLAG_RCH_READY                 RCC_ISR_RCH_RDYF                         /**< RCH Ready的中断标志位   */

/* RCC清除控制位定义 */
#define RCC_CLEAR_RCL_READY                RCC_ICR_RCL_RDYC                         /**< RCL Ready的清除控制位   */
#define RCC_CLEAR_RCH_READY                RCC_ICR_RCH_RDYC                         /**< RCH Ready的清除控制位   */

/* IO端口时钟选择 */
#define RCC_PERIPH_CLK_GPIOA              RCC_IOPEN_GPIOAEN                        /**< GPIOA 时钟控制位  */
#define RCC_PERIPH_CLK_GPIOB              RCC_IOPEN_GPIOBEN                        /**< GPIOB 时钟控制位  */
#define RCC_PERIPH_CLK_GPIOC              RCC_IOPEN_GPIOCEN                        /**< GPIOC 时钟控制位  */

/* IO端口复位选择 */
#define RCC_PERIPH_RESET_GPIOA            RCC_IOPRST_GPIOA_RST                            /**< GPIOA端口复位控制位  */
#define RCC_PERIPH_RESET_GPIOB            RCC_IOPRST_GPIOB_RST                            /**< GPIOB端口复位控制位  */
#define RCC_PERIPH_RESET_GPIOC            RCC_IOPRST_GPIOC_RST                            /**< GPIOC端口复位控制位  */

/* AHB外设时钟选择 */
#define RCC_PERIPH_CLK_CRC                RCC_AHBEN_CRCEN                                 /**< AHB外设CRC时钟控制位  */

/* AHB外设复位选择 */
#define RCC_PERIPH_RESET_CRC              RCC_AHBRST_CRC_RST                              /**< AHB外设CRC复位控制位  */

/* APB外设时钟选择 */
#define RCC_PERIPH_CLK_TIM3               RCC_APBEN1_TIM3EN                               /**< APB外设TIM3时钟控制位    */
#define RCC_PERIPH_CLK_UART2              RCC_APBEN1_UART2EN                              /**< APB外设UART2时钟控制位   */
#define RCC_PERIPH_CLK_I2C1               RCC_APBEN1_I2C1EN                               /**< APB外设I2C1时钟控制位    */
#define RCC_PERIPH_CLK_PMU                RCC_APBEN1_PMUEN                                /**< APB外设PMU时钟控制位     */
#define RCC_PERIPH_CLK_LPTIM1             RCC_APBEN1_LPTIM1EN                             /**< APB外设LPTIM1时钟控制位  */

#define RCC_PERIPH_CLK_COMP               RCC_APBEN2_COMPEN                               /**< APB外设COMP时钟控制位    */
#define RCC_PERIPH_CLK_TIM1               RCC_APBEN2_TIM1EN                               /**< APB外设TIM1时钟控制位    */
#define RCC_PERIPH_CLK_SPI1               RCC_APBEN2_SPI1EN                               /**< APB外设SPI1时钟控制位    */
#define RCC_PERIPH_CLK_UART1              RCC_APBEN2_UART1EN                              /**< APB外设UART1时钟控制位   */
#define RCC_PERIPH_CLK_ADC                RCC_APBEN2_ADCEN                                /**< APB外设ADC时钟控制位     */
#define RCC_PERIPH_CLK_DBG                RCC_APBEN2_DBGEN                                /**< APB外设DBG时钟控制位     */

/* APB外设复位选择 */
#define RCC_PERIPH_RESET_TIM3             RCC_APBRST1_TIM3_RST                            /**< APB外设TIM3复位控制位    */
#define RCC_PERIPH_RESET_UART2            RCC_APBRST1_UART2_RST                           /**< APB外设UART2复位控制位   */
#define RCC_PERIPH_RESET_I2C1             RCC_APBRST1_I2C1_RST                            /**< APB外设I2C1复位控制位    */
#define RCC_PERIPH_RESET_LPTIM1           RCC_APBRST1_LPTIM1_RST                          /**< APB外设LPTIM1复位控制位  */
                                                                                                                                                                  
#define RCC_PERIPH_RESET_COMP             RCC_APBRST2_COMP_RST                            /**< APB外设COMP复位制位      */
#define RCC_PERIPH_RESET_TIM1             RCC_APBRST2_TIM1_RST                            /**< APB外设TIM1复位控制位    */
#define RCC_PERIPH_RESET_SPI1             RCC_APBRST2_SPI1_RST                            /**< APB外设SPI1复位控制位    */
#define RCC_PERIPH_RESET_UART1            RCC_APBRST2_UART1_RST                           /**< APB外设UART复位控制位    */
#define RCC_PERIPH_RESET_ADC              RCC_APBRST2_ADC_RST                             /**< APB外设ADC复位控制位     */
#define RCC_PERIPH_RESET_DBG              RCC_APBRST2_DBG_RST                             /**< APB外设DBG复位控制位     */

/* LPTIM1异步时钟源选择 */
#define RCC_LPTIM1_ASYNC_CLK_SRC_PCLK         RCC_CLKSEL_LPTIM1_SEL_PCLK             /**< PCLK时钟作为LPTIM1时钟      */
#define RCC_LPTIM1_ASYNC_CLK_SRC_RCL          RCC_CLKSEL_LPTIM1_SEL_RCL              /**< RCL时钟作为LPTIM1时钟       */
#define RCC_LPTIM1_ASYNC_CLK_SRC_MCO          RCC_CLKSEL_LPTIM1_SEL_MCO              /**< MCO时钟作为LPTIM1时钟       */

/* COMP1异步时钟源选择 */
#define RCC_COMP1_ASYNC_CLK_SRC_PCLK          RCC_CLKSEL_COMP1_SEL_PCLK             /**< PCLK时钟作为COMP1时钟      */
#define RCC_COMP1_ASYNC_CLK_SRC_RCL           RCC_CLKSEL_COMP1_SEL_RCL              /**< RCL时钟作为COMP1时钟       */

/* COMP2异步时钟源选择 */
#define RCC_COMP2_ASYNC_CLK_SRC_PCLK          RCC_CLKSEL_COMP2_SEL_PCLK             /**< PCLK时钟作为COMP2时钟      */
#define RCC_COMP2_ASYNC_CLK_SRC_RCL           RCC_CLKSEL_COMP2_SEL_RCL              /**< RCL时钟作为COMP2时钟       */

/* 复位标志定义 */
#define RCC_RESET_FLAG_LOCKUP                   RCC_CSR2_LOCKUP_RSTF                  /**< LOCKUP复位标志           */
#define RCC_RESET_FLAG_NRST                     RCC_CSR2_NRST_RSTF                    /**< NRST引脚复位标志         */
#define RCC_RESET_FLAG_PMU                      RCC_CSR2_PMU_RSTF                     /**< POR/PDR、BOR复位标志     */
#define RCC_RESET_FLAG_SW                       RCC_CSR2_SW_RSTF                      /**< 软件复位标志             */
#define RCC_RESET_FLAG_IWDG                     RCC_CSR2_IWDG_RSTF                    /**< IWDG复位标志             */
#define RCC_RESET_FLAG_LPM                      RCC_CSR2_LPM_RSTF                     /**< 低功耗复位标志           */
#define RCC_RESET_FLAG_ALL                     (0xFFUL<<RCC_CSR2_LOCKUP_RSTF_POS)     /**< 所有复位标志             */


/** 
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup RCC_External_Functions RCC External Functions
* @brief    RCC对外函数
* @{
*
*/
/************************************************************************************************/
/** 
* @brief  使能RCH时钟
* @note   使能RCH后，软件应等待RCHRDY标志置位后，再使用该时钟
* @retval 无
*/
__STATIC_INLINE void std_rcc_rch_enable(void)
{
    RCC->CSR1 |= RCC_CSR1_RCHON;
} 

/** 
* @brief  关闭RCH时钟
* @note   如果RCH为系统时钟源，则其不能被停止。此时用户应先将系统时钟源切换为其他时钟，再关闭该时钟源
* @note   当关闭RCH时，需等待RCHRDY标志清零
* @retval 无
*/
__STATIC_INLINE void std_rcc_rch_disable(void)
{
    RCC->CSR1 &= (~RCC_CSR1_RCHON);
}

/** 
* @brief  获取RCH ready标志
* @retval uint32_t 返回RCH RDY标志状态
*             @arg 非0： 表示RCH ready已置位
*             @arg 0：表示RCH ready未置位
*/
__STATIC_INLINE uint32_t std_rcc_get_rch_ready(void) 
{
    return(RCC->CSR1 & RCC_CSR1_RCHRDY);
}        

/** 
* @brief  设置系统时钟源
* @param  clocksource 系统时钟源
*             @arg RCC_SYSCLK_SRC_RCHDIV6
*             @arg RCC_SYSCLK_SRC_RCHDIV3   
*             @arg RCC_SYSCLK_SRC_RCH    
*             @arg RCC_SYSCLK_SRC_RCL     
*             @arg RCC_SYSCLK_SRC_EXTCLK
* @retval 无
*/
__STATIC_INLINE void std_rcc_set_sysclk_source(uint32_t clocksource)
{
    MODIFY_REG(RCC->CFG, RCC_CFG_SYSW, clocksource);
}    

/** 
* @brief  获取系统时钟源信息
* @retval uint32_t 系统时钟源
*             @arg RCC_SYSCLK_SRC_STATUS_RCHDIV6  
*             @arg RCC_SYSCLK_SRC_STATUS_RCHDIV3   
*             @arg RCC_SYSCLK_SRC_STATUS_RCH    
*             @arg RCC_SYSCLK_SRC_STATUS_RCL    
*             @arg RCC_SYSCLK_SRC_STATUS_EXTCLK 
*/
__STATIC_INLINE uint32_t std_rcc_get_sysclk_source(void)
{
    return(RCC->CFG & RCC_CFG_SYSWS);
}    

/** 
* @brief  设置AHB时钟分频因子
* @param  ahb_div AHB分频因子
*             @arg RCC_HCLK_DIV1   
*             @arg RCC_HCLK_DIV2     
*             @arg ...   
*             @arg RCC_HCLK_DIV128
* @retval 无
*/
__STATIC_INLINE void std_rcc_set_ahbdiv(uint32_t ahb_div)
{
    MODIFY_REG(RCC->CFG, RCC_CFG_HPRE, ahb_div);
}    

/** 
* @brief  获取AHB时钟分频因子
* @retval uint32_t HCLK时钟分频因子
*             @arg RCC_HCLK_DIV1   
*             @arg RCC_HCLK_DIV2     
*             @arg ...   
*             @arg RCC_HCLK_DIV128
*/
__STATIC_INLINE uint32_t std_rcc_get_ahbdiv(void)
{
    return(RCC->CFG & RCC_CFG_HPRE);
} 

/** 
* @brief  设置APB时钟分频因子
* @param  apb_div APB分频因子
*             @arg RCC_PCLK_DIV1   
*             @arg RCC_PCLK_DIV2     
*             @arg ...   
*             @arg RCC_PCLK_DIV16
* @retval 无
*/
__STATIC_INLINE void std_rcc_set_apbdiv(uint32_t apb_div)
{
    MODIFY_REG(RCC->CFG, RCC_CFG_PPRE, apb_div);
}    

/** 
* @brief  获取APB时钟分频因子
* @retval uint32_t PCLK1时钟分频因子
*             @arg RCC_PCLK_DIV1   
*             @arg RCC_PCLK_DIV2     
*             @arg ...   
*             @arg RCC_PCLK_DIV16
*/
__STATIC_INLINE uint32_t std_rcc_get_apbdiv(void)
{
    return(RCC->CFG & RCC_CFG_PPRE);
} 

/**
* @brief  配置MCO输出信息
* @param  mco_source MCO输出源选择
*             @arg RCC_MCO_SRC_SYSCLK
*             @arg RCC_MCO_SRC_RCHDIV6
*             @arg RCC_MCO_SRC_EXTCLK
*             @arg RCC_MCO_SRC_RCL
* @param  mco_psc MCO输出分频因子
*             @arg RCC_MCO_DIV1
*             @arg RCC_MCO_DIV2
*             @arg ...
*             @arg RCC_MCO_DIV128
* @retval 无
*/
__STATIC_INLINE void std_rcc_mco_config(uint32_t mco_source, uint32_t mco_psc)
{
    MODIFY_REG(RCC->CFG, RCC_CFG_MCOSEL | RCC_CFG_MCOPRE, mco_source | mco_psc);
}

/** 
* @brief  使能EXTCLK时钟
* @retval 无
*/
__STATIC_INLINE void std_rcc_extclk_enable(void) 
{
    RCC->CSR1 |= RCC_CSR1_EXTCLKON;     
}

/** 
* @brief  关闭EXTCLK时钟
* @retval 无
*/
__STATIC_INLINE void std_rcc_extclk_disable(void)
{
    RCC->CSR1 &= (~RCC_CSR1_EXTCLKON);
} 

/** 
* @brief  使能RCC中断
* @param  interrupt 使能RCC中断源的信息
*             @arg  RCC_INTERRUPT_RCL_READY
*             @arg  RCC_INTERRUPT_RCH_READY
* @retval 无
*/
__STATIC_INLINE void std_rcc_interrupt_enable(uint32_t interrupt)     
{
    RCC->IER |= (interrupt);
}

/** 
* @brief  关闭RCC中断
* @param  interrupt 关闭RCC中断源的信息
*             @arg  RCC_INTERRUPT_RCL_READY
*             @arg  RCC_INTERRUPT_RCH_READY
* @retval 无
*/
__STATIC_INLINE void std_rcc_interrupt_disable(uint32_t interrupt)    
{
    RCC->IER &= (~(interrupt));
} 

/**
* @brief  获取RCC中断使能状态
* @param  interrupt RCC中断源信息
*             @arg  RCC_INTERRUPT_RCL_READY
*             @arg  RCC_INTERRUPT_RCH_READY
* @retval uint32_t 返回输入中断标志置位状态
*             @arg 非0： 表示指定的中断处于使能状态
*             @arg 0：表示指定的中断处于禁止状态
*/
__STATIC_INLINE uint32_t std_rcc_get_interrupt_enable(uint32_t interrupt)    
{
    return(RCC->IER & (interrupt));
}

/**
* @brief  获取RCC中断标志状态
* @param  flag RCC中断标志信息
*             @arg  RCC_FLAG_RCL_READY
*             @arg  RCC_FLAG_RCH_READY
* @retval uint32_t 返回输入中断标志置位状态
*             @arg 非0： 表示指定的中断已置位
*             @arg 0：表示指定的中断未置位
*/
__STATIC_INLINE uint32_t std_rcc_get_flag(uint32_t flag) 
{
    return(RCC->ISR & (flag));
}

/** 
* @brief  清除RCC标志
* @param  flags 清除标志位
*             @arg  RCC_CLEAR_RCL_READY
*             @arg  RCC_CLEAR_RCH_READY
* @retval 无
*/
__STATIC_INLINE void std_rcc_clear_flag(uint32_t flags)   
{
    RCC->ICR = (flags);
}

/** 
* @brief  使能RCL时钟
* @note   使能RCL后，软件应等待RCLRDY标志置位后，再使用该时钟
* @retval 无
*/
__STATIC_INLINE void std_rcc_rcl_enable(void)
{
    RCC->CSR2 |= RCC_CSR2_RCLON;
}          

/** 
* @brief  关闭RCL时钟
* @note   如果RCL为系统时钟源，则其不能被停止。此时用户应先将系统时钟源切换为其他时钟，再关闭该时钟源
* @note   当关闭RCL时，RCLRDY标志会在RCL关闭后自动清零
* @retval 无
*/
__STATIC_INLINE void std_rcc_rcl_disable(void) 
{
    RCC->CSR2 &= (~RCC_CSR2_RCLON);
}        

/** 
* @brief  获取RCL ready标志
* @retval uint32_t 返回RCL RDY标志状态
*             @arg 非0： 表示RCL ready已置位
*             @arg 0：表示RCL ready未置位
*/
__STATIC_INLINE uint32_t std_rcc_get_rcl_ready(void)          
{
    return(RCC->CSR2 & RCC_CSR2_RCLRDY);
}

/** 
* @brief  获取复位标志
* @param  reset_flag 指定要获取的复位标志
*             @arg RCC_RESET_FLAG_LOCKUP
*             @arg RCC_RESET_FLAG_NRST  
*             @arg RCC_RESET_FLAG_PMU   
*             @arg RCC_RESET_FLAG_SW    
*             @arg RCC_RESET_FLAG_IWDG  
*             @arg RCC_RESET_FLAG_LPM   
*             @arg RCC_RESET_FLAG_ALL   
* @retval uint32_t 返回状态标志
*             @arg 非0： 表示指定的复位标志已置位
*             @arg 0：表示指定的复位标志未置位
*/
__STATIC_INLINE uint32_t std_rcc_get_reset_flag(uint32_t reset_flag)            
{
    return(RCC->CSR2 & (reset_flag));
} 

/** 
* @brief  清除复位标志
* @note   该函数将清除所有复位标志
* @retval 无
*/
__STATIC_INLINE void std_rcc_clear_reset_flags(void)           
{
    RCC->CSR2 |= RCC_CSR2_RMVF;
}

/** 
* @brief  GPIO端口时钟使能
* @param  gpiox_clock 指定使能的GPIO时钟
*             @arg RCC_PERIPH_CLK_GPIOA
*             @arg RCC_PERIPH_CLK_GPIOB
*             @arg RCC_PERIPH_CLK_GPIOC
* @retval 无
*/
__STATIC_INLINE void std_rcc_gpio_clk_enable(uint32_t gpiox_clock)
{
    RCC->IOPEN |= gpiox_clock; 
    
    /* RCC外围时钟启用后的延迟 */  
    __NOP(); __NOP(); __NOP(); 
}

/** 
* @brief  GPIO端口时钟禁止
* @param  gpiox_clock 指定禁止的GPIO时钟
*             @arg RCC_PERIPH_CLK_GPIOA
*             @arg RCC_PERIPH_CLK_GPIOB
*             @arg RCC_PERIPH_CLK_GPIOC
* @retval 无
*/
__STATIC_INLINE void std_rcc_gpio_clk_disable(uint32_t gpiox_clock) 
{
    RCC->IOPEN &= (~(gpiox_clock)); 
    
    /* RCC外围时钟启用后的延迟 */  
    __NOP(); __NOP(); __NOP(); 
}

/** 
* @brief  GPIO端口复位
* @param  gpiox_rst 指定复位的GPIO端口
*             @arg RCC_PERIPH_RESET_GPIOA
*             @arg RCC_PERIPH_RESET_GPIOB
*             @arg RCC_PERIPH_RESET_GPIOC
* @retval 无
*/
__STATIC_INLINE void std_rcc_gpio_reset(uint32_t gpiox_rst)
{
    RCC->IOPRST |= (gpiox_rst);
    RCC->IOPRST &= (~(gpiox_rst));
}         

/** 
* @brief  AHB外设时钟使能
* @param  periph_clock 指定使能的AHB外设时钟
*             @arg RCC_PERIPH_CLK_CRC  
* @retval 无
*/
__STATIC_INLINE void std_rcc_ahb_clk_enable(uint32_t periph_clock)
{
    RCC->AHBEN |= periph_clock;
    
    /* RCC外围时钟启用后的延迟 */    
    __NOP(); __NOP(); __NOP(); 
}   

/** 
* @brief  AHB外设时钟禁止
* @param  periph_clock 指定禁止的AHB外设时钟
*             @arg RCC_PERIPH_CLK_CRC  
* @retval 无
*/
__STATIC_INLINE void std_rcc_ahb_clk_disable(uint32_t periph_clock)            
{
    RCC->AHBEN &= (~(periph_clock));
}

/** 
* @brief  AHB外设复位
* @param  periph_rst 指定复位的AHB外设
*             @arg RCC_PERIPH_RESET_CRC
* @retval 无
*/
__STATIC_INLINE void std_rcc_ahb_reset(uint32_t periph_rst)
{
    RCC->AHBRST |= (periph_rst);
    RCC->AHBRST &= (~(periph_rst));
}       

/** 
* @brief  APB外设1时钟使能
* @param  periph_clock 指定使能的APB外设1时钟
*             @arg RCC_PERIPH_CLK_TIM3   
*             @arg RCC_PERIPH_CLK_UART2 
*             @arg RCC_PERIPH_CLK_I2C1   
*             @arg RCC_PERIPH_CLK_PMU       
*             @arg RCC_PERIPH_CLK_LPTIM1 
* @retval 无
*/
__STATIC_INLINE void std_rcc_apb1_clk_enable(uint32_t periph_clock)
{
    RCC->APBEN1 |= periph_clock;
    
    /* RCC外围时钟启用后的延迟 */    
    __NOP(); __NOP(); __NOP(); 
}

/** 
* @brief  APB外设1时钟禁止
* @param  periph_clock 指定禁止的APB外设1时钟
*             @arg RCC_PERIPH_CLK_TIM3      
*             @arg RCC_PERIPH_CLK_UART2 
*             @arg RCC_PERIPH_CLK_I2C1   
*             @arg RCC_PERIPH_CLK_PMU      
*             @arg RCC_PERIPH_CLK_LPTIM1 
* @retval 无
*/
__STATIC_INLINE void std_rcc_apb1_clk_disable(uint32_t periph_clock)            
{
    RCC->APBEN1 &= (~(periph_clock));
}

/** 
* @brief  APB外设1复位
* @param  periph_rst 指定复位的APB外设1
*             @arg RCC_PERIPH_RESET_TIM3     
*             @arg RCC_PERIPH_RESET_UART2 
*             @arg RCC_PERIPH_RESET_I2C1   
*             @arg RCC_PERIPH_RESET_LPTIM1 
* @retval 无
*/
__STATIC_INLINE void std_rcc_apb1_reset(uint32_t periph_rst)
{
    RCC->APBRST1 |= (periph_rst);
    RCC->APBRST1 &= (~(periph_rst));
}       

/** 
* @brief  APB外设2时钟使能
* @param  periph_clock 指定使能的APB外设2时钟
*             @arg RCC_PERIPH_CLK_COMP
*             @arg RCC_PERIPH_CLK_TIM1   
*             @arg RCC_PERIPH_CLK_SPI1   
*             @arg RCC_PERIPH_CLK_UART1 
*             @arg RCC_PERIPH_CLK_ADC    
*             @arg RCC_PERIPH_CLK_DBG
* @retval 无
*/
__STATIC_INLINE void std_rcc_apb2_clk_enable(uint32_t periph_clock)
{
    RCC->APBEN2 |= periph_clock;
    
    /* RCC外围时钟启用后的延迟 */    
    __NOP(); __NOP(); __NOP(); 
}

/** 
* @brief  APB外设2时钟禁止
* @param  periph_clock 指定禁止的APB外设2时钟
*             @arg RCC_PERIPH_CLK_COMP
*             @arg RCC_PERIPH_CLK_TIM1   
*             @arg RCC_PERIPH_CLK_SPI1   
*             @arg RCC_PERIPH_CLK_UART1 
*             @arg RCC_PERIPH_CLK_ADC    
*             @arg RCC_PERIPH_CLK_DBG
* @retval 无
*/
__STATIC_INLINE void std_rcc_apb2_clk_disable(uint32_t periph_clock)            
{
    RCC->APBEN2 &= (~(periph_clock));
}

/** 
* @brief  APB外设2复位
* @param  periph_rst 指定复位的APB外设2
*             @arg RCC_PERIPH_RESET_COMP   
*             @arg RCC_PERIPH_RESET_TIM1   
*             @arg RCC_PERIPH_RESET_SPI1   
*             @arg RCC_PERIPH_RESET_UART1 
*             @arg RCC_PERIPH_RESET_ADC    
*             @arg RCC_PERIPH_RESET_DBG  
* @retval 无
*/
__STATIC_INLINE void std_rcc_apb2_reset(uint32_t periph_rst)
{
    RCC->APBRST2 |= (periph_rst);
    RCC->APBRST2 &= (~(periph_rst));
}       

/** 
* @brief  使能LOCKUP复位
* @retval 无
*/
__STATIC_INLINE void std_rcc_lockup_reset_enable(void)
{
    RCC->CSR2 |= RCC_CSR2_LOCKUP_RSTEN;
}

/** 
* @brief  禁止LOCKUP复位
* @retval 无
*/
__STATIC_INLINE void std_rcc_lockup_reset_disable(void)
{
    RCC->CSR2 &= (~RCC_CSR2_LOCKUP_RSTEN);
}

/** 
* @brief  LPTIM1时钟源选择
* @param  lptim1clk_select LPTIM1时钟源
*             @arg RCC_LPTIM1_ASYNC_CLK_SRC_PCLK
*             @arg RCC_LPTIM1_ASYNC_CLK_SRC_RCL
*             @arg RCC_LPTIM1_ASYNC_CLK_SRC_MCO
* @retval 无
*/
__STATIC_INLINE void std_rcc_set_lptim1clk_source(uint32_t lptim1clk_select)
{
    MODIFY_REG(RCC->CLKSEL, RCC_CLKSEL_LPTIM1_SEL, (lptim1clk_select));
}       

/** 
* @brief  获取LPTIM1时钟源
* @retval uint32_t 返回LPTIM1时钟源信息
*             @arg RCC_LPTIM1_ASYNC_CLK_SRC_PCLK1
*             @arg RCC_LPTIM1_ASYNC_CLK_SRC_RCL
*             @arg RCC_LPTIM1_ASYNC_CLK_SRC_MCO
*/
__STATIC_INLINE uint32_t std_rcc_get_lptim1clk_source(void)
{
    return(RCC->CLKSEL & RCC_CLKSEL_LPTIM1_SEL);
} 

/** 
* @brief  COMP1时钟源选择
* @param  comp1clk_select COMP1时钟源
*             @arg RCC_COMP1_ASYNC_CLK_SRC_PCLK
*             @arg RCC_COMP1_ASYNC_CLK_SRC_RCL
* @retval 无
*/
__STATIC_INLINE void std_rcc_set_comp1clk_source(uint32_t comp1clk_select)
{
    MODIFY_REG(RCC->CLKSEL, RCC_CLKSEL_COMP1_SEL, (comp1clk_select));
}       

/** 
* @brief  获取COMP1时钟源
* @retval uint32_t 返回COMP1时钟源信息
*             @arg RCC_COMP1_ASYNC_CLK_SRC_PCLK
*             @arg RCC_COMP1_ASYNC_CLK_SRC_RCL
*/
__STATIC_INLINE uint32_t std_rcc_get_comp1clk_source(void)
{
    return(RCC->CLKSEL & RCC_CLKSEL_COMP1_SEL);
} 

/** 
* @brief  COMP2时钟源选择
* @param  comp2clk_select COMP2时钟源
*             @arg RCC_COMP2_ASYNC_CLK_SRC_PCLK
*             @arg RCC_COMP2_ASYNC_CLK_SRC_RCL
* @retval 无
*/
__STATIC_INLINE void std_rcc_set_comp2clk_source(uint32_t comp2clk_select)
{
    MODIFY_REG(RCC->CLKSEL, RCC_CLKSEL_COMP2_SEL, (comp2clk_select));
}       

/** 
* @brief  获取COMP2时钟源
* @retval uint32_t 返回COMP2时钟源信息
*             @arg RCC_COMP2_ASYNC_CLK_SRC_PCLK
*             @arg RCC_COMP2_ASYNC_CLK_SRC_RCL
*/
__STATIC_INLINE uint32_t std_rcc_get_comp2clk_source(void)
{
    return(RCC->CLKSEL & RCC_CLKSEL_COMP2_SEL);
} 

/** 
* @brief  写RCL校准值
* @param  cal_value RCL校准值
* @retval 无
*/
__STATIC_INLINE void std_rcc_write_rcl_calibration(uint32_t cal_value)
{
    RCC->RCLCAL = cal_value;
}

/** 
* @brief  读取RCL校准值
* @retval uint32_t RCL校准值
*/
__STATIC_INLINE uint32_t std_rcc_read_rcl_calibration(void)
{
    return(RCC->RCLCAL);
}

/** 
* @brief  写RCH时钟粗调值
* @param  cal_value RCH时钟粗调值
* @retval 无
*/
__STATIC_INLINE void std_rcc_write_rch_coarse_calibration(uint32_t cal_value)
{
    MODIFY_REG(RCC->RCHCAL, RCC_RCHCAL_RCH_CAL_COARSE, cal_value << RCC_RCHCAL_RCH_CAL_COARSE_POS);
}

/** 
* @brief  读取RCH时钟粗调值
* @retval uint32_t RCH时钟粗调值
*/
__STATIC_INLINE uint32_t std_rcc_read_rch_coarse_calibration(void)
{
    return((RCC->RCHCAL & RCC_RCHCAL_RCH_CAL_COARSE) >> RCC_RCHCAL_RCH_CAL_COARSE_POS);
}

/** 
* @brief  写RCH时钟微调值
* @param  trim_value  RCH时钟微调值
* @retval 无
*/
__STATIC_INLINE void std_rcc_write_rch_fine_calibration(uint32_t trim_value)
{
    MODIFY_REG(RCC->RCHCAL, RCC_RCHCAL_RCH_CAL_FINE, trim_value);
}

/** 
* @brief  读取RCH时钟微调值
* @retval uint32_t RCH时钟微调值
*/
__STATIC_INLINE uint32_t std_rcc_read_rch_fine_calibration(void)
{
    return(RCC->RCHCAL & RCC_RCHCAL_RCH_CAL_FINE);
}


/* 获取时钟频率函数 */
uint32_t std_rcc_get_sysclkfreq(void);
uint32_t std_rcc_get_hclkfreq(void);
uint32_t std_rcc_get_pclkfreq(void); 


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

#endif /* CIU32F003_STD_RCC_H */

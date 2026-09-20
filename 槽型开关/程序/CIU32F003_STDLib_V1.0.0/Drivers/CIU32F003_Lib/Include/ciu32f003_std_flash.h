/************************************************************************************************/
/**
* @file               ciu32f003_std_flash.h
* @author             MCU Ecosystem Development Team
* @brief              FLASH STD库驱动头文件。
*                     提供FLASH相关的STD库操作函数声明、数据类型以及常量的定义。                         
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*避免头文件重复引用*/
#ifndef CIU32F003_STD_FLASH_H
#define CIU32F003_STD_FLASH_H

/************************************************************************************************/
/** 
* @addtogroup CIU32F003_STD_Driver
* @{
*/

/**
* @defgroup FLASH FLASH
* @brief FLASH存储器的STD库驱动
* @{
*/
/************************************************************************************************/



#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std_common.h"


/*-----------------------------------------type define------------------------------------------*/

/************************************************************************************************/
/** 
* @defgroup FLASH_Types FLASH Types
* @brief    FLASH数据类型定义
* @{
*/
/************************************************************************************************/

/**
* @brief  FLASH选项字节配置结构体定义
*/
typedef struct
{    
    uint32_t config_select;          /**< 选项字节配置选择，未被选中的选项字节将使用出厂默认值，
                                          选中的选项字节将使用选项字节配置结构体对应成员变量的值更新
                                              @arg FLASH_OB_CONFIG_RDP
                                              @arg FLASH_OB_CONFIG_BOR
                                              ...
                                              @arg FLASH_OB_CONFIG_ALL                                          */
    
    uint32_t rdp_level;              /**< FLASH读出保护等级选择                               
                                              @arg FLASH_RDP_LEVEL_0
                                              @arg FLASH_RDP_LEVEL_1                                            */
                                   
    uint32_t bor_enable;             /**< BOR使能控制
                                              @arg FLASH_BOR_ENABLE 
                                              @arg FLASH_BOR_DISABLE                                            */
    
    uint32_t bor_level;              /**< BOR档位选择
                                              @arg FLASH_BOR_LEVEL_0 
                                              @arg FLASH_BOR_LEVEL_1
                                              @arg FLASH_BOR_LEVEL_2
                                              @arg FLASH_BOR_LEVEL_3                                            */
    
    uint32_t nrst_swd_mode;          /**< NRST/SWDIO引脚功能选择
                                              @arg FLASH_PIN_MODE_NRST_SWD
                                              @arg FLASH_PIN_MODE_GPIO_SWD
                                              @arg FLASH_PIN_MODE_SWD_GPIO                                      */
                                   
    uint32_t reset_stop;             /**< 低功耗模式（Stop或Deepstop）复位控制
                                              @arg FLASH_STOP_RESET_ENABLE
                                              @arg FLASH_STOP_RESET_DISABLE                                     */
                                   
    uint32_t iwdg_stop;              /**< 独立看门狗低功耗模式（Stop或Deepstop）计数器计数控制
                                              @arg FLASH_IWDG_STOP_MODE_STOP
                                              @arg FLASH_IWDG_STOP_MODE_NORMAL                                  */
                                              
    uint32_t wrp_config;             /**< FLASH写入保护区域配置，其配置值范围在0x00~0x3F                        */
                                   
}std_flash_option_config_t;
     
/**
* @}
*/
     
/*-------------------------------------------define---------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup FLASH_Constants FLASH Constants 
* @brief  FLASH常量定义及宏定义
* @{
*
*/
/************************************************************************************************/

/* Flash读取访问等待周期 */      
#define FLASH_LATENCY_0CLK                FLASH_ACR_LATENCY_0CLK                     /**< 等待周期：0 HCLK         */
#define FLASH_LATENCY_1CLK                FLASH_ACR_LATENCY_1CLK                     /**< 等待周期：1 HCLK         */ 

/* Flash控制寄存器解锁密钥 */                                                        
#define FLASH_CR_KEY1                     (0xE57A1A85U)                              /**< Flash控制寄存器解锁密钥1 */
#define FLASH_CR_KEY2                     (0x7C6E8391U)                              /**< Flash控制寄存器解锁密钥2 */

/* Flash选项字节解锁密钥 */                                                          
#define FLASH_OPT_KEY1                    (0x6A894D7BU)                              /**< Flash选项字节解锁密钥1   */
#define FLASH_OPT_KEY2                    (0x7C311F5AU)                              /**< Flash选项字节解锁密钥2   */

/* Flash操作状态 */                                                                  
#define FLASH_FLAG_EOP                    FLASH_SR_EOP                               /**< Flash操作完成标志              */
#define FLASH_FLAG_BSY                    FLASH_SR_BSY                               /**< Flash操作状态标志              */
#define FLASH_FLAG_OPTVERR                FLASH_SR_OPTVERR                           /**< Flash option bytes校验错误标志 */
#define FLASH_FLAG_WRPERR                 FLASH_SR_WRPERR                            /**< Flash写保护错误标志            */

/* Flash中断源 */                      
#define FLASH_INTERRUPT_OPERR             FLASH_CR_OPERRIE                           /**< Flash操作异常中断        */
#define FLASH_INTERRUPT_EOP               FLASH_CR_EOPIE                             /**< Flash操作完成中断        */

/* Flash操作模式 */                                                                  
#define FLASH_MODE_IDLE                   FLASH_CR_OP_MODE_IDLE                      /**< Flash退出擦除编程模式    */
#define FLASH_MODE_PAGE_ERASE             FLASH_CR_OP_MODE_PAGE_ERASE                /**< Flash页擦除模式          */
#define FLASH_MODE_MASS_ERASE             FLASH_CR_OP_MODE_MASS_ERASE                /**< Flash批量擦除模式        */
#define FLASH_MODE_PROGRAM                FLASH_CR_OP_MODE_PROGRAM                   /**< Flash编程模式            */

/* 选项字节配置相关宏定义 */
/* 选项字节配置选择 */
#define FLASH_OB_CONFIG_RDP               (0x00000001U)                              /**< 选项字节选择更新RDP配置            */
#define FLASH_OB_CONFIG_BOR               (0x00000002U)                              /**< 选项字节选择更新BOR配置            */
#define FLASH_OB_CONFIG_NRST_SWD          (0x00000004U)                              /**< 选项字节选择更新NRST/SWDIO配置     */
#define FLASH_OB_CONFIG_RST_STOP          (0x00000008U)                              /**< 选项字节选择更新低功耗复位配置     */
#define FLASH_OB_CONFIG_IWDG_STOP         (0x00000010U)                              /**< 选项字节选择更新IWDG低功耗计数配置 */
#define FLASH_OB_CONFIG_WRP               (0x00000020U)                              /**< 选项字节选择更新WRP配置            */
#define FLASH_OB_CONFIG_ALL               (0x0000003FU)                              /**< 选项字节选择全部更新               */

/* 选项字节1配置参数 */
#define FLASH_PIN_MODE_MASK               FLASH_OPTR1_NRST_SWD_MODE_MASK             /**< NRST SWD引脚功能选择    */
#define FLASH_PIN_MODE_NRST_SWD           FLASH_OPTR1_NRST_SWD_MODE_0                /**< PC0: NRST  PB6: SWDIO   */
#define FLASH_PIN_MODE_GPIO_SWD           FLASH_OPTR1_NRST_SWD_MODE_2                /**< PC0: GPIO  PB6: SWDIO   */
#define FLASH_PIN_MODE_SWD_GPIO           FLASH_OPTR1_NRST_SWD_MODE_3                /**< PC0: SWDIO PB6: GPIO    */

#define FLASH_BOR_DISABLE                 (0x00000000U)                              /**< BOR禁止                             */
#define FLASH_BOR_ENABLE                  FLASH_OPTR1_BOR_EN                         /**< BOR使能                             */

#define FLASH_BOR_LEVEL_0                 FLASH_OPTR1_BOR_LEVEL_0                    /**< BOR检测电压上升/下降阈值：2.0/1.9V  */
#define FLASH_BOR_LEVEL_1                 FLASH_OPTR1_BOR_LEVEL_1                    /**< BOR检测电压上升/下降阈值：2.4/2.3V  */
#define FLASH_BOR_LEVEL_2                 FLASH_OPTR1_BOR_LEVEL_2                    /**< BOR检测电压上升/下降阈值：2.8/2.7V  */
#define FLASH_BOR_LEVEL_3                 FLASH_OPTR1_BOR_LEVEL_3                    /**< BOR检测电压上升/下降阈值：3.2/3.1V  */

#define FLASH_RDP_LEVEL_MASK              FLASH_OPTR1_RDPRP_MASK                     /**< RDP配置域       */
#define FLASH_RDP_LEVEL_0                 (0x00000000U)                              /**< RDP保护等级0    */
#define FLASH_RDP_LEVEL_1                 (0x00000001U)                              /**< RDP保护等级1    */

/* 选项字节2配置参数 */
#define FLASH_IWDG_STOP_MODE_STOP         (0x00000000U)                              /**< IWDG在Stop模式下停止计数                       */
#define FLASH_IWDG_STOP_MODE_NORMAL       FLASH_OPTR2_IWDG_STOP                      /**< IWDG在Stop模式下正常计数                       */

#define FLASH_STOP_RESET_ENABLE           (0x00000000U)                              /**< 执行进入Stop模式操作，不进入Stop，产生复位     */
#define FLASH_STOP_RESET_DISABLE          FLASH_OPTR2_RST_STOP                       /**< 执行进入Stop模式操作，正常进入Stop，不产生复位 */

/* 选项字节3配置参数 */
#define FLASH_WRP_AREA_0                  (0x0000003EU)                              /**< 写保护区域：0x00000000U ~ 0x00000FFF */
#define FLASH_WRP_AREA_1                  (0x0000003DU)                              /**< 写保护区域：0x00001000U ~ 0x00001FFF */
#define FLASH_WRP_AREA_2                  (0x0000003BU)                              /**< 写保护区域：0x00002000U ~ 0x00002FFF */
#define FLASH_WRP_AREA_3                  (0x00000037U)                              /**< 写保护区域：0x00003000U ~ 0x00003FFF */
#define FLASH_WRP_AREA_4                  (0x0000002FU)                              /**< 写保护区域：0x00004000U ~ 0x00004FFF */
#define FLASH_WRP_AREA_5                  (0x0000001FU)                              /**< 写保护区域：0x00005000U ~ 0x00005FFF */
#define FLASH_WRP_AREA_ALL                (0x00000000U)                              /**< User flash区域全部写保护             */
#define FLASH_WRP_AREA_NONE               (0x0000003FU)                              /**< User flash区域写保护关闭             */

/* 选项字节数量 */
#define FLASH_OB_NUM                      (3U)                                       /**< 选项字节个数            */

/* 选择字节出厂默认值 */
#define FLASH_OB1_DEFULT_VALUE            (0xFF5500AAU)                              /**< 选项字节1出厂默认值     */
#define FLASH_OB2_DEFULT_VALUE            (0xFFDE0021U)                              /**< 选项字节2出厂默认值     */
#define FLASH_OB3_DEFULT_VALUE            (0xFFC0003FU)                              /**< 选项字节3出厂默认值     */

/* 选项字节RDP编程位域及编程值 */
#define FLASH_OB_CONFIG_RDP_MASK          (0x000000FFU)                              /**< 选项字节RDPRP配置掩码   */
#define FLASH_OB_CONFIG_RDP_RDP0          (0x000000AAU)                              /**< 选项字节RDPRP配置为RDP0 */
#define FLASH_OB_CONFIG_RDP_RDP1          (0x00000055U)                              /**< 选项字节RDPRP配置为RDP1 */


/**
* @brief  计算Option Bytes区编程值
* @param  VAL OB区低16位预期编程值 
* @retval OB高16位取反后与低16位相加的值
*/
#define FLASH_OB_DATA_CALCULATE(VAL)      ((uint32_t)(((VAL) & 0xFFFF) | ((~(VAL & 0xFFFF)) << 16)))

/**
* @} 
*/

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @defgroup FLASH_External_Functions FLASH External Functions
* @brief    FLASH对外函数
* @{
*
*/
/************************************************************************************************/

/** 
* @brief  Flash控制寄存器解锁
* @note   FLASH控制器解锁后操作CRKEY寄存器会产生总线错误并锁定写保护，直至下次复位
* @retval 无
*/
__STATIC_INLINE void std_flash_unlock(void)
{
    if ((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
    {
        FLASH->CRKEY = FLASH_CR_KEY1;
        FLASH->CRKEY = FLASH_CR_KEY2; 
    }
}

/** 
* @brief  Flash控制寄存器锁定
* @retval 无
*/
__STATIC_INLINE void std_flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/** 
* @brief  读取Flash锁定状态
* @retval uint32_t Flash锁定状态
*             @arg 非0: 锁定
*             @arg 0:   未锁定
*/
__STATIC_INLINE uint32_t std_flash_get_lock_status(void)
{
    return (FLASH->CR & FLASH_CR_LOCK);
}

/** 
* @brief  Flash选项字节解锁
* @note   FLASH 选项字节解锁后操作OPTKEY寄存器会产生总线错误并锁定写保护，直至下次复位
* @retval 无
*/
__STATIC_INLINE void std_flash_opt_unlock(void)
{
    if ((FLASH->CR & FLASH_CR_OPTLOCK) == FLASH_CR_OPTLOCK)
    {
        FLASH->OPTKEY = FLASH_OPT_KEY1;
        FLASH->OPTKEY = FLASH_OPT_KEY2; 
    }
}

/** 
* @brief  Flash选项字节锁定
* @retval 无
*/
__STATIC_INLINE void std_flash_opt_lock(void)
{
    FLASH->CR |= FLASH_CR_OPTLOCK;
}

/** 
* @brief  读取Flash选项字节锁定状态
* @retval uint32_t Flash选项字节锁定状态
*             @arg 非0: 锁定
*             @arg 0:   未锁定
*/
__STATIC_INLINE uint32_t std_flash_get_opt_lock_status(void)
{
    return (FLASH->CR & FLASH_CR_OPTLOCK);
}

/**
* @brief  设置FLASH读取访问等待周期
* @param  latency 读取访问等待周期
*             @arg  FLASH_LATENCY_0CLK
*             @arg  FLASH_LATENCY_1CLK
* @retval 无
*/
__STATIC_INLINE void std_flash_set_latency(uint32_t latency)
{
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, latency);
} 

/**
* @brief  读取FLASH读取访问等待周期
* @retval uint32_t 读取访问等待周期
*             @arg  FLASH_LATENCY_0CLK
*             @arg  FLASH_LATENCY_1CLK
*/
__STATIC_INLINE uint32_t std_flash_get_latency(void)
{
    return (FLASH->ACR & FLASH_ACR_LATENCY);
} 

/**
* @brief  使能FLASH中断
* @param  interrupts 中断源
*             @arg FLASH_INTERRUPT_OPERR
*             @arg FLASH_INTERRUPT_EOP 
* @retval 无
*/
__STATIC_INLINE void std_flash_interrupt_enable(uint32_t interrupts)
{
    FLASH->CR |= interrupts;
}

/**
* @brief  禁止FLASH中断
* @param  interrupts 中断源
*             @arg FLASH_INTERRUPT_OPERR
*             @arg FLASH_INTERRUPT_EOP 
* @retval 无
*/
__STATIC_INLINE void std_flash_interrupt_disable(uint32_t interrupts)
{
    FLASH->CR &= (~interrupts);
}

/**
* @brief  读取FLASH中断使能状态
* @param  interrupt 中断源
*             @arg FLASH_INTERRUPT_OPERR
*             @arg FLASH_INTERRUPT_EOP 
* @retval uint32_t FLASH中断使能状态
*             @arg 非0: 使能
*             @arg 0:   禁止
*/
__STATIC_INLINE uint32_t std_flash_get_interrupt_enable(uint32_t interrupt)
{
    return (FLASH->CR & interrupt);
}

/**
* @brief  读取FLASH状态标志
* @param  flag 状态标志
*             @arg FLASH_FLAG_EOP
*             @arg FLASH_FLAG_BSY
*             @arg FLASH_FLAG_OPTVERR
*             @arg FLASH_FLAG_WRPERR
* @retval uint32_t FLASH状态标志
*             @arg 非0: 标志位置位
*             @arg 0:   标志位清除
*/
__STATIC_INLINE uint32_t std_flash_get_flag(uint32_t flag)
{
    return (FLASH->SR & flag);
}

/**
* @brief  清除FLASH状态标志
* @param  flags 状态标志组合
*             @arg FLASH_FLAG_EOP
*             @arg FLASH_FLAG_WRPERR
* @retval 无
*/
__STATIC_INLINE void std_flash_clear_flag(uint32_t flags)
{
    FLASH->SR = flags;
}

/** 
* @brief  设置Flash操作模式
* @param  mode 擦除模式
*             @arg FLASH_MODE_IDLE
*             @arg FLASH_MODE_PAGE_ERASE
*             @arg FLASH_MODE_MASS_ERASE
*             @arg FLASH_MODE_PROGRAM
* @retval 无
*/
__STATIC_INLINE void std_flash_set_operate_mode(uint32_t mode)
{
    MODIFY_REG(FLASH->CR, FLASH_CR_OP_MODE, mode);
}

/**
* @brief  读取选项字节寄存器1
* @retval uint32_t 选项字节1加载值
*/
__STATIC_INLINE uint32_t std_flash_get_opt1(void)
{
    return (FLASH->OPTR1);
}

/**
* @brief  读取FLASH读保护等级
* @retval uint32_t FLASH读保护等级
*             @arg FLASH_RDP_LEVEL_0
*             @arg FLASH_RDP_LEVEL_1
*/
__STATIC_INLINE uint32_t std_flash_get_rdp_level(void)
{
    return (FLASH->OPTR1 & FLASH_OPTR1_RDPRP);
}

/**
* @brief  读取BOR阈值等级
* @retval uint32_t FLASH读保护等级
*             @arg FLASH_BOR_LEVEL_0
*             @arg ...
*             @arg FLASH_BOR_LEVEL_3
*/
__STATIC_INLINE uint32_t std_flash_get_bor_level(void)
{
    return (FLASH->OPTR1 & FLASH_OPTR1_BOR_LEVEL);
}

/** 
* @brief  读取BOR使能控制
* @retval uint32_t BOR使能控制
*             @arg FLASH_BOR_DISABLE
*             @arg FLASH_BOR_ENABLE
*/
__STATIC_INLINE uint32_t std_flash_get_bor_en(void)
{
    return (FLASH->OPTR1 & FLASH_OPTR1_BOR_EN);
}

/** 
* @brief  读取NRST SWD引脚功能控制
* @retval uint32_t NRST SWD引脚功能选择
*             @arg FLASH_PIN_MODE_NRST_SWD
*             @arg FLASH_PIN_MODE_GPIO_SWD
*             @arg FLASH_PIN_MODE_SWD_GPIO
*/
__STATIC_INLINE uint32_t std_flash_get_nrst_swd_mode(void)
{
    return (FLASH->OPTR1 & FLASH_PIN_MODE_MASK);
}

/**
* @brief  读取选项字节寄存器2
* @retval uint32_t 选项字节2加载值
*/
__STATIC_INLINE uint32_t std_flash_get_opt2(void)
{
    return (FLASH->OPTR2);
}

/** 
* @brief  读取IWDG在STOP模式下计数器停止控制
* @retval uint32_t IWDG_STOP设置
*             @arg FLASH_IWDG_STOP_MODE_STOP
*             @arg FLASH_IWDG_STOP_MODE_NORMAL
*/
__STATIC_INLINE uint32_t std_flash_get_iwdg_stop(void)
{
    return (FLASH->OPTR2 & FLASH_OPTR2_IWDG_STOP);
}

/** 
* @brief  读取进入STOP模式是否复位的配置
* @retval uint32_t RST_STOP设置
*             @arg FLASH_STOP_RESET_ENABLE
*             @arg FLASH_STOP_RESET_DISABLE
*/
__STATIC_INLINE uint32_t std_flash_get_rst_stop(void)
{
    return (FLASH->OPTR2 & FLASH_OPTR2_RST_STOP);
}

/**
* @brief  读取写保护区配置值
* @retval uint32_t 写保护区配置值
*/
__STATIC_INLINE uint32_t std_flash_get_wrp(void)
{
    return (FLASH->WRP & FLASH_WRP_WRP);
}

/** 
* @brief  读取芯片型号
* @retval uint32_t 芯片型号
*/
__STATIC_INLINE uint32_t std_flash_get_device_type(void)
{
    return (*(uint32_t *)DEVICE_TYPE);
}

/** 
* @brief  读取Flash空间大小
* @retval uint32_t Flash空间大小
*/
__STATIC_INLINE uint32_t std_flash_get_flash_size(void)
{
    return (*(uint32_t *)USERFLASH_SIZE);
}

/** 
* @brief  读取SRAM空间大小
* @retval uint32_t SRAM空间大小
*/
__STATIC_INLINE uint32_t std_flash_get_sram_size(void)
{
    return (*(uint32_t *)SRAM_SIZE);
}


std_status_t std_flash_erase(uint32_t mode, uint32_t address);
std_status_t std_flash_word_program(uint32_t address, uint32_t prog_data);
std_status_t std_flash_option_bytes_config(std_flash_option_config_t *flash_option_config);

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

#endif /* CIU32F003_STD_FLASH_H */



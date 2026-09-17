/************************************************************************************************/
/**
* @file               ciu32f003_std_flash.c
* @author             MCU Ecosystem Development Team
* @brief              FLASH STD库驱动。
*                     实现FLASH擦除、编程API。
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
* @addtogroup FLASH 
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"

#ifdef STD_FLASH_PERIPHERAL_USED

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @addtogroup FLASH_External_Functions 
* @{
*
*/
/************************************************************************************************/ 

/**
* @brief  Flash擦除
* @param  mode 擦除模式
*             @arg FLASH_MODE_PAGE_ERASE
*             @arg FLASH_MODE_MASS_ERASE
* @param  address 擦除访问地址
* @note   user flash区擦除时，需先调std_flash_unlock()，解锁flash
* @note   Option Byte区擦除时，需先调用std_flash_unlock()，再调用std_flash_opt_unlock()，解锁选项字节
* @retval std_status_t API执行结果
*/
std_status_t std_flash_erase(uint32_t mode, uint32_t address)
{
    std_status_t status = STD_OK;
    
    /* 设置擦除模式 */
    std_flash_set_operate_mode(mode);
    
    /* 执行擦除 */
    *(uint32_t *)address = 0xFFFFFFFF;
    
    /* 等待擦除完成，查询异常标志位 */
    while (std_flash_get_flag(FLASH_FLAG_BSY));
    if ((FLASH->SR & FLASH_FLAG_WRPERR) != 0x00000000U)
    {
        status = STD_ERR;
    }
    
    /* 清除所有标志 */
    std_flash_clear_flag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);
    
    /* 退出擦除模式 */
    std_flash_set_operate_mode(FLASH_MODE_IDLE);
    
    return (status);
}


/**
* @brief  Flash区字编程
* @param  address   编程地址
* @param  prog_data 编程数据（4字节）
* @note   user flash区编程时，需先调std_flash_unlock()，解锁flash
* @note   Option Byte区字编程时，需先调用std_flash_unlock()，再调用std_flash_opt_unlock()，解锁选项字节
* @retval std_status_t API执行结果
*/
std_status_t std_flash_word_program(uint32_t address, uint32_t prog_data)
{
    std_status_t status = STD_OK;
    
    /* 进入编程模式 */
    std_flash_set_operate_mode(FLASH_MODE_PROGRAM);

    /* 向目标地址写入数据 */
    *(uint32_t *)address = prog_data;
    
    /* 等待编程完成，查询异常标志位 */
    while (std_flash_get_flag(FLASH_FLAG_BSY));
    if ((FLASH->SR & FLASH_FLAG_WRPERR) != 0x00000000U)
    {
        status = STD_ERR;
    }
    
    if (status == STD_OK)
    {
        /* 检查编程数据是否正确 */
        if(*((__IO uint32_t *)address) != prog_data)
        {
            status = STD_ERR;
        }
    }
    
    /* 清除所有标志 */
    std_flash_clear_flag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);
    
    /* 退出编程模式 */
    std_flash_set_operate_mode(FLASH_MODE_IDLE);
    
    return (status);
}

/**
* @brief  选项字节配置更新
* @param  flash_option_config 选项字节配置结构体
* @note   选项字节更新过程中产生断电或复位会导致数据写入异常，从而加载非预期值，调用该函数时需注意该风险
* @note   本函数基于出厂默认值，根据传入的参数配置进行选项字节更新
* @note   Option Bytes区配置更新时，需先调用std_flash_unlock()，再调用std_flash_opt_unlock()，解锁选项字节
* @retval std_status_t API执行结果
*/
std_status_t std_flash_option_bytes_config(std_flash_option_config_t *flash_option_config)
{
    uint32_t opt_value[FLASH_OB_NUM], index;
    std_status_t status = STD_OK;
    
    /* 选项字节出厂默认值 */
    opt_value[0] = FLASH_OB1_DEFULT_VALUE;
    opt_value[1] = FLASH_OB2_DEFULT_VALUE;
    opt_value[2] = FLASH_OB3_DEFULT_VALUE;
    
    /* 根据选项字节配置开关更新对应选项字节 */
    
    /* 更新BOR相关配置 */
    if((flash_option_config->config_select & FLASH_OB_CONFIG_BOR) == FLASH_OB_CONFIG_BOR) 
    {
        opt_value[0] &= ~(FLASH_OPTR1_BOR_EN_MASK | FLASH_OPTR1_BOR_LEVEL_MASK);
        opt_value[0] |= (flash_option_config->bor_enable | flash_option_config->bor_level);
    }
    
    /* 更新NRST/SWDIO引脚配置 */
    if((flash_option_config->config_select & FLASH_OB_CONFIG_NRST_SWD) == FLASH_OB_CONFIG_NRST_SWD)
    {
        opt_value[0] &= ~(FLASH_OPTR1_NRST_SWD_MODE_MASK);
        opt_value[0] |= flash_option_config->nrst_swd_mode;
    }
    
    /* 更新低功耗模式复位配置 */
    if((flash_option_config->config_select & FLASH_OB_CONFIG_RST_STOP) == FLASH_OB_CONFIG_RST_STOP)
    {
        opt_value[1] &= ~(FLASH_OPTR2_RST_STOP_MASK);
        opt_value[1] |= flash_option_config->reset_stop;
    }
    
    /* 更新IWDG低功耗计数配置 */
    if((flash_option_config->config_select & FLASH_OB_CONFIG_IWDG_STOP) == FLASH_OB_CONFIG_IWDG_STOP)
    {
        opt_value[1] &= ~(FLASH_OPTR2_IWDG_STOP_MASK);
        opt_value[1] |= flash_option_config->iwdg_stop;
    }
    
    /* 更新WRP配置 */
    if((flash_option_config->config_select & FLASH_OB_CONFIG_WRP) == FLASH_OB_CONFIG_WRP)
    {
        opt_value[2] &= ~(FLASH_WRP_WRP_MASK);
        opt_value[2] = flash_option_config->wrp_config;
    }
    
    /* 更新RDP配置 */
    if((flash_option_config->config_select & FLASH_OB_CONFIG_RDP) == FLASH_OB_CONFIG_RDP)
    {
        opt_value[0] &= ~FLASH_OB_CONFIG_RDP_MASK;
        
        if(flash_option_config->rdp_level == FLASH_RDP_LEVEL_1)
        {
            opt_value[0] |= FLASH_OB_CONFIG_RDP_RDP1;
        }
        else
        {
            opt_value[0] |= FLASH_OB_CONFIG_RDP_RDP0;
        }
    }
    
    /* 计算选项字节高位反码 */
    for(index = 0; index < FLASH_OB_NUM; index++)
    {
        opt_value[index] = FLASH_OB_DATA_CALCULATE(opt_value[index]);
    }
    
    /* 选项字节擦除 */
    if (STD_OK != std_flash_erase(FLASH_MODE_PAGE_ERASE, FLASH_OB_OPTR1_ADDR))
    {
        status = STD_ERR;
        return status;
    }
    
    /* 编程选项字节 */
    for(index = 0; index < FLASH_OB_NUM; index++)
    {
        if (STD_OK != std_flash_word_program((FLASH_OB_OPTR1_ADDR + (index << 2)), opt_value[index]))
        {
            status = STD_ERR;
            return status;
        }
    }
    
    return (status);
}

/** 
* @} 
*/

#endif /* STD_FLASH_PERIPHERAL_USED */

/** 
* @} 
*/

/** 
* @} 
*/

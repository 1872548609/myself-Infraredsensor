/************************************************************************************************/
/**
* @file               flash_bsp.c
* @author             MCU Ecosystem Development Team
* @brief              FLASH BSP驱动函数，实现FLASH功能配置。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "flash_bsp.h"

/*-------------------------------------------define---------------------------------------------*/
#define RETRY_NUM       (3U)

/*------------------------------------------functions-------------------------------------------*/
/**
* @brief  Flash Option Bytes配置
* @note   更新NRST引脚复位释放后切换成GPIO，更新IWDG在Stop/Deepstop模式下停止计数，其余选项字节为出厂默认值
* @retval 无
*/
void bsp_flash_option_byte_config(void)
{
    std_flash_option_config_t ob_config = {0};
    uint32_t update_num;
    
    /* 检查是否需要更新选项字节 */
    if((std_flash_get_nrst_swd_mode() != FLASH_PIN_MODE_GPIO_SWD) \
    || (std_flash_get_iwdg_stop() != FLASH_IWDG_STOP_MODE_STOP))
    {
        /* Flash控制寄存器和选项字节解锁 */
        std_flash_unlock();
        std_flash_opt_unlock();
        
        /* 选择更新NRST_SWD_MODE和IWDG_STOP选项字节配置 */
        ob_config.config_select = (FLASH_OB_CONFIG_NRST_SWD | FLASH_OB_CONFIG_IWDG_STOP);
        
        /* 设置PC0引脚复位释放前后默认复用为GPIO */
        ob_config.nrst_swd_mode = FLASH_PIN_MODE_GPIO_SWD;
        
        /* 设置IWDG在低功耗模式Stop/Deepstop停止计数 */
        ob_config.iwdg_stop = FLASH_IWDG_STOP_MODE_STOP;
        
        /* 更新选项字节配置，若产生异常则重试多次 */
        for(update_num=0; update_num<RETRY_NUM; update_num++)
        {
            if(STD_OK == std_flash_option_bytes_config(&ob_config))
            {
                break;
            }
        }
        
        /* 多次重试仍异常进入错误处理 */
        if(update_num >= RETRY_NUM)
        {
            error_process();
        }
        
        /* Flash控制寄存器和选项字节加锁 */
        std_flash_opt_lock();
        std_flash_lock();
        
        /* 取消注释后可实现Flash选项字节的加载操作 */
        /* NVIC_SystemReset(); */
    }
}


/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示Flash的页擦与编程功能。
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*------------------------------------------includes--------------------------------------------*/
#include "main.h"

/*------------------------------------define----------------------------------------------------*/
/* Flash编程字的个数 */
#define PAGE_WORD_COUNT           (0x20)
/* Flash擦写起始页编号：第32页 */
#define FLASH_PAGE_NUM            (0x20)
/* Flash擦写起始地址：0x00004000 */
#define FLASH_ERASE_PROGRAM_ADDR  (FLASH_MEM_BASE + (FLASH_PAGE_NUM * 0x200))  

/*-------------------------------------------functions------------------------------------------*/
int main(void)
{
    uint32_t program_data;
    uint32_t word_count;
    
    /* 配置系统时钟 */
    system_clock_config(); 

    /* 初始化编程缓冲区 */
    program_data = 0x5A5A5A5A;

    /* 清除所有标志位 */
    std_flash_clear_flag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);

    /* Flash控制寄存器解锁 */
    std_flash_unlock();
    
    /* Flash擦除 */
    if (STD_OK != std_flash_erase(FLASH_MODE_PAGE_ERASE, FLASH_ERASE_PROGRAM_ADDR))
    {
        error_process();
    }
        
    /* Flash编程 */
    for (word_count = 0; word_count < PAGE_WORD_COUNT; word_count++)
    {
        if (STD_OK != std_flash_word_program((FLASH_ERASE_PROGRAM_ADDR + (word_count << 2)), program_data))
        {
            error_process();
        }
        
        /* 校验编程数据 */
        if (*(uint32_t *)(FLASH_ERASE_PROGRAM_ADDR + (word_count << 2)) != program_data)
        {
            /* 校验异常，加入自定义处理代码 */
            error_process();
        }
    }
        
    /* Flash控制寄存器加锁 */
    std_flash_lock();
    
    while(1)
    {
    }
}

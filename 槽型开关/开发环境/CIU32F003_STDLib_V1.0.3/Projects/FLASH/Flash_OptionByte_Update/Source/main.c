/************************************************************************************************/
/**
* @file               main.c
* @author             MCU Ecosystem Development Team
* @brief              该示例展示Flash选项字节的擦除、编程与更新功能。
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

/*------------------------------------------functions-------------------------------------------*/
int main(void)
{   
    /* 配置系统时钟 */
    system_clock_config(); 
    
    /* GPIO初始化 */
    gpio_init();
    
    /* 等待用户按下USER按键以启动选项字节更新 */
    while (std_gpio_get_input_pin(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN) == true);
        
    /* 清除所有标志 */
    std_flash_clear_flag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);

    /* 选项字节擦除与编程 */
    bsp_flash_option_byte_config();
    
    while(1)
    {

    }
}

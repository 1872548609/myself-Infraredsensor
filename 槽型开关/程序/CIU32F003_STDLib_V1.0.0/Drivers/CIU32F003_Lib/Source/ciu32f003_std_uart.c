/************************************************************************************************/
/**
* @file               ciu32f003_std_uart.c
* @author             MCU Ecosystem Development Team
* @brief              UART STD库驱动。
*                     实现UART初始化等API。
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
* @addtogroup  UART
* @{
*
*/
/************************************************************************************************/

/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"

#ifdef STD_UART_PERIPHERAL_USED
/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @addtogroup UART_External_Functions 
* @{
*
*/
/************************************************************************************************/ 

/**
* @brief  UART初始化
* @param  uartx UART外设
* @param  uart_init_param UART初始化结构体
* @retval  无
*/
void std_uart_init(UART_t *uartx,std_uart_init_t *uart_init_param)
{
    uint32_t pclk;
   
    /* 设置 UART 字长 、收发模式、校验*/
    MODIFY_REG(uartx->CR1,
              (UART_CR1_WL|UART_CR1_TE|UART_CR1_RE|UART_CR1_PTS|UART_CR1_PEN),
              (uart_init_param->wordlength|uart_init_param->direction|uart_init_param->parity));
     
    /* 设置UART停止位 */
    std_uart_set_stopbits(uartx,uart_init_param->stopbits);
           
     /* 获取UART时钟频率 */
    pclk = std_rcc_get_pclkfreq();
 
    /* BRR取值范围[0x10,0xFFFF] */    
    if(uart_init_param->baudrate != 0)
    {
        uartx->BRR = (pclk + (uart_init_param->baudrate>>1))/uart_init_param->baudrate;
    }
}

/**
* @brief  UART去初始化
* @param  uartx UART外设
* @retval 无
*/
void std_uart_deinit(UART_t *uartx)
{
    /* UART 的RCC时钟复位 */
    if(uartx == UART1)
    {
        std_rcc_apb2_reset(RCC_PERIPH_RESET_UART1);
    }
    else if(uartx == UART2)
    {
        std_rcc_apb1_reset(RCC_PERIPH_RESET_UART2);
    }
}

/**
* @brief  UART结构体初始化
* @param  uart_init_struct UART初始化结构体
* @retval 无
*/
void std_uart_struct_init(std_uart_init_t *uart_init_struct)
{ 
    uart_init_struct->baudrate = 115200;
    uart_init_struct->wordlength = UART_WORDLENGTH_8BITS;
    uart_init_struct->stopbits = UART_STOPBITS_1;
    uart_init_struct->direction = UART_DIRECTION_SEND_RECEIVE;
    uart_init_struct->parity = UART_PARITY_NONE;
}

/** 
* @} 
*/

#endif /* STD_UART_PERIPHERAL_USED */

/** 
* @} 
*/

/** 
* @} 
*/


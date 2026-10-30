/************************************************************************************************/
/**
* @file               ciu32f003_std_spi.c
* @author             MCU Ecosystem Development Team
* @brief              SPI STD库驱动。
*                     实现SPI初始化等API。
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
* @addtogroup SPI 
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"

#ifdef STD_SPI_PERIPHERAL_USED

/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @addtogroup SPI_External_Functions 
* @{
*
*/
/************************************************************************************************/ 
   
/** 
* @brief  SPI初始化
* @param  spi_init_param SPI初始化结构体   
* @retval 无
*/
void std_spi_init(std_spi_init_t *spi_init_param)
{
    /* SPI初始化：
       - 配置SPI通信速率
       - 配置SPI时钟极性
       - 配置SPI时钟相位
       - 配置SPI数据大小端
    */
    MODIFY_REG(SPI1->CR1, (SPI_CR1_BR | SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST),
                          (spi_init_param->baud_rate_prescaler
                         | spi_init_param->clk_polarity
                         | spi_init_param->clk_phase
                         | spi_init_param->bitorder));
    
    /* 配置SPI主从模式 */
    MODIFY_REG(SPI1->CR1, SPI_CR1_MSTR, spi_init_param->mode);
}

/**
* @brief  SPI去初始化
* @retval 无
*/
void std_spi_deinit(void)
{
    std_rcc_apb2_reset(RCC_PERIPH_RESET_SPI1);
} 

/**
* @brief  SPI结构体初始化
* @param  spi_init_struct SPI初始化结构体 
* @retval 无
*/
void std_spi_struct_init(std_spi_init_t *spi_init_struct)
{
    spi_init_struct->mode = SPI_MODE_MASTER;
    spi_init_struct->baud_rate_prescaler = SPI_BAUD_PCLKDIV_128;
    spi_init_struct->clk_polarity = SPI_POLARITY_LOW;
    spi_init_struct->clk_phase = SPI_PHASE_1EDGE;
    spi_init_struct->bitorder = SPI_FIRSTBIT_MSB;
}

/** 
* @} 
*/

#endif /* STD_SPI_PERIPHERAL_USED */

/** 
* @} 
*/

/** 
* @} 
*/


/************************************************************************************************/
/**
* @file               ciu32f003_std_config.h
* @author             MCU Ecosystem Development Team
* @brief              STD库相关配置项定义。
*                     
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/*避免头文件重复引用*/
#ifndef CIU32F003_STD_CONFIG_H
#define CIU32F003_STD_CONFIG_H   

#ifdef __cplusplus
extern "C" {
#endif

    
/*--------------------------------------------define--------------------------------------------*/
/* 定义CIU32F003 STD库中所有芯片外设的使用开关 */
#define STD_ADC_PERIPHERAL_USED
#define STD_COMP_PERIPHERAL_USED
#define STD_CRC_PERIPHERAL_USED
#define STD_DBG_PERIPHERAL_USED
#define STD_EXTI_PERIPHERAL_USED 
#define STD_FLASH_PERIPHERAL_USED
#define STD_GPIO_PERIPHERAL_USED
#define STD_I2C_PERIPHERAL_USED
#define STD_IRTIM_PERIPHERAL_USED
#define STD_IWDG_PERIPHERAL_USED
#define STD_LPTIM_PERIPHERAL_USED
#define STD_PMU_PERIPHERAL_USED
#define STD_SPI_PERIPHERAL_USED
#define STD_TIM_PERIPHERAL_USED
#define STD_UART_PERIPHERAL_USED  


#if !defined  (EXTCLK_VALUE)  
    #define EXTCLK_VALUE    (8000000UL)     /**< EXTclock frequency(Hz)  */
#endif 
#if !defined  (RCH_VALUE)
    #define RCH_VALUE      (48000000UL)     /**< RCH clock frequency(Hz) */
#endif 

#if !defined  (RCL_VALUE)
    #define RCL_VALUE      (32000UL)        /**< RCL clock frequency(Hz) */
#endif 



/*------------------------------------------includes--------------------------------------------*/

/* 根据外设配置，引用外设头文件 */
#include "ciu32f003_std_rcc.h"
    
#ifdef STD_ADC_PERIPHERAL_USED
    #include "ciu32f003_std_adc.h"
#endif 

#ifdef STD_COMP_PERIPHERAL_USED
    #include "ciu32f003_std_comp.h"
#endif 

#ifdef STD_CRC_PERIPHERAL_USED
    #include "ciu32f003_std_crc.h"
#endif 

#ifdef STD_DBG_PERIPHERAL_USED
    #include "ciu32f003_std_dbg.h"
#endif 

#ifdef STD_EXTI_PERIPHERAL_USED
    #include "ciu32f003_std_exti.h"
#endif 

#ifdef STD_FLASH_PERIPHERAL_USED
    #include "ciu32f003_std_flash.h"
#endif 

#ifdef STD_GPIO_PERIPHERAL_USED
    #include "ciu32f003_std_gpio.h"
#endif 

#ifdef STD_I2C_PERIPHERAL_USED
    #include "ciu32f003_std_i2c.h"
#endif 

#ifdef STD_IRTIM_PERIPHERAL_USED
    #include "ciu32f003_std_irtim.h"
#endif 

#ifdef STD_IWDG_PERIPHERAL_USED
    #include "ciu32f003_std_iwdg.h"
#endif 

#ifdef STD_LPTIM_PERIPHERAL_USED
    #include "ciu32f003_std_lptim.h"
#endif 

#ifdef STD_PMU_PERIPHERAL_USED
    #include "ciu32f003_std_pmu.h"
#endif 

#ifdef STD_SPI_PERIPHERAL_USED
    #include "ciu32f003_std_spi.h"
#endif

#ifdef STD_TIM_PERIPHERAL_USED
    #include "ciu32f003_std_tim.h"
#endif

#ifdef STD_UART_PERIPHERAL_USED
    #include "ciu32f003_std_uart.h"
#endif


/*-------------------------------------------functions------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CIU32F003_STD_CONFIG_H */


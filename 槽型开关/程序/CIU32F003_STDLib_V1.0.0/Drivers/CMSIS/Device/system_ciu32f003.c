/************************************************************************************************/
/**
* @file               system_ciu32f003.c
* @author             MCU Ecosystem Development Team
* @brief              CMSIS Device System Source File for CIU32F003.                            
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
* @addtogroup CMSIS
* @{
*
* @addtogroup Device_System  Device System
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003.h"


/*--------------------------------------------define--------------------------------------------*/
/************************************************************************************************/
/**
* @defgroup Device_System_Constants Device System Constants
* @{
*/
/************************************************************************************************/
#if !defined  (EXTCLK_VALUE)  
    #define EXTCLK_VALUE    (8000000UL)      /**< EXT clock frequency(Hz) */
#endif 
#if !defined  (RCH_VALUE)
    #define RCH_VALUE      (48000000UL)     /**< RCH clock frequency(Hz) */
#endif 

#if !defined  (RCL_VALUE)
    #define RCL_VALUE      (32000UL)        /**< RCL clock frequency(Hz) */
#endif 

/** 
* @} 
*/

/*--------------------------------------------variables-----------------------------------------*/
/* The SystemCoreClock variable is the system core clock(HCLK) frequency.*/
uint32_t SystemCoreClock = RCH_VALUE/6;

const uint32_t g_ahb_divider_table[8] = {1UL, 2UL, 4UL, 8UL, 16UL, 32UL, 64UL, 128UL};
const uint32_t g_apb_divider_table[8] = {1UL, 1UL, 1UL, 1UL, 2UL, 4UL, 8UL, 16UL};


#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    extern uint32_t __vector_table;
#endif



/*-------------------------------------------functions------------------------------------------*/

/************************************************************************************************/
/**
* @addtogroup Device_System_External_Functions 
* @{
*
*/
/************************************************************************************************/

/**
* @brief  MCU system initialization function
* @retval None
*/
void SystemInit(void)
{
    /* Configure the vector table location add offset address */
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) &__vector_table; 
#endif
    
    SystemCoreClock = RCH_VALUE/6; 
}

/**
* @brief  System core clock(HCLK) update function
*
* @note   This function is used to update the variable SystemCoreClock 
*         and must be called whenever HCLK is changed
* @retval None
*/

void SystemCoreClockUpdate(void)
{
    uint32_t tmp,tmp_clock;
    
    /* Get SYSCLK source */
    switch (RCC->CFG & RCC_CFG_SYSWS_MASK)
    {       
        case RCC_CFG_SYSWS_RCHDIV3:  /* RCHDIV3 used as system clock */
        {
            tmp_clock = RCH_VALUE/3;
        }break;

        case RCC_CFG_SYSWS_RCH:    /* RCH used as system clock */
        {
            tmp_clock = RCH_VALUE;
        }break;
        
        case RCC_CFG_SYSWS_RCL:    /* RCL used as system clock */
        {
            tmp_clock = RCL_VALUE;
        }break;
        
        case RCC_CFG_SYSWS_EXTCLK:  /* EXTCLK used as system clock */
        {
            tmp_clock = EXTCLK_VALUE;
        }break;
      
        case RCC_CFG_SYSWS_RCHDIV6:  /* RCHDIV6 used as system clock */
        default:                     /* RCH used as system clock     */
        {
            tmp_clock = RCH_VALUE/6;
        }break;
    }
        
    /* Compute HCLK clock frequency */
    tmp = g_ahb_divider_table[((RCC->CFG & RCC_CFG_HPRE_MASK) >> RCC_CFG_HPRE_POS)];

    SystemCoreClock = tmp_clock/tmp;
}


/** 
* @} 
*/

/** 
* @} 
*/

/** 
* @} 
*/

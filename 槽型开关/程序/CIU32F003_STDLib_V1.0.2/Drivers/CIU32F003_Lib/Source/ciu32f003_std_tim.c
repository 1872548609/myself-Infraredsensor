/************************************************************************************************/
/**
* @file               ciu32f003_std_tim.c
* @author             MCU Ecosystem Development Team
* @brief              TIM STD库驱动。
*                     实现TIM基本计数、输入捕获、输出比较等功能初始化API。
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
* @addtogroup TIM
* @{
*
*/
/************************************************************************************************/


/*------------------------------------------includes--------------------------------------------*/
#include "ciu32f003_std.h"

#ifdef STD_TIM_PERIPHERAL_USED

/*-------------------------------------------functions------------------------------------------*/
/************************************************************************************************/
/**
* @addtogroup TIM_External_Functions 
* @{
*
*/
/************************************************************************************************/ 
/**
* @brief  TIM初始化
* @param  timx TIM外设
* @param  tim_init_param TIM初始化结构体
* @note   由于DIR位在中心对齐模式下为只读，从中心对齐模式切换到边沿对齐计数器模式(或反向)，
*         需要停止计数后修改，以避免造成计数异常。
* @retval 无
*/
void std_tim_init(TIM_t *timx, std_tim_basic_init_t *tim_init_param)
{   
    if(TIM1 == timx)
    {
        /* 先切换到边沿模式，才能修改DIR控制位 */   
        timx->CR1 &= (~TIM_CR1_CMS);
        
        /* 选择计数器模式及时钟分频参数 */    
        MODIFY_REG(timx->CR1,
                  ((TIM_CR1_DIR | TIM_CR1_CMS) | TIM_CR1_CLK_DIV),
                  (tim_init_param->counter_mode | tim_init_param->clock_div));
            
        /* 设置重复计数参数 */
        std_tim_set_repcounter(timx, tim_init_param->repeat_counter);
    }
    else if(TIM3 == timx)
    {
        /* 设置时钟分频参数 */    
        MODIFY_REG(timx->CR1, TIM_CR1_CLK_DIV, tim_init_param->clock_div);
    }
    
    /* 设置自动加载值 */
    std_tim_set_autoreload(timx, tim_init_param->period);       
    
    /* 设置预分频值 */
    std_tim_set_psc(timx, tim_init_param->prescaler);
       
    /* 生成一个更新事件来重新加载预预分频值 */
    /* 如果支持RCR模式，则更新事件也将重新加载重复计数器值 */
    std_tim_set_sw_trig_event(timx, TIM_EVENT_SRC_UPDATE);
}

/**
* @brief  TIM去初始化
* @param  timx TIM外设
* @retval 无
*/
void std_tim_deinit(TIM_t *timx)    
{       
    /* 复位外设 */
    if(TIM1 == timx)
    {
        std_rcc_apb2_reset(RCC_PERIPH_RESET_TIM1);
    }
    else if(TIM3 == timx)
    {
        std_rcc_apb1_reset(RCC_PERIPH_RESET_TIM3);
    }
}

/**
* @brief  设置std_tim_basic_init_t结构体为默认值
* @param  tim_init_struct TIM初始化结构体
* @retval 无
*/
void std_tim_struct_init(std_tim_basic_init_t *tim_init_struct)
{
    tim_init_struct->prescaler             = 0x0000U;
    tim_init_struct->counter_mode          = TIM_COUNTER_MODE_UP;
    tim_init_struct->period                = 0xFFFFU;
    tim_init_struct->clock_div             = TIM_CLOCK_DTS_DIV1;
    tim_init_struct->repeat_counter        = 0x0000U;
}


/**
* @brief  配置TIM输入捕获通道
* @param  timx TIM外设
* @param  input_config TIM 输入捕获配置结构体
* @param  channel_id TIM 通道配置
*             @arg TIM_CHANNEL_1
*             @arg TIM_CHANNEL_2
* @retval 无
*/
void std_tim_input_capture_init(TIM_t *timx, std_tim_input_capture_init_t *input_config, uint32_t channel_id)
{         
    uint32_t tmp_value = ((channel_id & 0x01) == 0)?0U:8U;
    uint32_t shift_value = ((channel_id & 0x02) == 0)?0U:4U;        
    __IO uint32_t *preg = (__IO uint32_t *)((uint32_t)(&timx->CCM1) + shift_value);
    
    /* 禁止CCxE位 */
    timx->CCEN &= (~(TIM_CCEN_CC1E << (channel_id << 2)));

    /* 选择输入源和滤波及输入预分频参数 */
    MODIFY_REG(*preg,
              ((TIM_CCM1_CC1S | TIM_CCM1_IC1F |TIM_CCM1_IC1PSC ) << tmp_value),
              ((input_config->input_capture_sel | (input_config->input_capture_filter << 4U) | input_config->input_capture_prescaler) << tmp_value));
    
    /* 选择极性 */
    std_tim_set_input_pol(timx, channel_id, input_config->input_capture_pol);
}


/**
* @brief  设置std_tim_input_capture_init_t结构体为默认值
* @param  input_init_struct TIM输入捕获结构体
* @retval 无
*/
void std_tim_input_capture_struct_init(std_tim_input_capture_init_t *input_init_struct)
{
    input_init_struct->input_capture_pol       = TIM_INPUT_POL_RISING;
    input_init_struct->input_capture_sel       = TIM_INPUT_CAPTURE_SEL_DIRECTTI;
    input_init_struct->input_capture_prescaler = TIM_INPUT_CAPTURE_PSC_DIV1;
    input_init_struct->input_capture_filter    = 0x00U;
}


/**
* @brief  配置TIM比较输出参数
* @param  timx TIM外设
* @param  output_config TIM 输出比较配置结构体
* @param  channel_id TIM 通道配置
*             @arg TIM_CHANNEL_1
*             @arg TIM_CHANNEL_2
*             @arg TIM_CHANNEL_3
*             @arg TIM_CHANNEL_4
* @retval 无
*/
void std_tim_output_compare_init(TIM_t *timx, std_tim_output_compare_init_t *output_config, uint32_t channel_id)
{   
    uint32_t channel_oisx = (channel_id << 1);
    uint32_t channel_ccxe = (channel_id << 2);
    
    /* 禁止CCxE和CCxNE位 */
    timx->CCEN &= (~((TIM_CCEN_CC1E | TIM_CCEN_CC1NE) << channel_ccxe));
    
    /* 选择输出比较模式 */
    std_tim_set_ocmode(timx, channel_id, output_config->output_compare_mode);   
    
    /* 设置输出比较极性、输出使能位 */
    MODIFY_REG(timx->CCEN, 
              ((TIM_CCEN_CC1P | TIM_CCEN_CC1E) << channel_ccxe), 
              (output_config->output_pol << channel_ccxe) | (output_config->output_state << channel_ccxe));
    
    /* 配置比较匹配值 */
    std_tim_set_ccx_value(timx, channel_id, output_config->pulse);
        
    if (timx == TIM1)
    {
        /* 设置通道和互补输出通道的空闲状态 */
        MODIFY_REG(timx->CR2,
                  ((TIM_CR2_OIS1 | TIM_CR2_OIS1N) << channel_oisx),
                  ((output_config->output_idle_state | output_config->output_negtive_idle_state) << channel_oisx));
            
        /* 设置互补通道输出比较极性、输出使能位 */
        MODIFY_REG(timx->CCEN, 
                  ((TIM_CCEN_CC1NP | TIM_CCEN_CC1NE) << channel_ccxe), 
                  (output_config->output_negtive_pol << channel_ccxe) | (output_config->output_negtive_state << channel_ccxe));        
    }
}


/**
* @brief  设置std_tim_output_compare_init_t结构体为默认值
* @param  output_init_struct TIM输出结构体
* @retval 无
*/
void std_tim_output_compare_struct_init(std_tim_output_compare_init_t *output_init_struct)
{
    output_init_struct->output_compare_mode         = TIM_OUTPUT_MODE_FROZEN;
    output_init_struct->pulse                       = 0x0000U;
    output_init_struct->output_state                = TIM_OUTPUT_DISABLE;
    output_init_struct->output_negtive_state        = TIM_OUTPUT_NEGTIVE_DISABLE;
    output_init_struct->output_pol                  = TIM_OUTPUT_POL_HIGH;
    output_init_struct->output_negtive_pol          = TIM_OUTPUT_NEGTIVE_POL_HIGH;
    output_init_struct->output_idle_state           = TIM_OUTPUT_IDLE_RESET;
    output_init_struct->output_negtive_idle_state   = TIM_OUTPUT_NEGTIVE_IDLE_RESET;
}


/**
* @brief  断路参数初始化
* @param  timx TIM外设
* @param  bdt_init_param TIM断路参数结构体
* @retval 无
*/
void std_tim_bdt_init(TIM_t* timx, std_tim_break_init_t *bdt_init_param)
{   
    /* 配置断路基本参数 */
    MODIFY_REG(timx->BDT, 
              (TIM_BDT_DTG | TIM_BDT_OSSI | TIM_BDT_OSSR),
              (bdt_init_param->dead_time | bdt_init_param->off_state_idle_mode | bdt_init_param->off_state_run_mode));    
    
    /* 配置锁定级别，并使能断路 */
    MODIFY_REG(timx->BDT, 
              (TIM_BDT_BKEN | TIM_BDT_LOCK),
              (bdt_init_param->break_state | bdt_init_param->lock_level));   
}


/**
* @brief  设置std_tim_break_init_t结构体为默认值
* @param  bdt_init_struct TIM断路参数结构体
* @retval 无
*/
void std_tim_bdt_struct_init(std_tim_break_init_t *bdt_init_struct)
{
    bdt_init_struct->off_state_run_mode    = TIM_OSSR_DISABLE;
    bdt_init_struct->off_state_idle_mode   = TIM_OSSI_DISABLE;
    bdt_init_struct->lock_level            = TIM_LOCK_LEVEL_OFF;
    bdt_init_struct->dead_time             = 0x00U;
    bdt_init_struct->break_state           = TIM_BREAK_DISABLE;
}



/** 
* @} 
*/

#endif /* STD_TIM_PERIPHERAL_USED */

/** 
* @} 
*/

/** 
* @} 
*/

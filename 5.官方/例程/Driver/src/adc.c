/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : adc.c
 * Description : adc driver source file
 * Author(s)   : Hackett
 * version     : V1.0
 * Modify date : 2021-12-21
 ***********************************************************************/
#include "adc.h"


void (*adcfunc)(void) = {NULL};

 /************************************************************************
 * function   	: ADC_IRQHandler
 * Description	: ADC_IRQHandler 
 * input 		: none
 * return		: none
 ************************************************************************/
void ADC_IRQHandler(void)	interrupt 6
{
	if((ADCCSTAT & 0x1) == 0x1)								//ADC接收器BUF存有数据
	{		
		if(adcfunc != NULL)		
		{		
			(*adcfunc)();  									//ADC的中断处理回调函数
		}		
	}
	ADCCSTAT = 0x1;											//RXAVLD写1清除	
}

 /************************************************************************
 * function   	: adc_clk_config
 * Description	: adc_clk_config 
 * input 		:	
 *				uint8_t clksource 		：ADC CLK SOURCE
 *				uint8_t vrefsource 		：ADC VREF SOURCE
 *				uint16_t clksource_div 	：fadc_clk = fpclk / clkdiv，clkdiv = {clkdiv1, clkdiv0}
 *				uint8_t newstate 		：ADC ENABLE/DISABLE
 * return		: none
 ************************************************************************/
void adc_clk_config(uint8_t clksource, uint8_t vrefsource, uint16_t	clksource_div, uint8_t newstate)
{
	if(newstate == ADC_ENABLE)
	{
		PCLK0 |= (1<<4);									//ADC时钟使能
		PRESET0 |= (1<<4);									//ADC复位释放
		
		ADCGCR0 = (ADCGCR0 & (~(1<<6))) | (clksource<<6);	//ADC时钟源选择
		
		ADCCDR0 = 0;										//清零CLKDIV
		ADCCDR1 = 0;
		ADCCDR0 = (0x00ff & clksource_div);					//ADC内部时钟分频倍数
		ADCCDR1 = (0xff00 & clksource_div)>>8;		
		
		ADCVREF = (ADCVREF & (~(1<<0)))|(vrefsource<<0);	//电压参考源选择
	}
	else
	{
		PCLK0 &= ~(1<<4);									//ADC时钟关闭
		PRESET0 &= ~(1<<4);									//ADC复位
	}
}

 /************************************************************************
 * function   	: adc_sample_clk_config
 * Description	: adc_sample_clk_config 
 * input 		:	
 *				uint8_t sampclk	：ADC SAMPLE CLK
 * return		: none
 ************************************************************************/
void adc_sample_clk_config(uint8_t sampclk)
{
	ADCSPW &= ~(0x07);
	switch(sampclk)											//采样时钟脉冲宽度配置
	{
		case ADC_SAMPCLK_4:
			ADCSPW |= (ADC_SAMPCLK_4<<0);
			break;
		case ADC_SAMPCLK_5:
			ADCSPW |= (ADC_SAMPCLK_5<<0);			
			break;
		case ADC_SAMPCLK_6:
			ADCSPW |= (ADC_SAMPCLK_6<<0);			
			break;
		default:
			break;		
	}
}

 /************************************************************************
 * function   	: adc_io_config
 * Description	: adc_io_config 
 * input 		:	
 *				uint8_t ch	：ADC Channel
 * return		: none
 ************************************************************************/
void adc_io_config(uint8_t ch)
{
	if(ch & 0x80)
	{
		ch &= ~(1<<8);
	}

	ADCHL = ch;												//配置为ADC输入
	ADCGCR2 &= ~(1<<4);										//选定P1_4作为通道0的采样通道
//	ADCGCR2 |= (1<<4);										//选定P1_2作为通道0的采样通道
}

 /************************************************************************
 * function   	: adc_power_config
 * Description	: adc_power_config 
 * input 		:	
 *				uint8_t newstate 		：ADC ENABLE/DISABLE
 * return		: none
 ************************************************************************/
void adc_power_config(uint8_t newstate)
{
	if(newstate == ADC_ENABLE)
	{
		ADCGCR1 &= ~(1<<0);									//ADC上电
		ADCGCR1 &= ~(1<<1);									//ADC释放
	}
	else
	{
		ADCGCR1 |= (1<<1);									//ADC复位		
		ADCGCR1 |= (1<<0);									//ADC掉电
	}
}

 /************************************************************************
 * function   	: adc_scan_mode_config
 * Description	: adc_scan_mode_config 
 * input 		:	
 *				uint8_t mode ：ADC CONVERT MODE
 * return		: none
 ************************************************************************/
void adc_scan_mode_config(uint8_t mode)
{
//	ADCGCR0 &= ~(1<<5);										//禁止读取ADC数据寄存器后清除数据寄存器
	ADCGCR0 |= (1<<5);
	if(mode == ADC_MODE_SINGLE)
	{
		ADCGCR0 &= ~(1<<1);
	}
	else
	{
		ADCGCR0 |= (1<<1);
	}
	ADCGCR3 &= ~(1<<0);										//ADC数据在EOC的下降沿被采样(在本芯片设计中此位只能设置为0)
}

 /************************************************************************
 * function   	: adc_convert_start
 * Description	: adc_convert_start 
 * input 		:	
 *				uint8_t ch	：ADC Channel
 * return		: none
 ************************************************************************/
void adc_convert_start(uint8_t ch)
{
	switch(ch)
	{
		case ADC_CHANNEL_0:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x01);			
			break;
		case ADC_CHANNEL_1:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x02);			
			break;
		case ADC_CHANNEL_2:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x03);			
			break;
		case ADC_CHANNEL_3:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x04);			
			break;
		case ADC_CHANNEL_4:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x05);			
			break;
		case ADC_CHANNEL_5:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x06);			
			break;
		case ADC_CHANNEL_6:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x07);			
			break;
		case ADC_CHANNEL_7:
			ADCGCR2 = (ADCGCR2 & (~(0x0f))) | (0x08);			
			break;
		default:
			ADCGCR2 &= ~(0x0f);
			break;			
	}

	/*当信号具有从低到高的转换时，ADC转换开始*/
	ADCGCR1 &= ~(1<<2);
	ADCGCR1 |= (1<<2);
}

 /************************************************************************
 * function   	: adc_convert_stop
 * Description	: adc_convert_stop 
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_convert_stop(void)
{
	/*当信号具有从高到低的转换时，ADC转换操作完成*/
	ADCGCR1 |= (1<<2);	
	ADCGCR1 &= ~(1<<2);
//	ADCGCR0 &= ~(1<<1);
}

 /************************************************************************
 * function   	: adc_controller_config
 * Description	: adc_controller_config 
 * input 		:
 *				uint8_t newstate ：ADC ENABLE/DISABLE
 * return		: none
 ************************************************************************/
void adc_controller_config(uint8_t newstate)
{
	if(newstate == ADC_ENABLE)
	{
		ADCGCR0 |= (1<<0);	
	}
	else
	{
		ADCGCR0 &= ~(1<<0);		
	}
}

 /************************************************************************
 * function   	: adc_irq_config
 * Description	: adc_irq_config 
 * input 		:	
 *				void (*adc_irq_pro)()	：call back function
 *				uint8_t newstate 		：ADC ENABLE/DISABLE
 * return		: none
 ************************************************************************/
void adc_irq_config(void (*adc_irq_pro)(), uint8_t newstate)
{
	if(newstate == ADC_ENABLE)
	{
		adcfunc = adc_irq_pro;								//注册中断回调函数		
		
		ADCIER |= (1<<0);	
		IEN0 |= (1<<6);										//打开ADC中断
		IEN0 |= (1<<7);										//打开总中断
	}
	else
	{
		adcfunc = NULL;		
		
		ADCIER &= ~(1<<0);				
		IEN0 &= ~(1<<6);									//关闭ADC中断
		IEN0 &= ~(1<<7);									//关闭总中断	
	}
}

 /************************************************************************
 * function   	: adc_priority_level_config
 * Description	: adc_priority_level_config 
 * input 		:	
 *				uint8_t level：ADC PRIORITY LEVEL
 * return		: none
 ************************************************************************/
void adc_priority_level_config(uint8_t level)
{
	switch(level)
	{
		case ADC_PRIORITY_LEVEL0:
			IP0 &= ~(1<<0);
			IP1 &= ~(1<<0);
			break;
		case ADC_PRIORITY_LEVEL1:
			IP0 |= (1<<0);
			IP1 &= ~(1<<0);								
			break;
		case ADC_PRIORITY_LEVEL2:
			IP0 &= ~(1<<0);
			IP1 |= (1<<0);								
			break;
		case ADC_PRIORITY_LEVEL3:
			IP0 |= (1<<0);
			IP1 |= (1<<0);								
			break;
		default:
			IP0 &= ~(1<<0);
			IP1 &= ~(1<<0);								
			break;		
	}
}

 /************************************************************************
 * function   	: adc_wait_state
 * Description	: adc_wait_state 
 * input 		:	
 *				uint8_t state	 ：ADC STATE
 *				uint16_t timeout ：ADC TIME OUT
 * return		: 
 *				1：FAIL
 *				0：SUCCESS	
 ************************************************************************/
uint8_t	adc_wait_state(uint8_t state, uint16_t timeout)
{
	if(state == ADC_STATE_CONVERT)
	{
		while(timeout--)
		{
			if((ADCGCR1 & 0x04) == 0)						//转换完成，信号置0
			{
				return ADC_SUCCESS;
			}
		}	
	}
	else
	{
		while(timeout--)
		{
			if(ADCCSTAT & 0x01)								//接收器BUF存有数据，信号置1
			{
				return ADC_SUCCESS;
			}
		}	
	}

	return ADC_FAIL;
}

 /************************************************************************
 * function   	: adc_read_buf_state
 * Description	: adc_read_buf_state 
 * input 		: none
 * return		: none
 ************************************************************************/
//uint8_t adc_read_buf_state(void)
//{
//	return ADCCSTAT;
//}

 /************************************************************************
 * function   	: adc_clear_buf_state
 * Description	: adc_clear_buf_state 
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_clear_buf_state(void)
{
	ADCCSTAT = 0x1;											//RXAVLD写1清除	
}

 /************************************************************************
 * function   	: adc_get_value
 * Description	: adc_get_value 
 * input 		: none
 * return		: 12bit convert value of adc
 ************************************************************************/
uint16_t adc_get_value(void)
{
	/*做速率补偿*/
	uint16_t temp = 0;
	
	temp = ADCDR & 0x0FFF;
	
	if(temp > 1)
	{
		temp += 5;
	}
	if(temp > 4095)
	{
		temp = 4095;
	}
	
	return temp;								//12bit转换数据

}

 /************************************************************************
 * function   	: adc_get_ch_value
 * Description	: adc_get_ch_value 
 * input 		:	
 *				uint8_t ch	：ADC Channel
 * return		: 12bit convert value of adc
 ************************************************************************/
uint16_t adc_get_ch_value(uint8_t ch)
{
	uint16_t status = 0;
	adc_convert_start(ch);									//选择转换通道并启动转换
	
	status = adc_wait_state(ADC_STATE_CONVERT, ADC_TIMEOUT);//等待转换结束
	if(status == ADC_FAIL)
	{
		return ADC_INVALID;
	}
	
	status = adc_wait_state(ADC_STATE_BUF, ADC_TIMEOUT);	//等待接收器BUF存有数据
	if(status == ADC_FAIL)
	{
		return ADC_INVALID;
	}
	
	adc_clear_buf_state();									//清除BUF状态位
	
	return adc_get_value();
}




















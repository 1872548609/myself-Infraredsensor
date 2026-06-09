/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "common.h"
#include "uart0.h"
#include "adc.h"

uint8_t adc_irq_flag = 0;
uint8_t adc_count = 0;
uint16_t adc_data_buf[10];

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

/************************************************************************
 * Function   	: uart0_rec_pro
 * Description	: uart0_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart0_rec_pro(void)
{
	uart0_rx_buf[rx_count++]= uart0_recv_byte();
	rx_flag = 1;	
}

/************************************************************************
 * Function   	: uart_init
 * Description	: uart_init 串口初始化
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart_init(void)
{
	rx_flag = 0;
	uart0_init(UART0_BAUD_RATE);
  uart0_irq_init(UART0_IRQ_ENABLE,uart0_rec_pro);
}


 /************************************************************************
 * function   	: adc_pro
 * Description	: adc call back function of single mode
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_single_pro(void)
{
	adc_irq_flag = 1;															//置位adc_irq_flag标志
}

 /************************************************************************
 * function   	: adc_continuous_pro
 * Description	: adc call back function of continuous mode
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_continuous_pro(void)
{
	adc_data_buf[adc_count++] = adc_get_value();
	if(adc_count == 10)
	{
		adc_irq_flag = 1;														//置位adc_irq_flag标志	
		adc_controller_config(ADC_DISABLE);										//ADC控制器关闭
	}
}

 /************************************************************************
 * function   	: adc_single_mode_test
 * Description	: adc_single_mode_test 
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_single_mode_test(void)
{
	uint8_t i = 0, j = 0;
	
	uint16_t adc_data = 0;
	float vol_value = 0.0;

	/* ADC采样速率设置1Msps，ADC采样速率 = f(ADCCLK) / (采样时间+转换时间) = 16MHz/(4clk+12clk) = 1Msps */
	/*时钟源选择系统时钟发生器产生的时钟时，ADCCDR分频无效*/
	adc_clk_config(ADC_CLKSOURCE_DIVCLK, ADC_VREFSOURCE_AVDD33, 2, ADC_ENABLE);	//1.ADC时钟源选择 2.ADC电压基准源选择 3.ADC内部时钟分频倍数

	adc_sample_clk_config(ADC_SAMPCLK_4);										//采样时钟脉冲宽度配置	
	
	adc_io_config(ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | 
				  ADC_CHANNEL_3 | ADC_CHANNEL_4 | ADC_CHANNEL_7);				//配置相应IO为ADC输入
	adc_scan_mode_config(ADC_MODE_SINGLE);										//ADC工作模式
	
	adc_power_config(ADC_ENABLE);												//ADC上电
	adc_controller_config(ADC_ENABLE);											//ADC控制器使能

/********************中断方式********************/
#if ADC_IRQ_MODE == ADC_IRQ_ENABLE	
	adc_priority_level_config(ADC_PRIORITY_LEVEL0);								//ADC中断优先级
	adc_irq_config(adc_single_pro, ADC_ENABLE);									//使能ADC中断，注册中断回调函数
	
	delay1ms(1000);
	printfS("single irq adc! \n");	
	
	adc_convert_start(ADC_CHANNEL_1);											//启动转换
	
	while(1)
	{
		if(adc_irq_flag == 1)													//等待中断置位adc_irq_flag标志
		{
			adc_irq_flag = 0;													//清零adc_irq_flag标志
			
			adc_data = adc_get_value();											//读取数据
			vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
			printfS("ADC irq data : %d\r\n", adc_data);
			printfS("vol : %0.3fV\r\n", vol_value);			
			printfS("\r\n");
			delay1ms(1000);
			adc_convert_start(ADC_CHANNEL_1);									//启动转换
		}
	}

/********************查询方式********************/	
#else
	
	printfS("single adc! \n");	
	adc_priority_level_config(ADC_PRIORITY_LEVEL0);					//ADC中断优先级
	adc_irq_config(NULL, ADC_DISABLE);											//不使能ADC中断

	while(1)
	{
		/*单通道*/
		adc_data = adc_get_ch_value(ADC_CHANNEL_1);								//启动转换，等待转换完成，读取数据
		if(adc_data == ADC_INVALID)												//数据无效
		{
			printfS("数据无效\r\n");
		}
		else																	//数据有效
		{
			vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
			printfS("ADC data : %d\r\n", adc_data);
			printfS("vol : %0.3fV\r\n", vol_value);	
		}
		printfS("\r\n");
		delay1ms(1000);
		
		/*多通道*/
//		for(i = ADC_CHANNEL_0, j = 0;i <= ADC_CHANNEL_3; i = i<<1)
//		{
//			adc_data = adc_get_ch_value(i);										//启动转换，等待转换完成，读取数据
//			if(adc_data == ADC_INVALID)											//数据无效
//			{
//				printfS("--------通道%bd--------\r\n", j);
//				printfS("数据无效\r\n");
//			}
//			else																//数据有效
//			{
//				vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
//				printfS("--------通道%bd--------\r\n", j);
//				printfS("ADC data : %d\r\n", adc_data);
//				printfS("vol : %0.3fV\r\n", vol_value);	
//			}
//			j++;
//			adc_convert_stop();
//		}
//		printfS("\r\n");
//		delay1ms(1000);
	}		
#endif
	
}

 /************************************************************************
 * function   	: adc_continuous_mode_test
 * Description	: adc_continuous_mode_test 
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_continuous_mode_test(void)
{
	uint16_t adc_data = 0;
	float vol_value = 0.0;
	
	adc_clk_config(ADC_CLKSOURCE_DIVCLK, ADC_VREFSOURCE_AVDD33, 4, ADC_ENABLE);	//1.ADC时钟源选择 2.ADC电压基准源选择 3.ADC内部时钟分频倍数

	adc_sample_clk_config(ADC_SAMPCLK_4);										//采样时钟脉冲宽度配置
	
	adc_io_config(ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3);								//配置相应IO为ADC输入
	
	adc_scan_mode_config(ADC_MODE_CONTINUOUS);									//ADC工作模式
	
	adc_power_config(ADC_ENABLE);												//ADC上电
	adc_controller_config(ADC_ENABLE);											//ADC控制器使能
	
/********************中断方式********************/	
#if ADC_IRQ_MODE == ADC_IRQ_ENABLE
	
	printfS("continuous irq adc! \n");		
	adc_priority_level_config(ADC_PRIORITY_LEVEL0);								//ADC中断优先级
	adc_irq_config(adc_continuous_pro, ADC_ENABLE);								//不使能ADC中断
	
	adc_convert_start(ADC_CHANNEL_1);											//启动转换	
	
	while(1)
	{
		if(adc_irq_flag == 1)													//等待中断置位adc_irq_flag标志
		{
			while(adc_count)													//循环打印10次的转换值
			{
				vol_value = (float)(adc_data_buf[10 - adc_count] * ADC_VREF_VDDA)/4095;
				printfS("ADC irq data : %d\r\n", adc_data_buf[10 - adc_count]);
				printfS("vol : %0.3fV\r\n", vol_value);	
				adc_data_buf[10 - adc_count] = 0;			
				adc_count--;
			}
			printfS("\r\n");	
			adc_irq_flag = 0;													//清零adc_irq_flag标志
			delay1ms(1000);
			adc_controller_config(ADC_ENABLE);									//ADC控制器关闭	
			adc_convert_start(ADC_CHANNEL_1);									//启动转换	
		}
	}

/********************查询方式********************/		
#else
	
	printfS("continuous adc! \n");	
	adc_priority_level_config(ADC_PRIORITY_LEVEL0);								//ADC中断优先级
	adc_irq_config(NULL, ADC_DISABLE);											//不使能ADC中断
	
	adc_convert_start(ADC_CHANNEL_2);											//启动转换

	while(1)
	{
//		/*开启转换后不停*/
//		adc_data = adc_get_value();												//读取数据
//		vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
//		printfS("ADC data : %d\r\n", adc_data);
//		printfS("vol : %0.3fV\r\n", vol_value);		
//		printfS("\r\n");
//		delay1ms(1000);
		
		/*开启转换后停止*/
		adc_data = adc_get_value();												//读取数据
		printfS("ADCDR: %d\r\n", ADCDR & 0x0FFF);
		vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
		printfS("ADC data : %d\r\n", adc_data);
		printfS("vol : %0.3fV\r\n", vol_value);	
		printfS("\r\n");
		adc_controller_config(ADC_DISABLE);										//ADC控制器关闭		
		delay1ms(1000);		
		adc_controller_config(ADC_ENABLE);										//ADC控制器打开
		adc_convert_start(ADC_CHANNEL_2);										//启动转换		
	}

		
//		/*开启转换后停止*/
//		adc_data = adc_get_value();												//读取数据
//		printfS("ADCDR: %d\r\n", ADCDR & 0x0FFF);
//		vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
//		printfS("ADC data : %d\r\n", adc_data);
//		printfS("vol : %0.3fV\r\n", vol_value);	
//		printfS("\r\n");
//		adc_controller_config(ADC_DISABLE);										//ADC控制器关闭
//		delay1ms(1000);	
//		
//	adc_controller_config(ADC_ENABLE);										//ADC控制器关闭	
//	adc_convert_start(ADC_CHANNEL_2);											//启动转换
//	while(1)
//	{
//		adc_data = adc_get_value();												//读取数据
//		printfS("ADCDR: %d\r\n", ADCDR & 0x0FFF);
//		vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
//		printfS("ADC data : %d\r\n", adc_data);
//		printfS("vol : %0.3fV\r\n", vol_value);	
//		printfS("\r\n");
//		delay1ms(1000);			
//	}

#endif
	
}

 /************************************************************************
 * function   	: adc_performance_test
 * Description	: adc_performance_test 
 * input 		: none
 * return		: none
 ************************************************************************/
void adc_performance_test(void)
{
	uint8_t times = 1;
	int len = 2;
	uint8_t count = 1;		
	uint16_t adc_data;
//	float vol_value;
	
	printfS("Start single scan of single channel test \r\n");
	/* ADC采样速率设置1Msps，ADC采样速率 = f(ADCCLK) / (采样时间+转换时间) = 16MHz/(4clk+12clk) = 1Msps */
	/*时钟源选择系统时钟发生器产生的时钟时，ADCCDR分频无效*/
	adc_clk_config(ADC_CLKSOURCE_SYSCLK, ADC_VREFSOURCE_AVDD33, 8, ADC_ENABLE);	//1.ADC时钟源选择 2.ADC电压基准源选择 3.ADC内部时钟分频倍数

	adc_sample_clk_config(ADC_SAMPCLK_4);										//采样时钟脉冲宽度配置
	
	adc_io_config(ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | 
				  ADC_CHANNEL_3 | ADC_CHANNEL_4 | ADC_CHANNEL_7);												//配置相应IO为ADC输入
	adc_scan_mode_config(ADC_MODE_SINGLE);										//ADC工作模式
	
	adc_power_config(ADC_ENABLE);												//ADC上电
	adc_controller_config(ADC_ENABLE);											//ADC控制器使能

//	adc_priority_level_config(ADC_PRIORITY_LEVEL0);								//ADC中断优先级
//	adc_irq_config(NULL, ADC_DISABLE);											//不使能ADC中断

	printfS("串口发送\"start\"开始转换。\r\n");	

	while(1)
	{
		if(rx_flag)
		{
			delay1ms(50); 
			printfS(">>\r\n", times);
			if(strncmp((char *)uart0_rx_buf,"start", sizeof(5)) == 0)
			{
				printfS("----------转换开始%d----------\r\n", times);
				times++;
				if(times > 200)
				{
					times = 1;
				}
				
				count = 100;
				while(count--)
				{			
					adc_data = adc_get_ch_value(ADC_CHANNEL_1);					//启动转换，等待转换完成，读取数据
//					vol_value = (float)(adc_data * ADC_VREF_VDDA)/4095;
//					printfS("ADC data : %d\r\n", adc_data);
//					printfS("vol : %0.3fV\r\n", vol_value);	
					printfS("%d\n", adc_data);
//					delay1ms(100);					
				}
				printfS("\r\n\r\n");
			}	
			else
			{
				printfS("不匹配\r\n");
			}
	
			rx_count = 0;   		
			rx_flag = 0;
			for(len = 0; len < 32; len++)
			{
				uart0_rx_buf[len] = 0;
			}			
			   
		}
		
	}
}

/************************************************************************
 * Function   	: soc_test
 * Description	: soc_test
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void soc_test(void)
{
	printfS("System run at %ldHz\n\r", system_core_clock);
	printfS("start test adc! \n");

	
	adc_single_mode_test();
//	adc_continuous_mode_test();
	
//	adc_performance_test();
}



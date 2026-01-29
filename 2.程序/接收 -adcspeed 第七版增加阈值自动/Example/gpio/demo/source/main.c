/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : main.c
 * Description : main source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/     
#include "system_um800y.h"
#include "app.h"
#include "gtimer.h"
#include "pwm.h"
#include "common.h"
#include "config.h"
#include "gpio.h"
#include "adc.h"

#define impulse 	P1_4
#define impulsetGPIO_HIGH(); 	gpio_io_set(P1_4, GPIO_LOW);
#define impulsetGPIO_LOW(); 	gpio_io_set(P1_4, GPIO_HIGH);

volatile uint16_t duty2 = 8000;	
volatile uint16_t cycle2 = 64000;	
uint16_t ADC_Flag = 0;
volatile uint16_t adc_data = 0;
volatile uint16_t adc_data1 = 0;
volatile uint16_t adc_set = 100;
uint16_t adc_max_set = 400;
uint16_t adc_min_set = 0;

uint32_t adc_sum = 0;

uint16_t adc_datap= 0;
uint16_t adc_datan = 0;

float vol_value = 0.0;

uint16_t status = 0;
uint16_t adcount =0;
uint16_t adcount1 =0;

bit adget = 0;
bit state = 0;

uint16_t adc_value = 0;
uint8_t exitintcount = 0;

uint8_t adflag=0;

volatile int get_Delay = 0;
char adstate=0;




void gtimer0_UECallBack(void);
void gtimer1_UECallBack(void);

void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);

void main(void)  
{  		
	system_init();
	GPIO_Init();
	uart_init();
	
	
	gtimer0_count_init(3000,16-1);		//gtimer0_count_init(16000,1000-1);		
		
	gtimer0_irq_init(GTIMER_IRQ_ENABLE,GTIMER0_UIE_IRQ,gtimer0_UECallBack);

	gtimer0_start();	
	
	adc_clk_config(ADC_CLKSOURCE_SYSCLK, ADC_VREFSOURCE_AVDD33, 4, ADC_ENABLE);	

	adc_sample_clk_config(ADC_SAMPCLK_4);										
	
	adc_io_config(ADC_CHANNEL_1|ADC_CHANNEL_2);								
	
	adc_scan_mode_config(ADC_MODE_SINGLE);									
	
	adc_power_config(ADC_ENABLE);												
	adc_controller_config(ADC_ENABLE);
	
	
										
		
	while(1) 
	{
		

		
	}
}



void gtimer0_UECallBack(void)
{
	gpio_io_set(P1_2, GPIO_LOW);
	//gpio_io_set(P1_0, GPIO_LOW);
	gpio_io_set(P1_3, GPIO_LOW);
	adstate=0;
}


void GPIO_IRQHandler(void) interrupt 0
{
		if(gpio_irq_get(P1_4))											
		{
            gpio_io_set(P1_0, GPIO_HIGH);//常闭  
            
                adc_convert_start(ADC_CHANNEL_1);									

				while((ADCGCR1 & 0x04) != 0);
				
				while(!(ADCCSTAT & 0x01))	;
				
				ADCCSTAT = 0x1;											
				
				adc_data=adc_get_value();	
            
            
            
            
				REG_GTIM0_CR0 &= ~(1<<0);				
			
				adc_convert_start(ADC_CHANNEL_1);								
				
				while((ADCGCR1 & 0x04) != 0);
				
				while(!(ADCCSTAT & 0x01))	;
				
				ADCCSTAT = 0x1;										
				
				adc_data1=adc_get_value();	
			
				//printfS("adj:%d\r\n", adc_data1);	
			
				if(adc_data1 >=3500&&adc_data1<4095)											
				{
							
							adc_set=2500;
				} 
				else if(adc_data1 >=3200&&adc_data1<3500)											
				{
						
							adc_set=2000;
				} 
				else if(adc_data1 >=3000&&adc_data1<3200)											
				{
						
							adc_set=1800;
				} 
				else if(adc_data1 >=2700&&adc_data1<3000)											
				{
							
							adc_set=1600;
				} 
				else if(adc_data1 >=2500&&adc_data1<2700)											
				{
						
							adc_set=1400;
				} 
				else if(adc_data1 >=2200&&adc_data1<2500)											
				{
						
							adc_set=1200;
				} 
				else if(adc_data1 >=2000&&adc_data1<2200)											
				{
				
							adc_set=1000;
					
				} 				
				
				//printfS("adc:%d\r\n", adc_data);	

			if(adc_data >=adc_set&&adc_data<ADC_INVALID-1)													
			{
				//printfS("on\r\n");	
				
				switch(adstate)
				{
					case 0:{
							adcount++;
							if(adcount==2)
							{
								
								REG_GTIM0_CNT0&=0x00;
								REG_GTIM0_CNT1&=0x00;
								
								
								gpio_io_set(P1_2, GPIO_HIGH);
								//gpio_io_set(P1_0, GPIO_HIGH);
								gpio_io_set(P1_3, GPIO_HIGH);
								adstate=1;
								adcount=0;
							}
					}break;
					case 1:{
								REG_GTIM0_CNT0&=0x00;
								REG_GTIM0_CNT1&=0x00;
					}break;
				}
				
			}	
		}		
        
         gpio_io_set(P1_0, GPIO_LOW);//常闭
        
		REG_GTIM0_CR0 |= (1<<0);				
		gpio_irq_clr(P1_4);
}

void gpio_UECallBack(void)
{
	//= 不用二次回调
}

void GPIO_Init(void)
{

	REG_P10_CFG=0x00;
	gpio_init(P1_0);
	gpio_dir_set(P1_0, GPIO_DIR_OUT);
	gpio_dr_set(P1_0, GPIO_SR_HIGH);
	gpio_io_set(P1_0, GPIO_HIGH);
	

	REG_P12_CFG=0x00;
	gpio_init(P1_2);
	gpio_dir_set(P1_2, GPIO_DIR_OUT);
	gpio_dr_set(P1_2, GPIO_SR_HIGH);
	gpio_io_set(P1_2, GPIO_HIGH);
	

	//= p13做io
//	REG_P13_CFG=0x00;
//	gpio_init(P1_3);
//	gpio_dir_set(P1_3, GPIO_DIR_OUT);
//	gpio_dr_set(P1_3, GPIO_SR_HIGH);
//	gpio_io_set(P1_3, GPIO_HIGH);
	
	//= p13做输入
	gpio_init(P1_3);
	gpio_dir_set(P1_3, GPIO_DIR_IN);
	gpio_dr_set(P1_3, GPIO_SR_HIGH);
	gpio_in_enable(P1_3, IN_ENABLE);         
//	gpio_irq_set(P1_3,GPIO_IRQ_ENABLE,gpio_UECallBack);
//	P1AL|=(0x01<<7);
//	P1AL&=~(0x01<<6);
	
	

	gpio_init(P1_4);
	gpio_dir_set(P1_4, GPIO_DIR_IN);
	gpio_dr_set(P1_4, GPIO_SR_HIGH);
	gpio_in_enable(P1_4, IN_ENABLE);         
	gpio_irq_set(P1_4,GPIO_IRQ_ENABLE,gpio_UECallBack);
	P1AH&=~(0x02);
	P1AH|=(0x01);
	REG_P14_CFG=0x20;

	REG_P15_CFG=0x00;
	gpio_init(P1_5);                        
	gpio_dir_set(P1_5, GPIO_DIR_IN);          
	gpio_in_enable(P1_5, IN_ENABLE);          
}
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


volatile uint16_t adc_data = 0;
volatile uint16_t adc_data1 = 0;
volatile uint16_t adc_set = 200;

uint16_t status = 0;
uint16_t adcount =0;
uint16_t adcount1 =0;

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
	
	
	gtimer0_count_init(2200,24-1);			
		
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
	gpio_io_set(P1_3, GPIO_LOW);
    gpio_io_set(P1_0, GPIO_HIGH);//常闭 
}


void GPIO_IRQHandler(void) interrupt 0
{
    if(gpio_irq_get(P1_4))											
    {
        REG_GTIM0_CR0 &= ~(1<<0);	
        
        //gpio_io_set(P1_0, GPIO_HIGH);//常闭  
        
        adc_convert_start(ADC_CHANNEL_1);									

        while((ADCGCR1 & 0x04) != 0);
        
        while(!(ADCCSTAT & 0x01))	;
        
        ADCCSTAT = 0x1;											
        
        adc_data=adc_get_value();	
        
       //printfS("adc:%d\r\n", adc_data);	
       // if(adc_data >=adc_set&&adc_data<ADC_INVALID-1)
        if(1)
        {
            //printfS("on\r\n");
                     
            REG_GTIM0_CNT0&=0x00;
            REG_GTIM0_CNT1&=0x00;
                
            adcount++;
            if(adcount==3)
            {
                gpio_io_set(P1_2, GPIO_HIGH);
                gpio_io_set(P1_3, GPIO_HIGH);
                gpio_io_set(P1_0, GPIO_LOW);
                
                adcount=0;
            }
            
        }
    }
           
     //gpio_io_set(P1_0, GPIO_LOW);//常闭
    
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
	

	REG_P13_CFG=0x00;
	gpio_init(P1_3);
	gpio_dir_set(P1_3, GPIO_DIR_OUT);
	gpio_dr_set(P1_3, GPIO_SR_HIGH);
	gpio_io_set(P1_3, GPIO_LOW);
    
	
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
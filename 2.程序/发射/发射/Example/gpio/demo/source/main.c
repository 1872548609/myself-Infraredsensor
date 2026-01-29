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

volatile uint16_t duty2 = 8000;	//占空比 = duty/cycle
volatile uint16_t cycle2 = 64000;	//pwm频率=时钟频率/cycle
uint16_t ADC_Flag = 0;
uint16_t adc_data = 0;
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

void gtimer0_UECallBack(void);

void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);

void main(void)  
{  		
	system_init();
	GPIO_Init();
	uart_init();
	
	pwm2_init(16000,800,LOW);//1khz 12.5us dt0.0125
	pwm2_start();
    
	while(1) 
	{

	}
}


void GPIO_Init(void)
{
	//红色指示灯
	gpio_init(P1_3);
	gpio_dir_set(P1_3, GPIO_DIR_OUT);
	gpio_dr_set(P1_3, GPIO_SR_HIGH);
	gpio_io_set(P1_3, GPIO_HIGH);
	
	//脉冲发射口
	REG_P14_CFG=0x20;
	gpio_sr_set(P1_4,GPIO_SR_HIGH);
	
	//脉冲接收口
	REG_P15_CFG=0x00;
	gpio_init(P1_5);                          //时钟初始化，模块正常工作
	gpio_dir_set(P1_5, GPIO_DIR_IN);          //配置为输入
	gpio_in_enable(P1_5, IN_ENABLE);          //输入使能 
}
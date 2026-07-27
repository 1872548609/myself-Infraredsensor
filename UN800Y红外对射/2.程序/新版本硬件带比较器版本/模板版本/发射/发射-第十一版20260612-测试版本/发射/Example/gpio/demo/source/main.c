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

void GPIO_Init(void);
void ADC_Init(void);
void gpio_int_pro(void);
void gtimer0_UECallBack(void);

void main(void)  
{  		
	system_init(); // 24Mhz
	GPIO_Init();
	uart_init();
	
	pwm2_init(24000,1200,LOW);
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
	// 脉冲发射口：P1.4 复用为 PWM2
    REG_P14_CFG = 0x02;
    gpio_sr_set(P1_4, GPIO_SR_HIGH);
	
	//脉冲接收口
	REG_P15_CFG=0x00;
	gpio_init(P1_5);                          //时钟初始化，模块正常工作
	gpio_dir_set(P1_5, GPIO_DIR_IN);          //配置为输入
	gpio_in_enable(P1_5, IN_ENABLE);          //输入使能 
}
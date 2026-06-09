/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : adc.h
 * Description : adc driver header file
 * Author(s)   : Hackett
 * version     : V1.0
 * Modify date : 2021-12-21
 ***********************************************************************/
#ifndef __ADC_H__
#define __ADC_H__

#include "um800y.h"

#define ADC_IRQ_ENABLE						1
#define ADC_IRQ_DISABLE						0
#define ADC_IRQ_MODE						ADC_IRQ_DISABLE

#define	ADC_SUCCESS							0
#define ADC_FAIL							1
#define ADC_INVALID							0x8000


/****************ADC Channel****************/
#define	ADC_CHANNEL_0						(uint8_t)(1<<0)				//P1_4
#define	ADC_CHANNEL_1						(uint8_t)(1<<1)				//P1_5
#define	ADC_CHANNEL_2						(uint8_t)(1<<2)				//P2_0
#define	ADC_CHANNEL_3						(uint8_t)(1<<3)				//P2_2
#define	ADC_CHANNEL_4						(uint8_t)(1<<4)				//P2_3
#define	ADC_CHANNEL_5						(uint8_t)(1<<5)				//P2_6
#define	ADC_CHANNEL_6						(uint8_t)(1<<6)				//P2_7
#define	ADC_CHANNEL_7						(uint8_t)(1<<7)				//内部LDO

/****************ADC ENABLE/DISABLE****************/
#define	ADC_ENABLE							1
#define	ADC_DISABLE							0

/****************ADC CONVERT MODE****************/
#define	ADC_MODE_SINGLE						0
#define	ADC_MODE_CONTINUOUS					1

/****************ADC CLK SOURCE****************/
#define	ADC_CLKSOURCE_DIVCLK				0
#define	ADC_CLKSOURCE_SYSCLK				1

/****************ADC VREF SOURCE****************/
#define	ADC_VREFSOURCE_AVDD33					0
#define	ADC_VREFSOURCE_EXTERVREF				1

/****************ADC SAMPLE CLK****************/
#define	ADC_SAMPCLK_4						3
#define	ADC_SAMPCLK_5						4
#define	ADC_SAMPCLK_6						5

/****************ADC PRIORITY LEVEL****************/
//Level0(lowest)
//Level1
//Level2
//Level3(highest)
#define	ADC_PRIORITY_LEVEL0					0	
#define	ADC_PRIORITY_LEVEL1					1
#define	ADC_PRIORITY_LEVEL2					2
#define	ADC_PRIORITY_LEVEL3					3

/****************ADC STATE****************/
#define	ADC_STATE_CONVERT					0
#define	ADC_STATE_BUF						1

/****************ADC TIME OUT****************/
#define	ADC_TIMEOUT							60000



#define ADC_VREF_VDDA						3.315
#define ADC_VREF_EXIT						2.500


void adc_clk_config(uint8_t clksource, uint8_t vrefsource, uint16_t	clksource_div, uint8_t newstate);

void adc_sample_clk_config(uint8_t sampclk);

void adc_io_config(uint8_t ch);

void adc_power_config(uint8_t newstate);

void adc_scan_mode_config(uint8_t mode);

void adc_convert_start(uint8_t ch);

void adc_convert_stop(void);

void adc_controller_config(uint8_t newstate);

void adc_irq_config(void (*adc_irq_pro)(), uint8_t newstate);

void adc_priority_level_config(uint8_t level);

uint16_t adc_get_value(void);

uint16_t adc_get_ch_value(uint8_t ch);

uint8_t	adc_wait_state(uint8_t state, uint16_t timeout);

//uint8_t	adc_read_buf_state(void);

void adc_clear_buf_state(void);

#endif


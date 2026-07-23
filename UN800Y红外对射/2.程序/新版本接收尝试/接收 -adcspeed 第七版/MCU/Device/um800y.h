/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : um800y.h
 * Description : chip header file (registers and interrupt source)
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-29
 ***********************************************************************/
#ifndef __UM800Y_H__
#define __UM800Y_H__

#undef NULL
#define NULL 0                   /* see <stddef.h> */

#include "types.h"

#define Sfr(x, y)	sfr x = y
#define Sbit(x, y, z)   sbit x = y ^ z
#define Sfr16(x,y)	sfr16 x = y


///* ================================================================================ */
///* ==========================       C51 CORE Section       ======================== */
///* ================================================================================ */
/*----------------------- SYSTEM CONFIGURATION -----------------------------*/

/*------------------------- SP --------------------------------*/
Sfr (SP   , 0x81);

/*------------------------- DPL -------------------------------*/
Sfr (DPL  , 0x82);
Sfr (DPL1 , 0x84);

/*------------------------- DPH -------------------------------*/
Sfr (DPH  , 0x83);
Sfr (DPH1 , 0x85);

/*------------------------- BEEPCTR ---------------------------*/
Sfr (BEEPCTR  , 0x86);

/*------------------------- PCON ------------------------------*/
Sfr (PCON  , 0x87);

/*------------------------- PDSEL ------------------------------*/
Sfr (PDSEL  , 0x8E);

/*------------------------- POREN -----------------------------*/
Sfr (POREN , 0x8F);

/*------------------------- LDOTRIM ---------------------------*/
Sfr (LDOTRIML  , 0x91);
Sfr (LDOTRIMH  , 0xBD);

/*------------------------- DPS -------------------------------*/
Sfr (DPS  , 0x92);

/*-------------------------- RCHTRIM --------------------------*/
Sfr (RCHTRIML  , 0xBF);
Sfr (RCHTRIMH  , 0xBE);

/*-------------------------- EUARTEN --------------------------*/
Sfr (EUARTEN   , 0x9E);

/*-------------------------- RCLTRIM --------------------------*/
Sfr (RCLTRIMH   , 0x9F);
Sfr (RCLTRIML   , 0xBC);

/*------------------------- OUS -------------------------------*/
Sfr (OUS    , 0xA1);

/*------------------------- OINTEN ----------------------------*/
Sfr (OINTEN , 0xA2);

/*------------------------- OINTUS ----------------------------*/
Sfr (OINTUS , 0xA3);

/*-------------------------- OSEC -----------------------------*/
Sfr (OSEC   , 0xA5);

/*-------------------------- OSTATUS --------------------------*/
Sfr (OSTATUS , 0xA6);

/*-------------------------- OPSET ----------------------------*/
Sfr (OPSET   , 0xA7);

/*----------------------------IEN -----------------------------*/
Sfr (IEN0  , 0xA8);
Sbit (EA   , IEN0 , 7);
Sbit (EADC , IEN0 , 6);
Sbit (EPWM , IEN0 , 5);
Sbit (ES0  , IEN0 , 4);
Sbit (ET1  , IEN0 , 3);
Sbit (ES1  , IEN0 , 2);
Sbit (ET0  , IEN0 , 1);
Sbit (EX0  , IEN0 , 0);

Sfr (IEN1 , 0xB8);
Sbit (UART2INTEN  , IEN1 , 7);
Sbit (GTIM2INTEN  , IEN1 , 6);
Sbit (I2CINTEN    , IEN1 , 5);
Sbit (LPTIMINTEN  , IEN1 , 4);
Sbit (EFCINTEN    , IEN1 , 3);
Sbit (GTIM1INTEN  , IEN1 , 2);
Sbit (SPIINTEN    , IEN1 , 1);
Sbit (GTIM0INTEN  , IEN1 , 0);

Sfr (IEN2 , 0x9A);

/*---------------------------- IP -----------------------------*/
Sfr (IP0   , 0xA9);
Sfr (IP1   , 0xB9);

/*-------------------------- REMAP----------------------------*/
Sfr (REMAP , 0xAF);

/*-------------------------- CLKST ----------------------------*/
Sfr (CLKST , 0xB1);

/*-------------------------- ESTCR ----------------------------*/
Sfr (ESTCR , 0xB2);

/*-------------------------- XTHCTR ---------------------------*/
Sfr (XTHCTR , 0xB3);

/*---------------------------- PSW ----------------------------*/
Sfr (PSW , 0xD0);
Sbit (CY  , PSW , 7);
Sbit (AC  , PSW , 6);
Sbit (F0  , PSW , 5);
Sbit (RS1 , PSW , 4);
Sbit (RS0 , PSW , 3);
Sbit (F1  , PSW , 1);
Sbit (P   , PSW , 0);

/*--------------------------- OTP -----------------------------*/
Sfr16 (OADR , 0xD1);
Sfr (OADRL  , 0xD1);
Sfr (OADRH  , 0xD2);

Sfr (ODATA  , 0xD3);

Sfr (OCTRL  , 0xD4);

/*-------------------------- LVDCON ---------------------------*/
Sfr (LVDCON , 0xDD);

/*-------------------------- PCLK -----------------------------*/
Sfr (PCLK0  , 0xDE);
Sfr (PCLK1  , 0xDF);

/*---------------------------- ACC ----------------------------*/
Sfr (ACC   , 0xE0);
Sbit (ACC7  , ACC , 7);
Sbit (ACC6  , ACC , 6);
Sbit (ACC5  , ACC , 5);
Sbit (ACC4  , ACC , 4);
Sbit (ACC3  , ACC , 3);
Sbit (ACC2  , ACC , 2);
Sbit (ACC1  , ACC , 1);
Sbit (ACC0  , ACC , 0);

/*------------------------- PRESET ----------------------------*/
Sfr (PRESET0  , 0xE6);
Sfr (PRESET1  , 0xE7);

/*---------------------------- B ------------------------------*/
Sfr (B   , 0xF0);
Sbit (B7  , B , 7);
Sbit (B6  , B , 6);
Sbit (B5  , B , 5);
Sbit (B4  , B , 4);
Sbit (B3  , B , 3);
Sbit (B2  , B , 2);
Sbit (B1  , B , 1);
Sbit (B0  , B , 0);

/*-------------------------- ADC ------------------------------*/
Sfr (ADCGCR0   , 0xAC);
Sfr (ADCGCR1   , 0xAD);
Sfr (ADCGCR2   , 0xB4);
Sfr (ADCGCR3   , 0xB5);
Sfr (ADCDR0    , 0xB6);
Sfr (ADCDR1    , 0xB7);
Sfr16 (ADCDR   , 0xB6);
Sfr (ADCIER    , 0x93);
Sfr (ADCHL    , 0xF5);
Sfr (ADCCSTAT , 0xF6);
Sfr (ADCSPW   , 0xF7);
Sfr (ADCVREF  , 0xFD);
Sfr (ADCCDR0  , 0xFE);
Sfr (ADCCDR1  , 0xFF);

/*---------------------------- PORTS --------------------------*/
Sfr (P0  , 0x80);
Sbit (P05  , P0    , 5);
Sbit (P04  , P0    , 4);
Sbit (P03  , P0    , 3);
Sbit (P02  , P0    , 2);
Sbit (P01  , P0    , 1);
Sbit (P00  , P0    , 0);

Sfr (P1  , 0x90);			
Sbit (P15  , P1    , 5);
Sbit (P14  , P1    , 4);
Sbit (P13  , P1    , 3);
Sbit (P12  , P1    , 2);
Sbit (P11  , P1    , 1);
Sbit (P10  , P1    , 0);

Sfr (P2  , 0xA0);			
Sbit (P27  , P2    , 7);
Sbit (P26  , P2    , 6);
Sbit (P25  , P2    , 5);
Sbit (P23  , P2    , 3);
Sbit (P22  , P2    , 2);
Sbit (P20  , P2    , 0);

Sfr (P0IRQ  , 0xE1);
Sfr (P0IEN  , 0xE9);
Sfr (P0PU  , 0xF1);
Sfr (P0OEN  , 0xF9);
Sfr (P0DR   , 0x97);
Sfr (P0PD   , 0xD5);
Sfr (P0OD   , 0xD6);
Sfr (P0CS   , 0xD7);

Sfr (P1IRQ  , 0xE2);
Sfr (P1IEN  , 0xEA);
Sfr (P1PU  , 0xF2);
Sfr (P1OEN  , 0xFA);
Sfr (P1DR   , 0xE5);
Sfr (P1PD   , 0xDA);
Sfr (P1OD   , 0xDB);
Sfr (P1CS   , 0xDC);

Sfr (P2IRQ  , 0xE3);
Sfr (P2IEN  , 0xEB);
Sfr (P2PU  , 0xF3);
Sfr (P2OEN  , 0xFB);
Sfr (P2DR   , 0xFC);
Sfr (P2PD   , 0xE4);
Sfr (P2OD   , 0xEC);
Sfr (P2CS   , 0xF4);

/*---------------------------- PXAL -----------------------------*/
Sfr (P0AL , 0xA4);										
Sfr (P1AL , 0xAE);										
Sfr (P2AL , 0xC0);						
					
/*---------------------------- PXAH -----------------------------*/
Sfr (P0AH , 0xAB);
Sfr (P1AH , 0xB0);	
Sfr (P2AH , 0xE8);	

/*------------------------------- UART0 -------------------------------*/
Sfr (S0CON , 0x98);			//UART0 Interface Control
Sbit (SM0   , S0CON    , 7);
Sbit (SM1   , S0CON    , 6);
Sbit (SM20  , S0CON    , 5);
Sbit (REN0  , S0CON    , 4);
Sbit (TB80  , S0CON    , 3);
Sbit (RB80  , S0CON    , 2);
Sbit (TI0   , S0CON    , 1);
Sbit (RI0   , S0CON    , 0);

Sfr (S0BUF    , 0x99);			//UART0 Data Buffer

Sfr (S0RELL   , 0xAA);
Sfr (S0RELH   , 0xBA);

/*------------------------------- UART1 ---------------------------------*/
Sfr (S1CON  , 0x9B);			//UART1 Interface Control

Sfr (S1BUF  , 0x9C);			//UART1 Data Buffer

Sfr (S1RELL , 0x9D);
Sfr (S1RELH , 0xBB);

/*------------------------------- PWM0 ---------------------------------*/
Sfr (PWM0CON , 0xC8);
Sbit (PWM0EN  , PWM0CON  , 7);
Sbit (PWM0S   , PWM0CON  , 6);
Sbit (PWM0IE  , PWM0CON  , 2);
Sbit (PWM0IF  , PWM0CON  , 1);
Sbit (PWM0SS  , PWM0CON  , 0);

Sfr (PWM0DL   , 0xC3);
Sfr (PWM0DH   , 0xC4);

Sfr (PWM0PL   , 0xCB);
Sfr (PWM0PH   , 0xCC);

/*------------------------------- PWM1 ---------------------------------*/
Sfr (PWM1DL   , 0xC5);
Sfr (PWM1DH   , 0xC6);

Sfr (PWM1CON  , 0xC9);

Sfr (PWM1PL   , 0xCD);
Sfr (PWM1PH   , 0xCE);

/*------------------------------- PWM2 ---------------------------------*/
Sfr (PWM2DL   , 0xCF);
Sfr (PWM2DH   , 0xC7);

Sfr (PWM2CON  , 0xCA);

Sfr (PWM2PL   , 0xC1);
Sfr (PWM2PH   , 0xC2);

/*------------------------------- RSTSTAT -------------------------------*/
Sfr (RSTSTAT , 0xD8);
Sbit (WDOF    , RSTSTAT  , 7);
Sbit (WDEN    , RSTSTAT  , 6);
Sbit (LVRF    , RSTSTAT  , 5);
Sbit (PORF    , RSTSTAT  , 4);
Sbit (ERSTF   , RSTSTAT  , 3);
Sbit (WDT2    , RSTSTAT  , 2);
Sbit (WDT1    , RSTSTAT  , 1);
Sbit (WDT0    , RSTSTAT  , 0);


/*------------------------------ CLOCK ----------------------------------*/
Sfr (CLKCON    , 0xF8);
Sbit (RC32KF    , CLKCON  , 6);
Sbit (RC16MF    , CLKCON  , 5);
Sbit (XCLKF     , CLKCON  , 4);
Sbit (RC32KEN   , CLKCON  , 3);
Sbit (RC16MEN   , CLKCON  , 2);
Sbit (XCLKEN    , CLKCON  , 1);
Sbit (SYSSEL    , CLKCON  , 0);

Sfr (SYSDIV , 0xD9);
Sfr (RCHDIV , 0xEE);
///* ================================================================================ */
///* ================       Device Specific Peripheral Section       ================ */
///* ================================================================================ */
///*--------------------------IO-IE------------------------*/
#define REG_P0_IE							(*(volatile uint8_t xdata *)(0xC000))
#define REG_P1_IE							(*(volatile uint8_t xdata *)(0xC001))
#define REG_P2_IE							(*(volatile uint8_t xdata *)(0xC002))

///*--------------------------LVD------------------------*/
#define REG_LVD_LV							(*(volatile uint8_t xdata *)(0xC004))
	
///*--------------------------IO-SR------------------------*/
#define REG_P0_SR							(*(volatile uint8_t xdata *)(0xC005))
#define REG_P1_SR							(*(volatile uint8_t xdata *)(0xC006))
#define REG_P2_SR							(*(volatile uint8_t xdata *)(0xC007))
	
///*--------------------------IO-CFG------------------------*/
#define REG_P00_CFG							(*(volatile uint8_t xdata *)(0xC010))
#define REG_P01_CFG							(*(volatile uint8_t xdata *)(0xC011))
#define REG_P03_CFG							(*(volatile uint8_t xdata *)(0xC013))
#define REG_P04_CFG							(*(volatile uint8_t xdata *)(0xC014))
#define REG_P05_CFG							(*(volatile uint8_t xdata *)(0xC015))
#define REG_P10_CFG							(*(volatile uint8_t xdata *)(0xC018))	
#define REG_P11_CFG							(*(volatile uint8_t xdata *)(0xC019))
#define REG_P12_CFG							(*(volatile uint8_t xdata *)(0xC01A))	
#define REG_P13_CFG							(*(volatile uint8_t xdata *)(0xC01B))
#define REG_P14_CFG							(*(volatile uint8_t xdata *)(0xC01C))
#define REG_P15_CFG							(*(volatile uint8_t xdata *)(0xC01D))
#define REG_P20_CFG							(*(volatile uint8_t xdata *)(0xC020))
#define REG_P22_CFG							(*(volatile uint8_t xdata *)(0xC022))
#define REG_P23_CFG							(*(volatile uint8_t xdata *)(0xC023))
#define REG_P25_CFG							(*(volatile uint8_t xdata *)(0xC025))
#define REG_P26_CFG							(*(volatile uint8_t xdata *)(0xC026))
#define REG_P27_CFG							(*(volatile uint8_t xdata *)(0xC027))	


///*--------------------------I2C------------------------*/
#define I2C_BASE_ADDR 					0xCC00
#define REG_I2C_SLAVE_ADDR1				(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x00))
#define REG_I2C_CLK_DIV					(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x01))
#define REG_I2C_CR0						(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x02))
#define REG_I2C_CR1						(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x03))
#define REG_I2C_SR0						(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x04))
#define REG_I2C_SR1						(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x05))
#define REG_I2C_DR						(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x06))
#define REG_I2C_SLAVE_ADDR2				(*(volatile uint8_t xdata *)(I2C_BASE_ADDR + 0x07))
	
///*--------------------------SPI------------------------*/
#define SPI_BASE_ADDR 					0xC400
#define REG_SPI_SPCR1					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x00))
#define REG_SPI_SPCR2					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x01))
#define REG_SPI_SPCR3					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x02))
#define REG_SPI_SPCR4					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x03))
#define REG_SPI_SPIIE					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x04))
#define REG_SPI_SPSR					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x05))
#define REG_SPI_TXBUF					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x06))
#define REG_SPI_RXBUF					(*(volatile uint8_t xdata *)(SPI_BASE_ADDR + 0x07))

///*---------------------------LPTIMER------------------------*/
#define LPTIM_BASE_ADDR 				0xC800
#define REG_LPTIM_LPTCFG0				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x00))
#define REG_LPTIM_LPTCFG1				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x01))
#define REG_LPTIM_LPTCNTL				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x02))
#define REG_LPTIM_LPTCNTH				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x03))
#define REG_LPTIM_LPTCMPL				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x04))
#define REG_LPTIM_LPTCMPH				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x05))
#define REG_LPTIM_LPTTARGETL			(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x06))
#define REG_LPTIM_LPTTARGETH			(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x07))
#define REG_LPTIM_LPTIE					(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x08))
#define REG_LPTIM_LPTIF					(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x09))
#define REG_LPTIM_LPTCTRL				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x0A))
#define REG_LPTIM_CCMCFG1				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x0B))
#define REG_LPTIM_CCMCFG2				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x0C))
#define REG_LPTIM_LPTCMP2L				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x0D))
#define REG_LPTIM_LPTCMP2H				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x0E))
#define REG_LPTIM_LPTLOAD				(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x11))
#define REG_LPTIM_LPTBUFFERL			(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x12))
#define REG_LPTIM_LPTBUFFERH			(*(volatile uint8_t xdata *)(LPTIM_BASE_ADDR + 0x13))
	
///*---------------------------GTIMER0------------------------*/
#define GTIMER0_BASE_ADDR 				0xC900
#define REG_GTIM0_CR0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x00))					
#define REG_GTIM0_CR1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x01))					
#define REG_GTIM0_CR2					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x02))					
#define REG_GTIM0_CR3					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x03))					
#define REG_GTIM0_IER					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x04))					
#define REG_GTIM0_SR					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x05))					
#define REG_GTIM0_EGR					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x06))					
#define REG_GTIM0_CCMR0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x07))				
#define REG_GTIM0_CCMR1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x08))				
#define REG_GTIM0_CCER					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x09))				
#define REG_GTIM0_CNT0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x0A))				
#define REG_GTIM0_CNT1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x0B))				
#define REG_GTIM0_PSC0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x0C))				
#define REG_GTIM0_PSC1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x0D))				
#define REG_GTIM0_ARR0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x0E))				
#define REG_GTIM0_ARR1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x0F))				
#define REG_GTIM0_ARRN0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x10))				
#define REG_GTIM0_ARRN1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x11))				
#define REG_GTIM0_CCR0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x12))				
#define REG_GTIM0_CCR1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x13))				
#define REG_GTIM0_CCRN0					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x14))				
#define REG_GTIM0_CCRN1					(*(volatile uint8_t xdata *)(GTIMER0_BASE_ADDR + 0x15))				
	
///*---------------------------GTIMER1------------------------*/
#define GTIMER1_BASE_ADDR 				0xCA00
#define REG_GTIM1_CR0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x00))					
#define REG_GTIM1_CR1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x01))					
#define REG_GTIM1_CR2					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x02))					
#define REG_GTIM1_CR3					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x03))					
#define REG_GTIM1_IER					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x04))					
#define REG_GTIM1_SR					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x05))					
#define REG_GTIM1_EGR					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x06))					
#define REG_GTIM1_CCMR0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x07))				
#define REG_GTIM1_CCMR1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x08))				
#define REG_GTIM1_CCER					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x09))				
#define REG_GTIM1_CNT0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x0A))				
#define REG_GTIM1_CNT1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x0B))				
#define REG_GTIM1_PSC0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x0C))				
#define REG_GTIM1_PSC1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x0D))				
#define REG_GTIM1_ARR0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x0E))				
#define REG_GTIM1_ARR1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x0F))				
#define REG_GTIM1_ARRN0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x10))				
#define REG_GTIM1_ARRN1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x11))				
#define REG_GTIM1_CCR0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x12))				
#define REG_GTIM1_CCR1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x13))				
#define REG_GTIM1_CCRN0					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x14))				
#define REG_GTIM1_CCRN1					(*(volatile uint8_t xdata *)(GTIMER1_BASE_ADDR + 0x15))				
	
///*---------------------------GTIMER2------------------------*/
#define GTIMER2_BASE_ADDR 				0xCB00
#define REG_GTIM2_CR0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x00))					
#define REG_GTIM2_CR1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x01))					
#define REG_GTIM2_CR2					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x02))					
#define REG_GTIM2_CR3					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x03))					
#define REG_GTIM2_IER					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x04))					
#define REG_GTIM2_SR					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x05))					
#define REG_GTIM2_EGR					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x06))					
#define REG_GTIM2_CCMR0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x07))				
#define REG_GTIM2_CCMR1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x08))				
#define REG_GTIM2_CCER					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x09))				
#define REG_GTIM2_CNT0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x0A))				
#define REG_GTIM2_CNT1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x0B))				
#define REG_GTIM2_PSC0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x0C))				
#define REG_GTIM2_PSC1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x0D))				
#define REG_GTIM2_ARR0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x0E))				
#define REG_GTIM2_ARR1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x0F))				
#define REG_GTIM2_ARRN0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x10))				
#define REG_GTIM2_ARRN1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x11))				
#define REG_GTIM2_CCR0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x12))				
#define REG_GTIM2_CCR1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x13))				
#define REG_GTIM2_CCRN0					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x14))				
#define REG_GTIM2_CCRN1					(*(volatile uint8_t xdata *)(GTIMER2_BASE_ADDR + 0x15))				

///*---------------------------UART2------------------------*/
#define UART2_BASE_ADDR 				0xCD00
#define REG_UART2_ISR					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x00))
#define REG_UART2_IER					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x01))
#define REG_UART2_CR					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x02))
#define REG_UART2_TDR					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x03))
#define REG_UART2_RDR					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x03))
#define REG_UART2_BRPL					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x04))
#define REG_UART2_BRPH					(*(volatile uint8_t xdata *)(UART2_BASE_ADDR + 0x05))
	
///*---------------------------UART3------------------------*/
#define UART3_BASE_ADDR 				0xCE00
#define REG_UART3_ISR					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x00))
#define REG_UART3_IER					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x01))
#define REG_UART3_CR					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x02))
#define REG_UART3_TDR					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x03))
#define REG_UART3_RDR					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x03))
#define REG_UART3_BRPL					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x04))
#define REG_UART3_BRPH					(*(volatile uint8_t xdata *)(UART3_BASE_ADDR + 0x05))


///*---------------------------CAN------------------------*/
#define CAN_BASE_ADDR 					0xCF00
#define REG_CAN_MR						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x00))
#define REG_CAN_CMR						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x01))
#define REG_CAN_SR						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x02))
#define REG_CAN_ISR						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x03))		
#define REG_CAN_IMR						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x04))
#define REG_CAN_RMC						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x05))
#define REG_CAN_BTR0					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x06))
#define REG_CAN_BTR1					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x07))	
#define REG_CAN_TXBUF0					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x08))	
#define REG_CAN_TXBUF1					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x09))
#define REG_CAN_TXBUF2					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x0A))
#define REG_CAN_TXBUF3					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x0B))
#define REG_CAN_RXBUF0					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x0C))
#define REG_CAN_RXBUF1					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x0D))
#define REG_CAN_RXBUF2					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x0E))
#define REG_CAN_RXBUF3					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x0F))
#define REG_CAN_ACR0					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x10))
#define REG_CAN_ACR1					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x11))	
#define REG_CAN_ACR2					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x12))
#define REG_CAN_ACR3					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x13))
#define REG_CAN_AMR0					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x14))
#define REG_CAN_AMR1					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x15))	
#define REG_CAN_AMR2					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x16))
#define REG_CAN_AMR3					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x17))
#define REG_CAN_ECC						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x18))
#define REG_CAN_RXERR					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x19))
#define REG_CAN_TXERR					(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x1A))
#define REG_CAN_ALC						(*(volatile uint8_t xdata *)(CAN_BASE_ADDR + 0x1B))


///*---------------------------Sram trim------------------------*/
#define REG_SRAMICFG_TRIM				(*(volatile uint8_t xdata *)(0xC009))
#define REG_SRAMXCFG_TRIM				(*(volatile uint8_t xdata *)(0xC008))

	
///*---------------------------Flash trim------------------------*/
#define REG_TPROG0_TRIM					(*(volatile uint8_t xdata *)(0xC20D))
#define REG_TPROG1_TRIM					(*(volatile uint8_t xdata *)(0xC20E))
#define REG_TCHIP0_TRIM					(*(volatile uint8_t xdata *)(0xC20A))	
#define REG_TCHIP1_TRIM					(*(volatile uint8_t xdata *)(0xC20B))
#define REG_TCHIP2_TRIM					(*(volatile uint8_t xdata *)(0xC20C))
#define REG_TSERA0_TRIM					(*(volatile uint8_t xdata *)(0xC207))	
#define REG_TSERA1_TRIM					(*(volatile uint8_t xdata *)(0xC208))
#define REG_TSERA2_TRIM					(*(volatile uint8_t xdata *)(0xC209))
#define REG_HVSEL_TRIM					(*(volatile uint8_t xdata *)(0xC206))
#define REG_FT_TRIM						(*(volatile uint8_t xdata *)(0xC204))
#define REG_CT_TRIM						(*(volatile uint8_t xdata *)(0xC203))
#define REG_RT1_TRIM					(*(volatile uint8_t xdata *)(0xC201))
#define REG_RT2_TRIM					(*(volatile uint8_t xdata *)(0xC202))
#define REG_MT_TRIM						(*(volatile uint8_t xdata *)(0xC200))

	
#endif  /* UM800Y_H */

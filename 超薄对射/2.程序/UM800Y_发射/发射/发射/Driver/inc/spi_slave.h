/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : spi.h
 * Description : spi driver header file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2020-08-03
 ***********************************************************************/
#ifndef __SPI_H__
#define __SPI_H__

#include "um800y.h"

#define WORK_MODE_0  						0			//SPI0工作模式0:CPHOL = 0 CPHA = 0
#define WORK_MODE_1  						1			//SPI0工作模式1:CPHOL = 0 CPHA = 1
#define WORK_MODE_2  						2			//SPI0工作模式2:CPHOL = 1 CPHA = 0
#define WORK_MODE_3  						3			//SPI0工作模式3:CPHOL = 1 CPHA = 1
		
#define SPI_CLK_DIV_2           			0			//波特率2分频
#define SPI_CLK_DIV_4           			1			//波特率4分频
#define SPI_CLK_DIV_8           			2           //波特率8分频
#define SPI_CLK_DIV_16          			3           //波特率16分频
#define SPI_CLK_DIV_32          			4           //波特率32分频
#define SPI_CLK_DIV_64          			5           //波特率64分频
#define SPI_CLK_DIV_128         			6           //波特率128分频
#define SPI_CLK_DIV_256         			7           //波特率256分频
			
#define SPI_ERROR_IE						2			//SPI错误中断使能
#define SPI_TXE_IE							1			//SPI TXBUF空中断使能
#define SPI_RXNE_IE							0			//SPI RXBUF非空中断使能

#define SPI_RXBUF_WCOL_IF					4			//rx溢出中断标志
#define SPI_TXBUF_WCOL_IF					3			//tx为满标志
#define SPI_BUSY_IF							2			//SPI是否传输数据中断标志
#define SPI_TXE_IF							1			//SPI TXBUF空中断标志
#define SPI_RXF_IF							0			//SPI RXBUF非空中断标志

#define SPI_IRQ_ENABLE  					1
#define SPI_IRQ_DISABLE  					0

void spi_master_init(uint8_t work_mode,uint8_t spi_baudrate_psc);

void spi_slave_init(uint8_t work_mode);

void spi_deinit(void);

void spi_irq_init(uint8_t irq_enable, uint8_t spi_irq_type,void (*pfunc)());

void spi_irq_disable(void);
	
void spi_send_byte(uint8_t txdata);

uint8_t spi_receive_byte(void);

uint8_t spi_write_read_byte(uint8_t byte);

void spi_cs_enable(void);

void spi_cs_disable(void);

uint8_t spi_master_half_duplex_send_bytes(uint8_t *buff, uint32_t length);

uint8_t spi_master_half_duplex_receive_bytes(uint8_t *buff, uint32_t length);

uint8_t spi0_master_full_duplex_tranfer(uint8_t *send_buff, uint8_t *rec_buff, uint32_t length);
#endif
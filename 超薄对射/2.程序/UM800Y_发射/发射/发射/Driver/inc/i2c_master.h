/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : i2c.h
 * Description : i2c driver header file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-31
 ***********************************************************************/
#ifndef __I2C_H__
#define __I2C_H__

#include "um800y.h"

#define MTX_ANTO_EN 				(uint8_t)0x40		
#define OD_MODE 						(uint8_t)0x20	
#define MAAS2_INT_EN 				(uint8_t)0x8	
#define WBT_INT_EN 					(uint8_t)0x4	
#define RXNE_INT_EN 				(uint8_t)0x2	
#define TXE_INT_EN 					(uint8_t)0x1	
#define MAAS1_INT_EN 				(uint8_t)0x80	
#define MIEN 								(uint8_t)0x40	
#define RSTA 								(uint8_t)0x20
#define TACK 								(uint8_t)0x10
#define MTX 								(uint8_t)0x8
#define MSTA 								(uint8_t)0x4
#define MEN 								(uint8_t)0x1

#define WBT							(uint8_t)0x4    		//字节传输完成且TXE或RXNE为1
#define RXNE						(uint8_t)0x2				//接收时数据寄存器非空
#define TXE							(uint8_t)0x1				//发送时数据寄存器空
#define MAAS2						(uint8_t)0x80				//设备地址2和接收到的地址相等
#define MTF							(uint8_t)0x40				//字节传输完成
#define MAAS1						(uint8_t)0x20				//设备地址1和接收到的地址相等
#define MBB							(uint8_t)0x10				//总线上正在进行数据通信（检测到总线上START标志，此位清1）
#define SRW							(uint8_t)0x4				//作为从设备发送器
#define MTF_H						(uint8_t)0x2				//快速字节传输完成
#define RACK						(uint8_t)0x1				//最近的发送应答周期没有接收到应答

//speed
#define MASTER_I2C_SPEED_100K   	100000  //Standard-mode
#define MASTER_I2C_SPEED_400K			400000  //Fast-mode

#define I2C_SLAVE_ADDR1   0x20
#define I2C_SLAVE_ADDR2   0x40

#define	I2C_IRQ_MTF    	0
#define	I2C_IRQ_MAAS1  	1
#define	I2C_IRQ_MAAS2  	2
#define	I2C_IRQ_RXNE   	3
#define	I2C_IRQ_TXE    	4	
#define	I2C_IRQ_WBT   	5

void i2c_master_init(uint32_t i2c_speed);

void i2c_start(void);

void i2c_restart(void);

void i2c_stop(void);

void i2c_ack(void);

void i2c_no_ack(void);

uint8_t i2c_wait_ack(void);

void i2c_write_byte(uint8_t byte);

uint8_t i2c_read_byte(void);

void i2c_irq_enable(uint8_t irq_type ,void (*pfunc)());

void i2c_irq_disable(uint8_t irq_type);

uint8_t i2c_master_write_data(uint8_t slave_addr, uint8_t *buffer, uint16_t len);

uint8_t i2c_master_read_data(uint8_t slave_addr, uint8_t *buffer, uint16_t len);


#endif


/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : eflash.c
 * Description : eflash driver header file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-29
 ***********************************************************************/
#ifndef __EFLASH_H__
#define __EFLASH_H__

#include "um800y.h"


#define PAGE_NUM                 		128						//512bytes * 128 page = 64KBytes 
	
#define PAGE_SIZE                   	512
	
#define MAIN_BASE_ADDR					(0x0000)				//remap后

#define EEPROM0_BASE_ADDR				(0x8A00)				//remap后
#define EEPROM1_BASE_ADDR				(0x8C00)				//remap后
	

//EEPROM0地址
#define SM_KEY_RD_DISABLE				(EEPROM0_BASE_ADDR+0x1F0)		//PRIVATE KEY可读控制信号
#define SM_FLASH_CRYPT_EN				(EEPROM0_BASE_ADDR+0x1F4)		//flash加解密开关
#define SM_PRIVATE_KEY					(EEPROM0_BASE_ADDR+0x1F8)		//客户加解密私钥
#define SM_OTP_C_EN						(EEPROM0_BASE_ADDR+0x1FC)		//NVR_C区擦写锁定使能位

#define NVR1_BASE_ADDR					(0x9000)
#define NVR2_BASE_ADDR					(0x9200)


//NVR2地址
#define SM_NVR2_UID0_ADDR				(NVR2_BASE_ADDR+0x48)
#define SM_NVR2_UID1_ADDR				(NVR2_BASE_ADDR+0x4C)
#define SM_NVR2_UID2_ADDR				(NVR2_BASE_ADDR+0x50)
#define SM_NVR2_UID3_ADDR				(NVR2_BASE_ADDR+0x54)
#define SM_NVR2_CRC_ADDR				(NVR2_BASE_ADDR+0x58)

#define EFC_RD_WAIT 					(5<<3)  					//EFC_CTRL：等待5个系统时钟周期
#define EFC_WRITE_MODE					(1<<0)  					//EFC_CTRL：Write模式设使能
#define EFC_PAGE_ERASE_MODE				(1<<1)  					//EFC_CTRL：Sector Erase Mode模式设使能
#define EFC_CHIP_ERASE_MODE				(1<<2)  					//EFC_CTRL：Chip Erase Mode模式设使能


void eflash_init(uint32_t system_clk_hz);   
void eflash_write_byte(uint16_t addr,uint8_t value);						//write 8bits
void eflash_write_word(uint16_t addr,uint16_t value);						//write 16bits
void eflash_write_dword(uint16_t addr,uint32_t value);						//write 32bits
void eflash_erase_page(uint16_t page_addr);
void eflash_erase_chip(void);
void eflash_write_bytes(uint16_t addr, uint8_t *buff, uint32_t length);
uint8_t eflash_read_byte(uint16_t addr);
uint32_t eflash_read_bytes(uint16_t addr, uint8_t *buff, uint32_t length);
uint16_t eflash_read_word(uint16_t addr);
uint32_t eflash_read_dword(uint16_t addr);

void read_sequence(uint8_t *buff);
void read_UID(uint8_t *buff);
void read_Vcap(uint8_t *buff);

#endif


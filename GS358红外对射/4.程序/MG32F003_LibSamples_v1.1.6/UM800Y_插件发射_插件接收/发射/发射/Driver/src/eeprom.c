/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : eflash.c
 * Description : eflash driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-29
 ***********************************************************************/
#include  "eeprom.h"

void eeprom_init(uint32_t system_clk_hz)  
{	
	uint8_t system_clk_mhz;
	
	if(system_clk_hz < 2000000)
	{
		return;
	}
	
	system_clk_mhz = system_clk_hz/1000000;
	
	OPSET |= (0<<3);			         //RD_WAIT = 0
    
	if(((*(volatile uint8_t xdata *)(0x9000+0x1B0)) ) == ~((*(volatile uint8_t xdata *)(0x9000+0x1B0+2))))
    {
        OUS = (*(volatile uint8_t xdata *)(0x9000+0x1B0));
    }
    else
    {
        OUS = 0x0f;      
    }
	
	OINTUS = 0x3f;   			         //清除所有中断标志位
	OINTEN = 0x00;		 		         //关闭所有中断功能
		
}

void eeprom_erase_page(uint16_t page_addr)	
{
	OPSET &= ~(7<<0);
	OPSET |= EFC_NVR_SET_MODE;  		//开启eeprom擦写使能
	OPSET |= EFC_PAGE_ERASE_MODE;		//开启page擦除模式
	OADR = page_addr;
	OCTRL |= (0x1<<3);					//PUMP_SEL = 6.75v
	OCTRL |= (1<<6);					//PUMP_EN=1
	while(!(OCTRL & (1<<1)));			//wait for PUMP OK
	OCTRL |= (1<<7);					//VPPO_EN = 1
	while((OCTRL & (1<<7)));			//wait for VPPO_EN = 0
	OCTRL &= ~(1<<6);					//PUMP_EN=0
	OPSET &= ~EFC_PAGE_ERASE_MODE; 		//关闭page擦除模式
	OPSET &= ~EFC_NVR_SET_MODE; 		//关闭eeprom擦写使能
}

void eeprom_write_byte(uint16_t addr,uint8_t value)	 
{
	OPSET &= ~(7<<0);
	OPSET |= EFC_NVR_SET_MODE;  		//开启eeprom擦写使能
	OPSET |= EFC_WRITE_MODE;			//使能写模式	
	OADR = addr;
	ODATA = value;
	OCTRL |= (0x1<<3);					//PUMP_SEL = 6.75v
	OCTRL |= (1<<6);					//PUMP_EN=1
	while(!(OCTRL & (1<<1)));			//wait for PUMP OK;
	OCTRL |= (1<<7);					//VPPO_EN = 1
	while((OCTRL & (1<<7)));			//wait for VPPO_EN = 0
	OCTRL &= ~(1<<6);					//PUMP_EN=0

	OPSET &= ~EFC_WRITE_MODE;			//关闭写模式
	OPSET &= ~EFC_NVR_SET_MODE; 		//关闭eeprom擦写使能
	
}

void eeprom_write_bytes(uint16_t addr, uint8_t *buff, uint32_t length)	 
{ 
	OPSET &= ~(7<<0);
	OPSET |= EFC_NVR_SET_MODE;  		//开启eeprom擦写使能
	OPSET |= EFC_WRITE_MODE;			//使能写模式 
    while(length--)
    {
        OADR = addr++;
        ODATA = *buff++;
        OCTRL |= (0x1<<3);				//PUMP_SEL = 6.75v
        OCTRL |= (1<<6);				//PUMP_EN=1
        while(!(OCTRL & (1<<1)));	    //wait for PUMP OK;
        OCTRL |= (1<<7);				//VPPO_EN = 1
        while((OCTRL & (1<<7)));	    //wait for VPPO_EN = 0
        OCTRL &= ~(1<<6);				//PUMP_EN=0     
    }
	OPSET &= ~EFC_WRITE_MODE;			//关闭写模式
	OPSET &= ~EFC_NVR_SET_MODE; 		//关闭eeprom擦写使能
}

uint8_t eeprom_read_byte(uint16_t addr)
{
	uint8_t value;

	value = (*(volatile uint8_t xdata *)(addr));
      
	return value;

}

uint32_t eeprom_read_bytes(uint16_t addr, uint8_t *buff, uint32_t length)
{
	uint32_t index;
    
    for(index = 0; index<length; index++)
    {
        buff[index] = (*(volatile uint8_t xdata *)(addr+index));        
    }
	
	return index;
}



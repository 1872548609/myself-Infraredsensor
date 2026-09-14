/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : eflash.c
 * Description : eflash driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-29
 ***********************************************************************/
#include  "eflash.h"

#include "config.h"


void eflash_init(uint32_t system_clk_hz)  
{	
	uint8_t system_clk_mhz;
	
	if(system_clk_hz < 2000000)
	{
		return;
	}
	
	system_clk_mhz = system_clk_hz/1000000;
	
	OPSET |= (0<<3);			         //RD_WAIT = 0
    
	//OUS = (system_clk_mhz/2)-1;			 //系统时钟频率(MHz)/2-1
    OUS = 0x0f;	
	OINTUS = 0x3f;   			         //清除所有中断标志位
	OINTEN = 0x00;		 		         //关闭所有中断功能
		
}

void eflash_erase_page(uint16_t page_addr)	
{
    bit temp;
    temp = EA;    
    EA = 0;                             //关闭总中断
    
	OPSET &= ~(7<<0);
	OPSET |= EFC_PAGE_ERASE_MODE;  		//开启page擦除模式
	
	OADR = page_addr;
	OCTRL |= (0x1<<3);					//PUMP_SEL = 6.75v
	OCTRL |= (1<<6);					//PUMP_EN=1
	while(!(OCTRL & (1<<1)));			//wait for PUMP OK
	OCTRL |= (1<<7);					//VPPO_EN = 1
	while((OCTRL & (1<<7)));			//wait for VPPO_EN = 0
	OCTRL &= ~(1<<6);					//PUMP_EN=0
	
	OPSET &= ~EFC_PAGE_ERASE_MODE; 		//关闭page擦除模式
    
    EA = temp;                          //恢复中断源状态
}

void eflash_write_byte(uint16_t addr,uint8_t value)	 
{
    bit temp;
    temp = EA;    
    EA = 0;                             //关闭总中断
    
	OPSET &= ~(7<<0);
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
    
    EA = temp;                          //恢复中断源状态
	
}

void eflash_write_bytes(uint16_t addr, uint8_t *buff, uint32_t length)	 
{     
    bit temp;
    temp = EA;    
    EA = 0;                             //关闭总中断
    
	OPSET &= ~(7<<0);
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
      
    EA = temp;                          //恢复中断源状态
}

uint8_t eflash_read_byte(uint16_t addr)
{
	uint8_t value;

	value = (*(volatile uint8_t code *)(addr));
      
	return value;

}

uint32_t eflash_read_bytes(uint16_t addr, uint8_t *buff, uint32_t length)
{
	uint32_t index;
    
    for(index = 0; index<length; index++)
    {
        buff[index] = (*(volatile uint8_t code *)(addr+index));        
    }
	
	return index;
}




/*----------------------------SN--------------------------------*/

#define SN_ADDR     						(NVR2_BASE_ADDR+0x48)  //SN addr
#define SN_CRC_ADDR     					(NVR2_BASE_ADDR+0x58)  //SN CRC value
#define VDD25_ADDR                          (NVR1_BASE_ADDR+0x19C) 


void read_sequence(uint8_t *buff)
{
    uint8_t i = 0;
   
    for(i = 0; i < 16; i++)
    {
        *buff++ = (*(volatile uint8_t xdata*)(SN_ADDR+i));
    }

}


void read_UID(uint8_t *buff)
{
    uint32_t temp = 0;
    temp =  (*(volatile uint32_t xdata*)(SN_ADDR));
     
    buff[0] = temp >> 24;
    buff[1] = temp >> 16;
    buff[2] = temp >> 8;
    buff[3] = temp;
    
    temp =  (*(volatile uint8_t xdata*)(SN_ADDR+4));
    buff[4] = temp;

    temp =  (*(volatile uint8_t xdata*)(SN_ADDR+8));
    buff[5] = temp;
    temp =  (*(volatile uint8_t xdata*)(SN_ADDR+10));
    buff[6] = temp;

    temp =  (*(volatile uint8_t xdata*)(SN_ADDR+14));
    buff[7] = temp;

}


void read_Vcap(uint8_t *buff)
{
    uint32_t temp = 0;
    temp =  (*(volatile uint32_t xdata*)(VDD25_ADDR));
    buff[0] = temp >> 24;
    buff[1] = temp >> 16;
    buff[2] = temp >> 8;
    buff[3] = temp;


}




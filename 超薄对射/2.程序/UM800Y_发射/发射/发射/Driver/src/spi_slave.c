/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : spi.c
 * Description : spi driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2020-08-03
 ***********************************************************************/
#include "spi_slave.h"

void (*spi_func)(void) = {0};


/************************************************************************
 * Function   	: SPI_IRQHandler
 * Description	: SPI中断处理函数
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void SPI_IRQHandler(void) interrupt 7
{
	if((REG_SPI_SPSR & (0x01<<0)))		//RXNE
	{
		if(spi_func != NULL)
		{
			spi_func();
		}	
	}
			
}

/************************************************************************
 * Function   	: spi_master_init
 * Description	: spi_master_init	SPI主机初始化
 * Input 		: uint8_t work_mode	选择SPI工作模式0/1/2/3 
 *				  uint8_t spi_baudrate_psc	选择SPI波特率分频 SPI通信速率=fSYSCLK/波特率分频
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_master_init(uint8_t work_mode,uint8_t spi_baudrate_psc)
{				
	PRESET0 |= (1<<3);								//SPI复位释放
	PCLK0   |= (1<<3);								//SPI模块时钟使能
	
	REG_SPI_SPCR1 = 0x00;
	REG_SPI_SPCR2 = 0x00;
	REG_SPI_SPCR3 = 0xF4;
	
	REG_SPI_SPCR1 |= (1<<2);						//master模式
	REG_SPI_SPCR1 &= ~(1<<3);						//先发送MSB
	REG_SPI_SPCR1 |= (spi_baudrate_psc<<5);			//波特率分频
	REG_SPI_SPCR2 |= (1<<4);						//软件控制SSN
		
	
	switch(work_mode)
	{
		case WORK_MODE_0:
			REG_SPI_SPCR1 &= ~(1<<0);    			//在SCK第一个边沿采样
			REG_SPI_SPCR1 &= ~(1<<1);				//SCK空闲时低电平	
			break;		
		case WORK_MODE_1:		
			REG_SPI_SPCR1 |= (1<<0);       			//在SCK第二个边沿采样	
			REG_SPI_SPCR1 &= ~(1<<1);	    		//SCK空闲时低电平	
			break;		
		case WORK_MODE_2:		
			REG_SPI_SPCR1 &= ~(1<<0);      			//在SCK第一个边沿采样	
			REG_SPI_SPCR1 |= (1<<1);       			//SCK空闲时高电平	
			break;		
		case WORK_MODE_3:		
			REG_SPI_SPCR1 |= (1<<0);       			//在SCK第二个边沿采样
			REG_SPI_SPCR1 |= (1<<1);       			//SCK空闲时高电平	
			break;
		default:		
			break;
	}
	
	REG_SPI_SPCR2 |= (1<<5);						//使能SPI

}

/************************************************************************
 * Function   	: spi_slave_init
 * Description	: spi_slave_init	SPI从机初始化
 * Input 		: uint8_t work_mode	选择SPI工作模式0/1/2/3 
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_slave_init(uint8_t work_mode)
{
	PRESET0 |= (1<<3);								//SPI复位释放
	PCLK0   |= (1<<3);								//SPI模块时钟使能
	
	REG_SPI_SPCR1 = 0x00;
	REG_SPI_SPCR2 = 0x00;
	REG_SPI_SPCR3 = 0xF4;
	
	REG_SPI_SPCR1 &= ~(1<<2);						//slave模式
	REG_SPI_SPCR3 |= (1<<1);						//对SSN/SCK/MOSI上可能产生的毛刺数字滤波
//	REG_SPI_SPCR3 |= (1<<0);						//slave模式下，提前半个周期发送(速率12MHz时用)

	switch(work_mode)
	{
		case WORK_MODE_0:
			REG_SPI_SPCR1 &= ~(1<<0);    			//在SCK第一个边沿采样
			REG_SPI_SPCR1 &= ~(1<<1);				//SCK空闲时低电平	
			break;		
		case WORK_MODE_1:		
			REG_SPI_SPCR1 |= (1<<0);       			//在SCK第二个边沿采样	
			REG_SPI_SPCR1 &= ~(1<<1);	    		//SCK空闲时低电平	
			break;		
		case WORK_MODE_2:		
			REG_SPI_SPCR1 &= ~(1<<0);      			//在SCK第一个边沿采样	
			REG_SPI_SPCR1 |= (1<<1);       			//SCK空闲时高电平	
			break;		
		case WORK_MODE_3:		
			REG_SPI_SPCR1 |= (1<<0);       			//在SCK第二个边沿采样
			REG_SPI_SPCR1 |= (1<<1);       			//SCK空闲时高电平	
			break;
		default:		
			break;
	}
	
	REG_SPI_SPCR2 |= (1<<5);						//使能SPI
}
	
/************************************************************************
 * Function   	: spi_deinit
 * Description	: spi_deinit	关闭SPI模块
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_deinit(void)
{
	PRESET0 &= ~(1<<3);								//SPI复位
	PCLK0   &= ~(1<<3);								//SPI模块时钟关闭
}

/************************************************************************
 * Function   	: spi_irq_enable
 * Description	: spi_irq_enable spi中断使能
 * Input 		: uint8_t spi_irq_type 中断类型
 *                void (*pfunc)() 中断回调函数
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_irq_init(uint8_t irq_enable, uint8_t spi_irq_type,void (*pfunc)())
{
	if(irq_enable == SPI_IRQ_ENABLE)
	{
		switch(spi_irq_type)
		{
			case SPI_RXNE_IE:
				REG_SPI_SPIIE |= (1<<SPI_RXNE_IE);			//SPI错误中断使能
				break;
			case SPI_TXE_IE:
				REG_SPI_SPIIE |= (1<<SPI_TXE_IE);			//SPI TXBUF空中断使能
				break;
			case SPI_ERROR_IE:
				REG_SPI_SPIIE |= (1<<SPI_ERROR_IE);			//SPI RXBUF非空中断使能
				break;
			default:
				break;
		}
		
	spi_func = pfunc;									//SPI中断回调函数
	SPIINTEN = 1;										//使能SPI中断
	EA = 1;												//使能总中断
	}
	else
	{
		REG_SPI_SPIIE = 0x00;
		
		spi_func = NULL;
		SPIINTEN = 0;										//关闭SPI中断
		EA = 0;												//关闭总中断
		
	}
}

/************************************************************************
 * Function   	: spi_irq_disable
 * Description	: spi_irq_disable
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_irq_disable(void)
{
	REG_SPI_SPIIE = 0x00;
	SPIINTEN = 0;
}

/************************************************************************
 * Function   	: spi_send_byte
 * Description	: spi_send_byte 写入一字节
 * Input 		: uint8_t txdata	写入一字节数据
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_send_byte(uint8_t txdata)
{	
	while(!(REG_SPI_SPSR & (1<<1)));							//等待发送缓存为空，TXBE事件
	
	REG_SPI_TXBUF = txdata;
}

/************************************************************************
 * Function   	: spi_receive_byte
 * Description	: spi_receive_byte 读取一字节
 * Input 		: none
 * Output		: none   
 * Return		: REG_SPI_RXBUF	读取一字节数据
 ************************************************************************/
uint8_t spi_receive_byte(void)
{
	while(!(REG_SPI_SPSR & (1<<0)));							//等待接收缓存非空，RXBF事件
	
	return REG_SPI_RXBUF;										//返回读取寄存器的值

}

/************************************************************************
 * Function   	: spi_write_read_byte
 * Description	: spi_write_read_byte 读取一字节
 * Input 		: uint8_t byte	写入的一个字节
 * Output		: none   
 * Return		: REG_SPI_RXBUF 	读取到的一个字节
 ************************************************************************/
uint8_t spi_write_read_byte(uint8_t byte)
{	
	while(!(REG_SPI_SPSR & (1<<1)));							//等待发送缓存为空，TXBE事件

	REG_SPI_TXBUF = byte;										//写入数据寄存器，把要写入的数据写入发送缓存	
	
	while(!(REG_SPI_SPSR & (1<<0)));							//等待接收缓存非空，RXBF事件

	return REG_SPI_RXBUF;										//返回读取寄存器的值

}

//全双工
uint8_t spi0_master_full_duplex_tranfer(uint8_t *send_buff, uint8_t *rec_buff, uint32_t length)
{
	while(length--)
	{
		while(!(REG_SPI_SPSR & (1<<SPI_TXE_IF)));		//等待发送缓存为空
		
		REG_SPI_TXBUF = *send_buff++;
		
		while(!(REG_SPI_SPSR & (1<<SPI_RXF_IF)));				//等待接收缓存非空，RXBF事件		
		
		*rec_buff++ = REG_SPI_RXBUF;
	}
	
	return 0;
}


//半双工传输
uint8_t spi_master_half_duplex_send_bytes(uint8_t *buff, uint32_t length)
{
	REG_SPI_SPCR3 |= (1<<3);					//txonly
	while(length--)
	{
		while(!(REG_SPI_SPSR & (1<<SPI_TXE_IF)));		//等待发送缓存为空
	
		REG_SPI_TXBUF = *buff++;
	}
	while((REG_SPI_SPSR & (1<<SPI_BUSY_IF)));				//等待BUSY=0
	
	REG_SPI_SPCR3 &= ~(1<<3);								//关闭txonly
	
	return 0;
}
 
uint8_t  spi_master_half_duplex_receive_bytes(uint8_t *buff, uint32_t length)
{
	uint32_t time_out = 0xFFFFFFFF;
	while(length--)
	{
		while(!(REG_SPI_SPSR & (1<<SPI_RXF_IF)))	//等待接收缓存非空
		{
			if((time_out--) == 0) 
			{
				return 1;
			}
		}
		*buff++ = REG_SPI_RXBUF;
	}

		return 0;

}
/************************************************************************
 * Function   	: spi_cs_enable
 * Description	: spi_cs_enable CS片选拉低开始工作
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/ 
void spi_cs_enable(void)
{
	 REG_SPI_SPCR3 &= ~(1<<2); 
}

/************************************************************************
 * Function   	: spi_cs_disable
 * Description	: spi_cs_disable CS片选拉高停止工作
 * Input 		: none
 * Output		: none   
 * Return		: none
 ************************************************************************/
void spi_cs_disable(void)
{ 
	REG_SPI_SPCR3 |= (1<<2); 
	
}

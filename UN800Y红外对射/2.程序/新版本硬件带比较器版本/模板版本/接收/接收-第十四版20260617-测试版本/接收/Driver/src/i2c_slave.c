/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : i2c.c
 * Description : i2c driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-31
 ***********************************************************************/
#include "i2c_slave.h"

void (*i2c_irq[6])(void) = { 0 };

void I2C_IRQHandler(void) interrupt 13
{
	if((REG_I2C_SR0 & MTF) && (REG_I2C_CR0 & MIEN))
	{
		if(i2c_irq[0] != NULL)
		{
			i2c_irq[0]();
		}
		REG_I2C_SR0 = MTF;			//清MTF状态位
	}
	else if((REG_I2C_SR0 & MAAS1) && (REG_I2C_CR0 & MAAS1_INT_EN))
	{
		if(i2c_irq[1] != NULL)
		{
			i2c_irq[1]();
		}
		REG_I2C_SR0 = MAAS1;			//清MAAS1状态位
	}
	else if((REG_I2C_SR0 & MAAS2) && (REG_I2C_CR0 & MAAS2_INT_EN))
	{
		if(i2c_irq[2] != NULL)
		{
			i2c_irq[2]();
		}
		REG_I2C_SR0 = MAAS2;			//清MAAS2状态位
	}
	else if((REG_I2C_SR1 & RXNE) && (REG_I2C_CR0 & RXNE_INT_EN))
	{
		if(i2c_irq[3] != NULL)
		{
			i2c_irq[3]();
		}
		REG_I2C_SR1 = RXNE;				//清RXNE状态位
	}
	else if((REG_I2C_SR1 & TXE) && (REG_I2C_CR0 & TXE_INT_EN))
	{
		if(i2c_irq[4] != NULL)
		{
			i2c_irq[4]();
		}
		REG_I2C_SR1 = TXE;			//清TXE状态位
	}
	else if((REG_I2C_SR1 & WBT) && (REG_I2C_CR0 & WBT_INT_EN))
	{
		if(i2c_irq[5] != NULL)
		{
			i2c_irq[5]();
		}
		REG_I2C_SR1 = WBT;			//清WBT状态位
	}	
	else
	{
		REG_I2C_SR0 |= (0x7<<4);  //清MAAS1、MAAS2和MTF状态位
		REG_I2C_SR1 = 0x7;				//清WBT、RXNE和TXE状态位
	}

}


/************************************************************************
 * Function   	: i2c_ack
 * Description	: i2c_ack
 * Input 				: none
 * Output				: none  
 * Return				: none
 ************************************************************************/
void i2c_ack(void)
{
	REG_I2C_CR0 &= ~(1<<4);		//应答ACK
}

/************************************************************************
 * Function   	: i2c_no_ack
 * Description	: i2c_no_ack
 * Input 				: none
 * Output				: none  
 * Return				: none
 ************************************************************************/
void i2c_no_ack(void)
{
	REG_I2C_CR0 |= (1<<4);			//不应答ACK
}

/************************************************************************
 * Function   	: i2c_wait_ack
 * Description	: i2c_wait_ack	等待应答
 * Input      	: none	
 * Output				: none  
 * Return     	: 1:NACK  0:ACK
 ************************************************************************/
uint8_t i2c_wait_ack(void)
{
	uint16_t data timeout = 65535;
	
	while((REG_I2C_SR0 & RACK) == RACK)
	{
		if(timeout-- == 0)
		{
			return 1;
		}
	}
	return 0;
}

/************************************************************************
 * Function   	: i2c_write_addr
 * Description	: i2c_write_addr	发送I2C地址+R/W标志
 * Input      	: uint8_t addr
 * Output				: none  
 * Return     	: none
 ************************************************************************/
void i2c_write_addr(uint8_t addr)
{
	REG_I2C_DR = addr;						//发送I2C地址+R/W标志				
}


/************************************************************************
 * Function   	: i2c_write_byte
 * Description	: i2c_write_byte	写一个byte
 * Input      	: uint8_t byte
 * Output				: none  
 * Return     	: none
 ************************************************************************/
void i2c_write_byte(uint8_t byte)
{	
	while(!(REG_I2C_SR1 & TXE));  			//等待TXE为空
	
	REG_I2C_DR = byte;						//发送数据
	
	while(!(REG_I2C_SR0 & MTF));  			//等待字节传输完成
	
	REG_I2C_SR0 = MTF;	
	
}

/************************************************************************
 * Function   	: i2c_read_byte
 * Description	: i2c_read_byte	读一个byte
 * Input      	: none
 * Output				: none  
 * Return     	: uint8_t byte
 ************************************************************************/
uint8_t i2c_read_byte(void)
{
	uint8_t byte;
	
	REG_I2C_CR0 &= ~MTX; 								//作为接收器
	
	while(!(REG_I2C_SR1 & RXNE)); 			//等待接收数据寄存器非空
	
	while(!(REG_I2C_SR0 & MTF));  			//等待字节传输完成
	
	REG_I2C_SR0 = MTF;
	
	byte = REG_I2C_DR;
			
	return byte;
}

/***********************************************************************
 * Function   	: i2c_irq_enable
 * Description	: 使能I2C中断
 * Input 				: uint8_t irq_type：i2c中断源
 *           		：void (*pfunc)()：产生中断之后执行的回调函数
 * Output				: none
 * Return				: none
 ***********************************************************************/
void i2c_irq_enable(uint8_t irq_type,void (*pfunc)())
{
	EA = 1;																//打开总中断开关
	IEN1 |= (1<<5);												//打开i2c中断开关
	switch(irq_type)
	{
		case I2C_IRQ_MTF:
		{
			REG_I2C_CR0 |= MIEN;
			i2c_irq[0] = pfunc;
			break;
		}
		case I2C_IRQ_MAAS1:
		{
			REG_I2C_CR0 |= MAAS1_INT_EN;
			i2c_irq[1] = pfunc;
			break;
		}
		case I2C_IRQ_MAAS2:
		{
			REG_I2C_CR1 |= MAAS2_INT_EN;
			i2c_irq[2] = pfunc;
			break;
		}
		case I2C_IRQ_RXNE:
		{
			REG_I2C_CR1 |= RXNE_INT_EN;
			i2c_irq[3] = pfunc;
			break;
		}
		case I2C_IRQ_TXE:
		{
			REG_I2C_CR1 |= TXE_INT_EN;
			i2c_irq[4] = pfunc;
			break;
		}
		case I2C_IRQ_WBT:
		{
			REG_I2C_CR1 |= WBT_INT_EN;
			i2c_irq[5] = pfunc;
			break;
		}
	}
}

/***********************************************************************
 * Function   	: i2c_irq_disable
 * Description	: 关闭中断
 * Input 				: uint8_t irq_type：i2c中断源
 * Output				: none
 * Return				: none
 ***********************************************************************/
void i2c_irq_disable(uint8_t irq_type)
{
	switch(irq_type)
	{
		case I2C_IRQ_MTF:
		{
			REG_I2C_CR0 &= ~MIEN;
			i2c_irq[0] = NULL;
			break;
		}
		case I2C_IRQ_MAAS1:
		{
			REG_I2C_CR0 &= ~MAAS1_INT_EN;
			i2c_irq[1] = NULL;
			break;
		}
		case I2C_IRQ_MAAS2:
		{
			REG_I2C_CR1 &= ~MAAS2_INT_EN;
			i2c_irq[2] = NULL;
			break;
		}
		case I2C_IRQ_RXNE:
		{
			REG_I2C_CR1 &= ~RXNE_INT_EN;
			i2c_irq[3] = NULL;
			break;
		}
		case I2C_IRQ_TXE:
		{
			REG_I2C_CR1 &= ~TXE_INT_EN;
			i2c_irq[4] = NULL;
			break;
		}
		case I2C_IRQ_WBT:
		{
			REG_I2C_CR1 &= ~WBT_INT_EN;
			i2c_irq[5] = NULL;
			break;
		}
	}
}


/************************************************************************
 * Function   	: i2c_slave_init
 * Description	: i2c_slave_init	I2C从机初始化
 * Input 				: none
 * Output				: none  
 * Return				: none
 ************************************************************************/
void i2c_slave_init(void)
{
	PCLK0 |= (1<<7);							//打开I2C模块时钟
	PRESET0 |= (1<<7);						//释放I2C复位
				
	P0PU &= ~(1<<4);							//P04上拉
	P1PU &= ~(1<<0);           		//P10上拉
	
	REG_P04_CFG = 0x03;						//I2C_SDA 		
	REG_P10_CFG = 0x04;						//I2C_SCL	
	
	REG_I2C_CR1 |= (1<<5);				//使能开漏模式		
	REG_I2C_SR0 |= MTF; 					//清除传输完成状态位
	REG_I2C_CR1 |= MTX_ANTO_EN;		//SLAVE模式下自动切换SDA传输方向

	REG_I2C_CR0 |= (1<<1);				//hold_en使能
	REG_I2C_CR0 |= MEN;						//I2C使能
	
}

/************************************************************************
 * Function   	: i2c_set_slave_addr1
 * Description	: i2c_set_slave_addr1 设置从机匹配地址
 * Input 				: uint8_t slave_addr	从机地址
 * Output				: none  
 * Return				: none
 ************************************************************************/
void i2c_set_slave_addr1(uint8_t slave_addr)
{
	REG_I2C_SLAVE_ADDR1 = slave_addr;
	
}

/************************************************************************
 * Function   	: i2c_set_slave_addr2
 * Description	: i2c_set_slave_addr2 设置从机匹配地址
 * Input 				: uint8_t slave_addr	从机地址
 * Output				: none  
 * Return				: none
 ************************************************************************/
void i2c_set_slave_addr2(uint8_t slave_addr)
{
	REG_I2C_SLAVE_ADDR2 = slave_addr | 0x01;

}

/************************************************************************
* Function   	: i2c_slave_read
* Description	: i2c_slave_read
* Input 			: uint8_t *rxdata: 要接收的数据
* Output			: none  
* Return			: datalen	数据传输长度
************************************************************************/
uint8_t i2c_slave_read(uint8_t *rxdata)
{
	uint8_t datalen;
	
	datalen = 0;
	
	while((REG_I2C_SR0 & MBB)) //MBB
	{
		if((REG_I2C_SR1 & RXNE) == RXNE) //接收时数据寄存器非空
		{	
			rxdata[datalen] = i2c_read_byte();
			datalen++;
		}
	}
	REG_I2C_SR0 = MTF;

	return datalen;
}	


/************************************************************************
* Function   	: i2c_slave_write
* Description	: i2c_slave_write
* Input 			: uint8_t* txdata: 要发送的数据 
* Output			: none  
* Return			: datalen	数据传输长度
************************************************************************/
uint8_t i2c_slave_write(uint8_t *txdata)
{
	uint8_t datalen;
	
  datalen = 0;
	
	while(!(REG_I2C_SR0 & MTF)); 
  REG_I2C_SR0 = MTF;
	REG_I2C_DR = txdata[0];		//发送第一个数据

	while(REG_I2C_SR0 & MBB)     //MBB add TimeOut
	{
		if((REG_I2C_SR1 & WBT)) 		//字节传输完成且TXE为1
		{			
				datalen++;
				REG_I2C_DR = txdata[datalen];
		}
	}
	REG_I2C_SR0 = MTF;
	
  return datalen;	
}

/***********************************************************************
 * Function   	: i2c_slave_is_match_addr1
 * Description	: 用于判断是否匹配地址1
 * Input 				: none
 * Output				: none
 * Return				: 0-否/1-是
 ***********************************************************************/
uint8_t i2c_slave_is_match_addr1(void)
{
	if((REG_I2C_SR0 & MAAS1) == MAAS1) 			//maas1匹配
	{					
		REG_I2C_SR0 = MAAS1;									//清空地址匹配状态
		if(!(REG_I2C_SR0 & SRW)) 							//作为从接收器
		{	
			return i2c_rx;										
		}
		if((REG_I2C_SR0 & SRW)) 							//作为从发送器
		{	
			return i2c_tx;
		}
	}
	return 0;
}

/***********************************************************************
 * Function   	: i2c_slave_is_match_addr2
 * Description	: 用于判断是否匹配地址2
 * Input 				: none
 * Output				: none
 * Return				: 0-否/1-是
 ***********************************************************************/
uint8_t i2c_slave_is_match_addr2(void)
{
	if((REG_I2C_SR0 & MAAS2) == MAAS2) 		//maas2匹配
	{				
		REG_I2C_SR0 = MAAS2;								//清空地址匹配状态
		if(!(REG_I2C_SR0 & SRW)) 							//作为从接收器
		{	
			return i2c_rx;
		}
		if((REG_I2C_SR0 & SRW)) 							//作为从发送器
		{	
			return i2c_tx;
		}
	}
	return 0;
}


/************************************************************************
* Function   	: i2c_deinit
* Description	: i2c_deinit	关闭I2C模块
* Input 			: none 
* Output			: none  
* Return			: none
************************************************************************/
void i2c_deinit(void)
{
	PCLK0 &= ~(1<<7);					//关闭I2C模块时钟
	PRESET0 &= ~(1<<7);					//复位I2C模块
}
	


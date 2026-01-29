/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-04-27
 ***********************************************************************/
#include "system_um800y.h"
#include "app.h"
#include "common.h"
#include "uart0.h"
#include "spi_master.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

volatile uint8_t spi_rx_data;
volatile uint8_t spi_rx_flag = 0;
uint8_t xdata spi_tx_buf[256];
uint8_t xdata spi_rx_buf[256];
/************************************************************************
 * Function   	: uart0_rec_pro
 * Description	: uart0_rec_pro 串口接收处理回调函数
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart0_rec_pro(void)
{
	uart0_rx_buf[rx_count++]= uart0_recv_byte();
	rx_flag = 1;	
}

/************************************************************************
 * Function   	: uart_init
 * Description	: uart_init 串口初始化
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void uart_init(void)
{
	rx_flag = 0;
	uart0_init(UART0_BAUD_RATE);
    uart0_irq_init(UART0_IRQ_ENABLE,uart0_rec_pro);
}
/************************************************************************
 * Function   	: spi_irq_func
 * Description	: spi_irq_func spi中断服务函数
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void spi_irq_func(void)
{
	spi_rx_data = spi_receive_byte();
	spi_rx_flag = 1;
}
/************************************************************************
 * Function   	: datacmp
 * Description	: datacmp 比较数据
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void datacmp(uint8_t *data1,uint8_t* data2,uint32_t len)
{
	uint8_t i = len;
	while(len--)
	{
		if((*data1++) == (*data2++))
		i--;	
	}
	if(i == 0)
	{
		printfS("SPI测试成功！\n\r");
	}
	else
	{
		printfS("SPI测试失败！\n\r");
	}
}

/************************************************************************
 * Function   	: spi_full_duplex_tranfer
 * Description	: spi_full_duplex_tranfer spi全双共通信 主机发0xa0-0xa7 从机发0x00-0x07
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void spi_full_duplex_tranfer()
{
	uint16_t xdata i;
	printfS("SPI full duplex tranfer test start!! \r\n");
#ifdef SPI_IRQ_MODE										//中断方式，在app.h中开启
	printfS("SPI Master RXNE irq test start!! \r\n");
	spi_irq_init(SPI_IRQ_ENABLE,SPI_RXNE_IE,spi_irq_func);
	while(1)
	{
		spi_cs_enable();
	
		for(i=0;i<LEN;i++)
		{
			
			spi_send_byte(spi_tx_buf[i]);
			while(!spi_rx_flag);
			spi_rx_buf[i] = spi_rx_data;
			spi_rx_flag = 0;

		}		
		spi_cs_disable();
		printfS("spi master receive data:\r\n");
		printfB8(spi_rx_buf,LEN);
		
		delay1ms(1000);
	}
			
		
#else																//查询方式	
	spi_irq_init(SPI_IRQ_DISABLE,SPI_RXNE_IE,spi_irq_func);  	//关闭中断使能
	while(1)
	{
		spi_cs_enable();	
		spi0_master_full_duplex_tranfer(spi_tx_buf,spi_rx_buf,LEN);  //全双工
		spi_cs_disable();
		printfS("spi master receive data:\r\n");	
		printfB8(spi_rx_buf,LEN);
		
		delay1ms(1000);
	}
#endif
	
	
}

/************************************************************************
 * Function   	: spi_half_duplex_tranfer
 * Description	: spi_half_duplex_tranfer spi半双工
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void spi_half_duplex_tranfer()
{
	uint16_t xdata a;
	uint8_t xdata result = 0;
	printfS("SPI half duplex tranfer test start!! \r\n");
	while(1)
	{
		spi_cs_enable();
		spi_master_half_duplex_send_bytes(spi_tx_buf, LEN);
		delay1ms(10);
		spi_master_half_duplex_receive_bytes(spi_rx_buf, LEN);
		spi_cs_disable();
		
		printfS("spi master receive data:\r\n");	
		printfB8(spi_rx_buf,LEN);
			
		delay1ms(1000);
		datacmp(spi_tx_buf,spi_rx_buf,LEN);
		for(a=0;a<LEN;a++)
		{
			spi_tx_buf[a]+= 0x10;
		}
	}		
}	
/************************************************************************
 * Function   	: spi_test
 * Description	: spi_test 
 * Input 		: none
 * Output 		: none
 * Return		: none
 ************************************************************************/
void spi_test(void)
{
	uint16_t xdata i;
	
	for(i=0;i<LEN;i++)
	{
		spi_tx_buf[i]= i+0xa0;
	}
	
	printfS("SPI Master test start!! \r\n");
	spi_master_init(WORK_MODE_0,SPI_CLK_DIV_4);//SPI初始化，选择工作模式，SPI通信速率=fSYSCLK/波特率分频

#ifdef FULL_DUPLEX_MODE
	spi_full_duplex_tranfer();		//全双工传输 主机发送的数据带0xa?,从机的数据带0x0
#elif defined HALF_DUPLEX_MODE 
	spi_half_duplex_tranfer();		//半双工传输	
#endif
	
}

void soc_test(void)
{
	spi_test();
}

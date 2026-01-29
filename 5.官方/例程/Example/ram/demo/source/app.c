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
#include "config.h"
#include "common.h"
#include "uart0.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

uint8_t bdata temp _at_ 0x2F;

sbit temp0 = temp^0;
sbit temp1 = temp^1;
sbit temp2 = temp^2;
sbit temp3 = temp^3;
sbit temp4 = temp^4;
sbit temp5 = temp^5;
sbit temp6 = temp^6;
sbit temp7 = temp^7;

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

uint8_t mem_test_march_C2_bit8 (uint8_t startaddr, uint8_t length, uint8_t value_0,uint8_t value_1)
{
	uint32_t i;
	uint8_t bdata *ramaddr;						//根据测试的RAM区选择
//	uint8_t idata *ramaddr;
//	uint8_t data *ramaddr;
//	uint8_t xdata *ramaddr;
	
	ramaddr = (uint8_t *)startaddr;
	// step 0: W0
	for(i=0; i<length; i++)
	{
		ramaddr[i] = value_0;
	}


	// step 1: R0->W1->R1  ,address ++
	for(i=0; i<length; i++)
	{
		if( ramaddr[i] != value_0)	
		{
			return 1;   
		}   
		ramaddr[i] = value_1;	     
		if( ramaddr[i] != value_1)	 
		{
			return 2;   
		} 
	}

	// step 2: R1->W0->R0  ,address ++
	for(i=0; i<length; i++)
	{
		
		if( ramaddr[i] != value_1)	
		{
			return 3;   
		}   
		ramaddr[i] = value_0;	     
		if( ramaddr[i] != value_0)	 
		{
			return 4;   
		} 
	}

	// step 3: R0->W1->R1  ,address --
	for(i=length; i>0; i--)
	{
		i =i-1;
		if( ramaddr[i] != value_0)	
		{
			return 5;   
		}   
		ramaddr[i] = value_1;	     
		if( ramaddr[i] != value_1)	 
		{
			return 6;   
		} 
	}

	// step 4: R1->W0->R0  ,address --
	for(i=length; i>0; i--)
	{
		i =i-1;
		if( ramaddr[i] != value_1)	
		{
			return 7;   
		}   
		ramaddr[i] = value_0;	     
		if( ramaddr[i] != value_0)	 
		{
			return 8;   
		} 
	}

	// step 5: R0 
	for(i=0; i<length; i++)
	{
		if(ramaddr[i]!=value_0)
		{
			return 9;   
		}
	}

	return 0;	 
}

void iram_bdata_bit_test(void)
{	
	temp0 = 1;
	temp1 = 0;
	temp2 = 1;
	temp3 = 0;
	temp4 = 0;
	temp5 = 1;
	temp6 = 0;
	temp7 = 1;

	printfS("temp = %bx\r\n",*(volatile uint8_t bdata*)(0x2F));
}

void iram_bdata_test(void)
{
#ifdef march_test
	uint8_t xdata result;
	printfS("bdata march test start\r\n");
	result = mem_test_march_C2_bit8(0x20,16,0x55,0xaa);				//初始地址20H  空间大小16Bytes 
	if(result)
	{
		printfS("bdata march test fail\r\n");
	}
	else
	{
		printfS("bdata march test success\r\n");
	}
	
	
#else
	uint8_t xdata i;
	uint8_t xdata bdata_error_flag = 0;

	//8bit write
	for(i=0 ;i<16 ;i++)
	{
		*(volatile uint8_t bdata*)(0x20+i) = 0x55;	
	}
	//8bit read
	for(i=0 ;i<16 ;i++)
	{
		if(*(volatile uint8_t bdata*)(0x20+i) != 0x55)
		{
			bdata_error_flag = 1;
			printfS("bdata 8bits write/8bits read error: addr=0x%bx, data=0x%bx\r\n",(0x20+i),*(volatile uint8_t bdata*)(0x20+i));
			break;
		}
	}
	
	//16bit write
	for(i=0 ;i<8 ;i++)
	{
		*(volatile uint16_t bdata*)(0x20+i*2) = 0x55aa;	
	}
	//16bit read
	for(i=0 ;i<8 ;i++)
	{
		if(*(volatile uint16_t bdata*)(0x20+i*2) != 0x55aa)
		{
			bdata_error_flag = 1;
			printfS("bdata 16bits write/16bits read error: addr=0x%x, data=0x%x\r\n",(0x20+i*2),*(volatile uint16_t bdata*)(0x20+i*2));
			break;
		}
	}
	
	//32bit write
	for(i=0 ;i<4 ;i++)
	{
		*(volatile uint32_t bdata*)(0x20+i*4) = 0x55aa1234;	
	}
	//32bit read
	for(i=0 ;i<4 ;i++)
	{
		if(*(volatile uint32_t bdata*)(0x20+i*4) != 0x55aa1234)
		{
			bdata_error_flag = 1;
			printfS("bdata 32bits write/32bits read error: addr=0x%lx, data=0x%lx\r\n",(0x20+i*4),*(volatile uint32_t bdata*)(0x20+i*4));
			break;
		}
	}
	
	if(bdata_error_flag)
	{
		printfS("bdata 20H-2FH write&read fail\r\n");
	}
	else
	{
		printfS("bdata 20H-2FH write&read success\r\n");
	}
#endif
}

void iram_idata_test(void)
{
#define idata_test
	
#ifdef march_test
	uint8_t xdata result;
	result = mem_test_march_C2_bit8(0x40,192,0x55,0xaa);		//预留了堆栈空间 初始地址40H  空间大小192Bytes 
	if(result)
	{
		printfS("idata march test fail\r\n");
	}
	else
	{
		printfS("idata march test success\r\n");
	}

#else
	uint8_t xdata i;
	uint8_t xdata idata_error_flag = 0;
	//8bit write
	for(i=0 ;i<192 ;i++)
	{
		*(volatile uint8_t idata*)(0x40+i) = 0x55;	
	}
	//8bit read
	for(i=0 ;i<192 ;i++)
	{
		if(*(volatile uint8_t idata*)(0x40+i) != 0x55)
		{
			idata_error_flag = 1;
			printfS("iram 8bits write/8bits read error: addr=0x%bx, data=0x%bx\r\n",(0x40+i),*(volatile uint8_t idata*)(0x40+i));
			break;
		}
	}
	
	//16bit write
	for(i=0 ;i<96 ;i++)
	{
		*(volatile uint16_t idata*)(0x40+i*2) = 0x55aa;	
	}
	//16bit read
	for(i=0 ;i<96 ;i++)
	{
		if(*(volatile uint16_t idata*)(0x40+i*2) != 0x55aa)
		{
			idata_error_flag = 1;
			printfS("iram 16bits write/16bits read error: addr=0x%x, data=0x%x\r\n",(0x40+i*2),*(volatile uint16_t idata*)(0x40+i*2));
			break;
		}
	}
	
	//32bit write
	for(i=0 ;i<48 ;i++)
	{
		*(volatile uint32_t idata*)(0x40+i*4) = 0x55aa1234;	
	}
	//32bit read
	for(i=0 ;i<48 ;i++)
	{
		if(*(volatile uint32_t idata*)(0x40+i*4) != 0x55aa1234)
		{
			idata_error_flag = 1;
			printfS("iram 32bits write/32bits read error: addr=0x%.8x, data=0x%.8x\r\n",(0x40+i*4),*(volatile uint32_t idata*)(0x40+i*4));
			break;
		}
	}

	if(idata_error_flag)
	{
		printfS("idata 40H-FFH write&read fail\r\n");
	}
	else
	{
		printfS("idata 40H-FFH write&read success\r\n");
	}
#endif

}

void iram_data_test(void)
{
#define data_test
#ifdef march_test
	uint8_t xdata result;
	result = mem_test_march_C2_bit8(0x40,64,0x55,0xaa);		//预留了堆栈空间 初始地址40H  空间大小64Bytes 
	if(result)
	{
		printfS("data march test fail\r\n");
	}
	else
	{
		printfS("data march test success\r\n");
	}

#else
	uint8_t xdata i;
	uint8_t xdata idata_error_flag = 0;
	// 8bit write
	for(i=0 ;i<64 ;i++)
	{
		*(volatile uint8_t data*)(0x40+i) = 0x55;	
	}
	// 8bit read
	for(i=0 ;i<64 ;i++)
	{
		if(*(volatile uint8_t data*)(0x40+i) != 0x55)
		{
			idata_error_flag = 1;
			printfS("iram 8bits write/8bits read error: addr=0x%bx, data=0x%bx\r\n",(0x40+i),*(volatile uint8_t data*)(0x40+i));
			break;
		}
	}
	
	//16bit write
	for(i=0 ;i<32 ;i++)
	{
		*(volatile uint16_t data*)(0x40+i*2) = 0x55aa;	
	}
	//16bit read
	for(i=0 ;i<32 ;i++)
	{
		if(*(volatile uint16_t data*)(0x40+i*2) != 0x55aa)
		{
			idata_error_flag = 1;
			printfS("iram 16bits write/16bits read error: addr=0x%x, data=0x%x\r\n",(0x40+i*2),*(volatile uint16_t data*)(0x40+i*2));
			break;
		}
	}
	
	//32bit write
	for(i=0 ;i<16 ;i++)
	{
		*(volatile uint32_t data*)(0x40+i*4) = 0x55aa1234;	
	}
	//32bit read
	for(i=0 ;i<16 ;i++)
	{
		if(*(volatile uint32_t data*)(0x40+i*4) != 0x55aa1234)
		{
			idata_error_flag = 1;
			printfS("iram 32bits write/32bits read error: addr=0x%lx, data=0x%lx\r\n",(0x40+i*4),*(volatile uint32_t data*)(0x40+i*4));
			break;
		}
	}

	if(idata_error_flag)
	{
		printfS("data 40H-7FH write&read fail\r\n");
	}
	else
	{
		printfS("data 40H-7FH write&read success\r\n");
	}
#endif

}

void xram_xdata_test(void)
{
#define xdata_test
	
#ifdef march_test
	uint8_t idata result;
	result = mem_test_march_C2_bit8(0x00,2048,0x55,0xaa);		//初始地址00H  空间大小2048Bytes 
	if(result)
	{
		printfS("xdata march test fail\r\n");
	}
	else
	{
		printfS("xdata march test success\r\n");
	}

#else
	uint32_t idata i;
	uint8_t idata xdata_error_flag = 0;
	//8bit write
	for(i=0 ;i<2048; i++)
	{
		*(volatile uint8_t xdata*)(0x00+i) = 0x5a;	
	}
	//8bit read
	for(i=0 ;i<2048; i++)
	{
		if(*(volatile uint8_t xdata*)(0x00+i) != 0x5a)
		{
			xdata_error_flag = 1;
			printfS("xram 8bits write/8bits read error: addr=0x%bx, data=0x%bx\r\n",(0x00+i),*(volatile uint8_t xdata*)(0x00+i));
			break;
		}		
	}
	
	//16bit write
	for(i=0 ;i<1024; i++)
	{
		*(volatile uint16_t xdata*)(0x00+i*2) = 0x5aa5;	
	}
	//16bit read
	for(i=0 ;i<1024; i++)
	{
		if(*(volatile uint16_t xdata*)(0x00+i*2) != 0x5aa5)
		{
			xdata_error_flag = 1;
			printfS("xram 16bits write/16bits read error: addr=0x%x, data=0x%x\r\n",(0x00+i*2),*(volatile uint16_t xdata*)(0x00+i*2));
			break;
		}		
	}
	
	//32bit write
	for(i=0 ;i<512; i++)
	{
		*(volatile uint32_t xdata*)(0x00+i*4) = 0x5aa51234;	
	}
	//32bit read
	for(i=0 ;i<512; i++)
	{
		if(*(volatile uint32_t xdata*)(0x00+i*4) != 0x5aa51234)
		{
			xdata_error_flag = 1;
			printfS("xram 32bits write/32bits read error: addr=0x%.8x, data=0x%.8x\r\n",(0x00+i*4),*(volatile uint32_t xdata*)(0x00+i*4));
			break;
		}		
	}

	
	if(xdata_error_flag)
	{
		printfS("xdata 00H-7FFH write&read fail\r\n");
	}
	else
	{
		printfS("xdata 00H-7FFH write&read success\r\n");
	}
	
#endif

}

void ram_test(void)
{
	iram_bdata_test();				//bdata位寻址区20H-2FH测试，堆栈不在此区开辟，可写此区全部地址
//	iram_idata_test();				//idata片内间接寻址区40H-FFH测试，堆栈在30H-80H区域开辟，预留了30H-40H空间用作堆栈，读写剩余40H-FFH空间
//	iram_data_test();				//data片内直接寻址区测试40H-7FH测试,堆栈在30H-80H区域开辟，预留了30H-40H空间用作堆栈，读写剩余40H-7FH空间
//	xram_xdata_test();				//XRAM区2KB 读写测试，初始化变量和堆栈放在IRAM区，则可写XRAM全部空间
	iram_bdata_bit_test();			//位寻址区位操作测试
}

void soc_test(void)
{
	ram_test();
}
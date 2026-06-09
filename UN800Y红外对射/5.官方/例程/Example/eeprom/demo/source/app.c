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
#include "eeprom.h"
#include "config.h"

volatile uint8_t rx_flag ;
volatile uint8_t uart0_rx_buf[32];
volatile uint8_t uart0_tx_buf[32];
volatile uint16_t rx_count = 0;
volatile uint16_t tx_count = 0;

volatile uint8_t  write_data[20]={0};
volatile uint8_t  read_data[20]={0};

//volatile const uint8_t code buf[58368];
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

void eeprom_page_check_8bits(uint16_t base_addr,uint8_t w_val, uint8_t page)
{
    uint16_t i;
    
    for(i = 0; i < PAGE_SIZE; i++)
    {
        eeprom_write_byte(base_addr+page*PAGE_SIZE+i,w_val);
    }
    
    for(i = 0; i < PAGE_SIZE; i++)
    {
        if(eeprom_read_byte(base_addr+page*PAGE_SIZE+i) != w_val)
        {
            printf("Error: 8bit Read not equal write![0x%x]\n",(base_addr+page*PAGE_SIZE+i));
            break;
        }
		//  printf("read_data:[0x%bx]\n",eflash_read_byte(base_addr+page*PAGE_SIZE+i));
    }

}



void eeprom_page_check_byte(uint16_t base_addr, uint8_t start_page, uint8_t num_page)
{
    uint16_t i;
    uint8_t page;

    //单字节读写测试
	
    for(page = start_page; page < num_page; page++)
    {
        eeprom_erase_page(base_addr + page * PAGE_SIZE);								//页擦除
        for(i = 0; i < PAGE_SIZE; i++)
        {
            if(eeprom_read_byte(base_addr+page*PAGE_SIZE+i) != 0xff)
            {
                    printf("Error: erase fail![0x%lx]\n",(base_addr+page*PAGE_SIZE+i));
                    break;
            }
        }	
        eeprom_page_check_8bits(base_addr,0x55,page);                                   //单字节读写
    }


}	

void eeprom_page_check_bytes(uint16_t base_addr,uint8_t w_val, uint32_t length)
{
    uint32_t i;
 
    //多字节读写测试
    eeprom_erase_page(base_addr);								//页擦除
   
    for(i = 0; i < length; i++)
    {
        if(eeprom_read_byte(base_addr+i) != 0xff)
        {
                printf("Error: erase fail![0x%lx]\n",(base_addr+i));
                break;
        }
    }	
   
	
	
    for(i=0;i<length;i++)
    {
        write_data[i] = w_val;                          
    }
    
	
    eeprom_write_bytes(base_addr, write_data, length);                       //多字节写数据
        
     
   if(length == eeprom_read_bytes(base_addr, read_data, length))             //多字节读    
   {
        for(i=0;i<length;i++)
        {
            if(read_data[i] != w_val)
            {
                printf("Error: multibyte Read not equal Write![0x%x]\n",(base_addr+i));
                break;
            } 
//           printf("read_data:[0x%bx]\n",read_data[i]);
        }  
   
   }
   
	        
}


void eeprom_test(void)
{
	eeprom_init(FCLK);
	
	/********MAIN区擦读写测试*********/
    //单字节读写测试
	printfS("EEPROM write and read by byte test start\r\n");
	
	eeprom_page_check_byte(0x8A00,0,1);				//单字节读写EEPROM0
	eeprom_page_check_byte(0x8C00,0,1);             //单字节读写EEPROM1
	
	printfS("EEPROM write and read by byte test end\r\n"); 

	delay1ms(10);
	
    //多字节读写测试
    printfS("EEPROM write and read by multi bytes test start\r\n");
	
	eeprom_page_check_bytes(0x8A00,0xa5,20);        //多字节读写EEPROM0
	eeprom_page_check_bytes(0x8C00,0xc5,20);		//字节读写EEPROM1
	
	printfS("EEPROM write and read by multi bytes test end\r\n");
	 
	
}







/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : app.c
 * Description : app driver source file
 * Author(s)   : Dan
 * version     : V1.0
 * Modify date : 2020-07-16
 ***********************************************************************/
#include  "app.h"
#include  "uart0.h"
#include "config.h"
void gpio_enable_interrupt(void)
{
	EA = 0x1;				//EA总中断开启
  EX0 = 0x1;             //外部中断使能
  P0IEN |= (0x1<<4);     //使能P04端口中断

}

void GPIO_IRQHandler(void) interrupt 0
{
    if((P0IRQ&0x10)==0x10)
    {    
        P0IRQ &= ~(0x1<<4);		
				
    }

}


void lowpower_test(void)
{
	
	uint8_t chs = 1;
	
	switch(chs)
	{ 
		case 0:             //SLEEP mode,wake up by GPIO inerrupt
		printfS("SLEEP  MODE------Enter SLEEP mode------Wake up by P04!\n");
	
		POREN |= (1<<0);				//关闭LVR
	  LVDCON |= (1<<4);				//关闭LVD
		
		CLKCON &= ~(0x1<<3);				//RCL时钟关闭
		
		REG_P0_IE = 0x14; 					//输入禁止(除P04、NRST)
		REG_P1_IE = 0x00;					//输入禁止	
		REG_P2_IE = 0x00;					//输入禁止		

		PCLK0 = 0x00;					//关闭所有模块时钟
		PCLK1 = 0x01;					//关闭除唤醒管脚IO之外所有模块时钟
		
		gpio_enable_interrupt();		//P04中断使能
	
		PCON |= (0x1<<0);   //enter SLEEP mode
		
	
		uart0_init(UART0_BAUD_RATE);	//重新打开UART0，通过打印信息判断是否唤醒
		printfS("SLEEP MODE------Exit SLEEP mode------Pass!\n");	
		break;
	case 1:             //Deepsleep  mode，wake up by GPIO(rising edge or falling edge ) inerrupt or Timer
		printfS("Deepsleep MODE------Enter Deepsleep mode------Wake up by P04!\n");
		
		POREN |= (1<<0);				//关闭LVR
	    LVDCON |= (1<<4);				//关闭LVD
	
		REG_P0_IE = 0x14; 					//输入禁止(除P04、NRST)
		REG_P1_IE = 0x00;					//输入禁止	
		REG_P2_IE = 0x00;					//输入禁止		
			
		gpio_enable_interrupt();		//P04中断使能
	
		PDSEL &= ~(1<<0); 
		PCON |= (0x1<<1);   			//enter stop mode
	
		uart0_init(UART0_BAUD_RATE);	//重新打开UART0，通过打印信息判断是否唤醒
		printfS("Deepsleep MODE------Exit Deepsleep mode------Pass!\n");
	
		break;
	case 2:             //powerdown mode，wake up by GPIO(rising edge or falling edge ) inerrupt
		printfS("POWERDOWN MODE------Enter powerdown mode------Wake up by P04!\n");
		
		POREN |= (1<<0);				//关闭LVR
		LVDCON |= (1<<4);				//关闭LVD

	
		REG_P0_IE = 0x14;  				//输入禁止(除P04、NRST)
		REG_P1_IE = 0x00;					//输入禁止	
		REG_P2_IE = 0x00;					//输入禁止		
		
	
		gpio_enable_interrupt();		//P04中断使能
	
		PDSEL |= (1<<0); 
		PCON |= (0x1<<1);  				//enter powerdown mode

		uart0_init(UART0_BAUD_RATE);	//重新打开UART0，通过打印信息判断是否唤醒
		printfS("POWERDOWN MODE------Exit powerdown mode------Pass!\n");
		break;
	default:
			break;
	
    }        
    
}

	
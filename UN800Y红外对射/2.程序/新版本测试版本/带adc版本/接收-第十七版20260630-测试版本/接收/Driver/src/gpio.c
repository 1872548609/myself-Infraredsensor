/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : gpio.c
 * Description : gpio source file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2021-2-26
 ***********************************************************************/
#include "gpio.h"

void (*gpiofunc[24])(void) = { 0 };
/************************************************************************
 * function   : GPIO_IRQHandler
 * Description: GPIO interrupt handling
 * input : none
 * return: none
 ************************************************************************/
//void GPIO_IRQHandler(void) interrupt 0
//{
//  uint8_t i=0;
//  for(i=0;i<24;i++)
//	{
//    if(gpio_irq_get(i) == 0x01)
//    {
//      if(gpiofunc[i] != NULL)
//			{
//				gpiofunc[i]();
//			}
//				gpio_irq_clr(i);
//    }		
//  }
//   
//}

/************************************************************************
 * function   : gpio_init
 * Description : gpio initial 
 * GPIO初始化，包括开时钟，模块正常工作
 * input : uint8_t pin ： P00，P01，P02	...
 ************************************************************************/
void gpio_init(uint8_t pin)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
    //开启GPIO时钟，GPIO模块正常工作
	if(gpio_pin == PORT0)
	{
		PCLK1 |= (1<<0); 		      
	  PRESET1 |= (1<<0);         
	}
	else if(gpio_pin == PORT1)
	{
		PCLK1 |= (1<<1); 		     
	    PRESET1 |= (1<<1);         
	}
	else if(gpio_pin == PORT2)
	{
		PCLK1 |= (1<<2); 		      
		PRESET1 |= (1<<2);        
	}
}

/************************************************************************
 * function   : gpio_dir_set
 * Description: set direction of gpio pin
 * GPIO管脚数据流方向配置
 * input :uint8_t pin  ： P00，P01，P02	...
          uint8_t dir  ： GPIO_DIR_OUT：输出；GPIO_DIR_IN：输入
 ************************************************************************/
void gpio_dir_set(uint8_t pin, uint8_t dir)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if(dir)                //输入
	{		
    if(gpio_pin == PORT0)    
		{
			P0OEN |= (1<<gpio_num);
		}
		else if(gpio_pin == PORT1)    
		{
			P1OEN |= (1<<gpio_num);
		}	
		else if(gpio_pin == PORT2)
		{
			P2OEN |= (1<<gpio_num);
		}
		
	}
	else	                  //输出
	{	
    if(gpio_pin == PORT0)    
		{
			P0OEN &= (~(1<<gpio_num));
		}
		else if(gpio_pin == PORT1)    
		{
			P1OEN &= (~(1<<gpio_num));
		}	
		else if(gpio_pin == PORT2)
		{
			P2OEN &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function   : gpio_in_enable
 * Description: gpio in enable
 * input :  uint8_t pin  ： P00，P01，P02	...
 *			uint8_t mode ： IN_ENABLE：输入使能；IN_DISABLE：输入禁止
 ************************************************************************/
void gpio_in_enable(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;

	if(mode)       //输入使能
	{
		if(gpio_pin == PORT0)    
		{
			REG_P0_IE |= (1<<gpio_num);
		}
		else if(gpio_pin == PORT1)    
		{
			REG_P1_IE |= (1<<gpio_num);
		}	
		else if(gpio_pin == PORT2)
		{
			REG_P2_IE |= (1<<gpio_num);
		}		

	}
	else             //输入禁止
	{
		if(gpio_pin == PORT0)    
		{
			REG_P0_IE &= (~(1<<gpio_num));
		}
		else if(gpio_pin == PORT1)    
		{
			REG_P1_IE &= (~(1<<gpio_num));
		}	
		else if(gpio_pin == PORT2)
		{
			REG_P2_IE &= (~(1<<gpio_num));
		}

	}
}

/************************************************************************
 * function    : gpio_pu_set
 * Description : set pullup function
 * GPIO管脚上拉配置
 * input :uint8_t pin  ： P00，P01，P02	...
 *        uint8_t mode ： GPIO_PU_ENABLE：上拉使能; GPIO_PU_DISABLE：上拉禁止
 ************************************************************************/
void gpio_pu_set(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if (mode)       //上拉禁止
	{
		if (gpio_pin == PORT0)    
		{
			P0PU |= (1<<gpio_num);
		}
		else if (gpio_pin == PORT1)    
		{
			P1PU |= (1<<gpio_num);
		}	
		else if (gpio_pin == PORT2)
		{
			P2PU |= (1<<gpio_num);
		}		
	}
	else	           //上拉使能
	{	
    if (gpio_pin == PORT0)    
		{
			P0PU &= (~(1<<gpio_num));
		}
		else if (gpio_pin == PORT1)    
		{
			P1PU &= (~(1<<gpio_num));
		}	
		else if (gpio_pin == PORT2)
		{
			P2PU &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function    : gpio_pd_set
 * Description : set pulldown function 
 * GPIO管脚下拉配置
 * input :uint8_t pin  ： P00，P01，P02	...
 *        uint8_t mode ： GPIO_PD_ENABLE：下拉使能; GPIO_PD_DISABLE：下拉禁止
 ************************************************************************/
void gpio_pd_set(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if (mode)       //下拉使能
	{
		if(gpio_pin == PORT0)    
		{
			P0PD |= (1<<gpio_num);
		}
		else if (gpio_pin == PORT1)    
		{
			P1PD |= (1<<gpio_num);
		}	
		else if (gpio_pin == PORT2)
		{
			P2PD |= (1<<gpio_num);
		}	
	}
	else	           //下拉禁止
	{	
		if(gpio_pin == PORT0)    
		{
			P0PD &= (~(1<<gpio_num));
		}
		else if (gpio_pin == PORT1)    
		{
			P1PD &= (~(1<<gpio_num));
		}	
		else if (gpio_pin == PORT2)
		{
			P2PD &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function    : gpio_od_set
 * Description : set gpio output open drain
 * GPIO管脚开漏输出配置
 * input :uint8_t pin  ： P00，P01，P02	...
 *        uint8_t mode ： GPIO_OD_ENABLE：开漏使能; GPIO_OD_DISABLE：开漏禁止
 ************************************************************************/
void gpio_od_set(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if (mode)       //开漏使能
	{
		if(gpio_pin == PORT0)    
		{
			P0OD |= (1<<gpio_num);
		}
		else if (gpio_pin == PORT1)    
		{
			P1OD |= (1<<gpio_num);
		}	
		else if (gpio_pin == PORT2)
		{
			P2OD |= (1<<gpio_num);
		}	
	}
	else	           //开漏禁止
	{	
		if(gpio_pin == PORT0)    
		{
			P0OD &= (~(1<<gpio_num));
		}
		else if (gpio_pin == PORT1)    
		{
			P1OD &= (~(1<<gpio_num));
		}	
		else if (gpio_pin == PORT2)
		{
			P2OD &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function    : gpio_cs_set
 * Description : set gpio input type
 * GPIO管脚输入类型配置
 * input :uint8_t pin  ： P00，P01，P02	...
 *        uint8_t mode ： GPIO_CS_CMOS：CMOS input buffer; 
                          GPIO_CS_SCHMITT：Schmitt input buffer 
 ************************************************************************/
void gpio_cs_set(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if(mode)       //CMOS input buffer
	{
		if(gpio_pin == PORT0)    
		{
			P0CS |= (1<<gpio_num);
		}
		else if (gpio_pin == PORT1)    
		{
			P1CS |= (1<<gpio_num);
		}	
		else if (gpio_pin == PORT2)
		{
			P2CS |= (1<<gpio_num);
		}	
	}
	else	           //Schmitt input buffer 
	{	
		if(gpio_pin == PORT0)    
		{
			P0CS &= (~(1<<gpio_num));
		}
		else if (gpio_pin == PORT1)    
		{
			P1CS &= (~(1<<gpio_num));
		}	
		else if (gpio_pin == PORT2)
		{
			P2CS &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function    : gpio_dr_set
 * Description : set gpio driving power
 * GPIO管脚驱动能力配置
 * input :uint8_t pin  ： P00，P01，P02	...
 *        uint8_t mode ： GPIO_DR_HIGH：高驱动能力 ; GPIO_DR_LOW：低驱动能力
 ************************************************************************/
void gpio_dr_set(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if(mode)       //低驱动能力
	{
		if(gpio_pin == PORT0)    
		{
			P0DR |= (1<<gpio_num);
		}
		else if(gpio_pin == PORT1)    
		{
			P1DR |= (1<<gpio_num);
		}	
		else if(gpio_pin == PORT2)
		{
			P2DR |= (1<<gpio_num);
		}	
	}
	else	           //高驱动能力 
	{	
		if(gpio_pin == PORT0)    
		{
			P0DR &= (~(1<<gpio_num));
		}
		else if(gpio_pin == PORT1)    
		{
			P1DR &= (~(1<<gpio_num));
		}	
		else if(gpio_pin == PORT2)
		{
			P2DR &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function    : gpio_sr_set
 * Description : set gpio speed
 * GPIO管脚速度配置
 * input :uint8_t pin  ： P00，P01，P02	...
 *        uint8_t mode ： GPIO_SR_HIGH：快速 ; GPIO_SR_LOW：慢速
 ************************************************************************/
void gpio_sr_set(uint8_t pin, uint8_t mode)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if (mode)       //慢速
	{
		if(gpio_pin == PORT0)    
		{
			REG_P0_SR |= (1<<gpio_num);
		}
		else if(gpio_pin == PORT1)    
		{
			REG_P1_SR |= (1<<gpio_num);
		}	
		else if(gpio_pin == PORT2)
		{
			REG_P2_SR |= (1<<gpio_num);
		}	
	}
	else	           //快速 
	{	
		if(gpio_pin == PORT0)    
		{
			REG_P0_SR &= (~(1<<gpio_num));
		}
		else if(gpio_pin == PORT1)    
		{
			REG_P1_SR &= (~(1<<gpio_num));
		}	
		else if(gpio_pin == PORT2)
		{
			REG_P2_SR &= (~(1<<gpio_num));
		}	
	}	
}

/************************************************************************
 * function    : gpio_io_get
 * Description : get gpio pin value
 * GPIO输入电平获取
 * input : uint8_t pin  :  P00，P01，P02...
 * return: 管脚电平状态
 ************************************************************************/
uint8_t gpio_io_get(uint8_t pin)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
  if(gpio_pin == PORT0)    
	{
    return ((P0 >> gpio_num) & 0x01);	
	}
	else if (gpio_pin == PORT1)    
	{
    return ((P1 >> gpio_num) & 0x01);			
	}	
  else
	{
    return ((P2 >> gpio_num) & 0x01);				
	}

}


/************************************************************************
 * function    : gpio_io_set
 * Description : gpio set 1 or 0
 * GPIO电平输出
 * input : uint8_t pin   :  P00，P01，P02...
 *		   uint8_t level :  GPIO_HIGH：1; GPIO_LOW：0
 ************************************************************************/
void gpio_io_set(uint8_t pin, uint8_t level)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if(level)         //输出高电平
	{
		if (gpio_pin == PORT0)    
		{
			P0 |= 1<< gpio_num;
		}
		else if (gpio_pin == PORT1)    
		{
			P1 |= 1<< gpio_num;		
		}	
		else if (gpio_pin == PORT2)
		{
			P2 |= 1<< gpio_num;		
		}
	}
	else              //输出低电平
	{
		if (gpio_pin == PORT0)    
		{
			P0 &= (~(1<<gpio_num));
		}
		else if (gpio_pin == PORT1)    
		{
			P1 &= (~(1<<gpio_num));		
		}	
		else if (gpio_pin == PORT2)
		{
			P2 &= (~(1<<gpio_num));		
		}
	}
}

/************************************************************************
 * function   : gpio_irq_set
 * Description: gpio interrupt configuration
 * GPIO中断使能，失能
 * input : uint8_t pin  :  P00，P01，P02...
 *		   uint8_t mode :  GPIO_IRQ_ENABLE: 使能; GPIO_IRQ_DISABLE：失能
           void (*pfunc)() :中断处理回调函数
 ************************************************************************/
void gpio_irq_set(uint8_t pin, uint8_t mode, void (*pfunc)())
{
  uint8_t gpio_pin = pin/8;
	
	uint8_t gpio_num = pin%8;
    
	if(mode)            //中断使能
	{
		gpiofunc[pin] = pfunc;
		
		EA = 1;         //总中断开启
		EX0 = 1;        //外部中断初级使能开启
		gpio_irq_clr(pin);      //清除GPIO端口中断状态
		if (gpio_pin == PORT0)    
		{
			P0IEN |= (1<<gpio_num);
		}
		else if (gpio_pin == PORT1)    
		{
			P1IEN |= (1<<gpio_num);
		}	
		else if (gpio_pin == PORT2)
		{
			P2IEN |= (1<<gpio_num);
		}	
	}
	else               //中断失能
	{ 
		EX0 = 0;        //外部中断初级使能关闭
		if (gpio_pin == PORT0)    
		{
			P0IEN &= (~(1<<gpio_num));
		}
		else if (gpio_pin == PORT1)    
		{
			P1IEN &= (~(1<<gpio_num));
		}	
		else if (gpio_pin == PORT2)
		{
			P2IEN &= (~(1<<gpio_num));
		}
	}	
	
}




/************************************************************************
 * function   : gpio_irq_clr
 * Description: clear the interrupt flag of gpio pin
 * GPIO中断清除
 * input : uint8_t pin : P00，P01，P02	...
 * return: none
 ************************************************************************/
void gpio_irq_clr(uint8_t pin)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
	if(gpio_pin == PORT0)        //写0清除
	{
		P0IRQ &= (~(1<<gpio_num));
	}
	else if(gpio_pin == PORT1)    
	{
		P1IRQ &= (~(1<<gpio_num));		
	}	
	else if(gpio_pin == PORT2)
	{
		P2IRQ &= (~(1<<gpio_num));		
	}		
}

/************************************************************************
 * function   : gpio_irq_get
 * Description: get the interrupt flag of gpio pin
 * GPIO中断状态获取
 * input : uint8_t pin : P00，P01，P02	...
 * return: 管脚中断状态
 ************************************************************************/
uint8_t gpio_irq_get(uint8_t pin)
{
  uint8_t gpio_pin = pin/8;
	uint8_t gpio_num = pin%8;
    
  if(gpio_pin == PORT0)    
	{
    return ( P0IRQ >> gpio_num) & 0x01;	
	}
	else if(gpio_pin == PORT1)    
	{
    return ( P1IRQ >> gpio_num) & 0x01;			
	}	
  else
	{
    return ( P2IRQ >> gpio_num) & 0x01;				
	}
}



/***********************************************************************
 * Copyright (c)  2019, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : um_lptimer.c
 * Description : lpTimer driver source file
 * Author(s)   : wanyi
 * version     : V1.0
 * Modify date : 2020-09-04
 ***********************************************************************/
#include "lptimer.h"

void (*lptimfunc[4])(void) = { 0 };
/************************************************************************
 * function   : lptimer_IRQHandler
 * Description: lptimer interrupt handler
 * input : none
 * output: none   
 * return: none
 ************************************************************************/
void LPTIMER_IRQHandler(void) interrupt 12
{ 
	if(((REG_LPTIM_LPTIF & 0x01) == 0x01)&&((REG_LPTIM_LPTIE & 0x01) == 0x01))		   //匹配中断 
	{
		REG_LPTIM_LPTIF = 0x01;                                                       //清中断标志 
        
        if(lptimfunc[0]!= NULL)
        {
            ((void(*)())(lptimfunc)[0])();                                           //lptim匹配中断回调函数 
        }
	}
  else if(((REG_LPTIM_LPTIF & 0x02) == 0x02)&&((REG_LPTIM_LPTIE & 0x02) == 0x02))	   //溢出中断 
	{
		REG_LPTIM_LPTIF = 0x02;                                                       //清中断标志  
  
        if(lptimfunc[1]!= NULL)
        {
            ((void(*)())(lptimfunc)[1])();                                            //lptim溢出中断回调函数
        }
	}
    else if(((REG_LPTIM_LPTIF & 0x04) == 0x04)&&((REG_LPTIM_LPTIE & 0x04) == 0x04))	  //外部Trigger触发中断 
	{
		REG_LPTIM_LPTIF = 0x04;	                                                      //清中断标志  
        
        if(lptimfunc[2]!= NULL)
        {
            ((void(*)())(lptimfunc)[2])();                                            //lptim外部Trigger触发中断回调函数 
        }   
	}
	else if(((REG_LPTIM_LPTIF & 0x08) == 0x08)&&((REG_LPTIM_LPTIE & 0x08) == 0x08))	   //外部Trigger触发中断 
	{
		REG_LPTIM_LPTIF = 0x08;	                                                       //清中断标志  
        
        if(lptimfunc[3]!= NULL)
        {
            ((void(*)())(lptimfunc)[3])();                                             //lptim外部Trigger触发中断回调函数 
        }   
	}
}
/*******************************************************************
* Function		: lptimer_init
* Description	: lptimer 初始化
* Input			: tmode：  0 普通定时器模式
*                          1 Trigger脉冲触发计数模式
*                          2 外部异步脉冲计数模式
*                          3 Timeout模式                        
* Output		: none
* Return		: none
********************************************************************/
void lptimer_init(uint8_t tmode, uint8_t clock_sel, uint8_t div)
{
  PCLK0 |=(1<<6); 									    //开启lpTimer时钟使能 
  PRESET0 |= (1<<6);									    // lpTimer正常工作 
    
	REG_LPTIM_LPTCFG0 = (REG_LPTIM_LPTCFG0&(~(3<<3)))|(clock_sel<<3);	// 时钟源设置 0:RCL低速时钟, 1:clk_1hz时钟,  2:系统时钟, 3:外部输入时钟 
  REG_LPTIM_LPTCFG0 = (REG_LPTIM_LPTCFG0&(~(7<<0)))|(div<<0);				//设置计数时钟分频 				
  REG_LPTIM_LPTCFG1 =  (REG_LPTIM_LPTCFG1&(~(3<<3)))|(tmode<<3);     //设置工作模式
    
	REG_LPTIM_LPTCFG1 &= ~(1<<2);								
    
  REG_LPTIM_LPTCFG1 |= (CONTINUOUS_MODE<<2);              //连续计数模式 
	
}

/*******************************************************************
* Function		: lptimer_irq_init
* Description	: lptimer 中断初始化
* Input			: irqstate：0  中断失能
*                           1  中断使能
*                 ie: 0 比较匹配中断使能 
*                     1 计数器溢出中断使能
*                     2 外部触发到来中断使能
*                 void (*pfunc)() ：中断回调函数  
* Output		: none
* Return		: none
********************************************************************/
void lptimer_irq_init(uint8_t irqstate,uint8_t ie,void (*pfunc)())
{
    if(irqstate == LPTIMER_IRQ_ENABLE)
    {
        EA = 1;				                                     //EA总中断开启 
        IEN1 |= (1<<4);                                    //lptimer中断开启  
        
        switch(ie)
        {
            case 0:        
                    REG_LPTIM_LPTIE |= (1<<0);             //比较匹配中断使能
                    lptimfunc[0] = pfunc;
                    break;
            case 1:
                    REG_LPTIM_LPTIE |= (1<<1);              //计数器溢出中断使能
                    lptimfunc[1] = pfunc;
                    break;           
            case 2:
                    REG_LPTIM_LPTIE |= (1<<2);           //外部触发到来中断使能
                    lptimfunc[2] = pfunc;
                    break;   
			case 3:        
                    REG_LPTIM_LPTIE |= (1<<3);              //比较匹配中断使能
                    lptimfunc[3] = pfunc;
                    break;
			
            default:
                    break;
        }
    }
    else
    {
      //  EA = 0;				                                      //EA总中断关闭   
        IEN1 &= ~(1<<4);                                      // lptimer中断关闭   

        switch(ie)
        {
            case 0:        
                    REG_LPTIM_LPTIE &= ~(1<<0);                 //比较匹配中断失能
                    break;
            case 1:
                    REG_LPTIM_LPTIE &= ~(1<<1);                   //计数器溢出中断失能
                    break;           
            case 2:
                    REG_LPTIM_LPTIE &= ~(1<<2);                    //外部触发到来中断失能
                    break;  
			case 3:        
                    REG_LPTIM_LPTIE &= ~(1<<3);                    //比较匹配2中断失能
                    break;			
            default:
                    break;            
        }        
    }
}
/*******************************************************************
* Function		: lptimer_pwm_init
* Description	: lptimer PWM输出初始化
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptimer_pwm_init(uint8_t clock_sel, uint8_t div)
{  
    PCLK0 |= (1<<6); 									    //开启lpTimer时钟使能 
    PRESET0 |= (1<<6);									    //lpTimer正常工作 
    
    REG_LPTIM_LPTCFG0 &= ~(3<<3);
	REG_LPTIM_LPTCFG0 |= (clock_sel<<3);					//时钟源设置 0:RCL低速时钟, 1:clk_1hz时钟,  2:系统时钟, 3:外部输入时钟 
 
	REG_LPTIM_LPTCFG0 &= ~(7<<0);									
    REG_LPTIM_LPTCFG0 |= (div<<0);							//设置计数时钟分频 
    
   	REG_LPTIM_LPTCFG1 &= ~(3<<3);									
    REG_LPTIM_LPTCFG1 |= (TIMER_TMODE<<3);                 //设置工作模式 
    
	REG_LPTIM_LPTCFG1 &= ~(1<<2);								  
    REG_LPTIM_LPTCFG1 |= (CONTINUOUS_MODE<<2);              //连续计数模式 
	
    REG_LPTIM_LPTCFG1 &= ~(1<<1);								    
    REG_LPTIM_LPTCFG1 |= (1<<1);                            //PWM输出模式 
	
}
/*******************************************************************
* Function		: lptimer_pwm_set
* Description	: lptimer_pwm_set
* Input			: xms:PWM波周期为xms = 时钟频率/target(时钟源为RCL低速时钟下最大计时时长为65535ms)
*                 duyt：占空比（0~100，标识占空比为0%~100%）
* Output		: none
* Return		: none
********************************************************************/

void lptimer_pwm_set(uint8_t changesel, uint32_t target, uint16_t duty)
{   
	
    REG_LPTIM_LPTTARGETL = (target)%256;											
    REG_LPTIM_LPTTARGETH = (target)/256;        
	
	if (changesel == PWM_CHANGE1)
	{
		REG_LPTIM_LPTCMPL = (((target-1)*(duty)/100))%256;
		REG_LPTIM_LPTCMPH = (((target-1)*(duty)/100))/256;
	
	}
	if (changesel == PWM_CHANGE2)
	{
		REG_LPTIM_LPTCMP2L = (((target-1)*(duty)/100))%256;
		REG_LPTIM_LPTCMP2H = (((target-1)*(duty)/100))/256;
	}
    
}

void lptimer_pwm_channel_config (uint8_t channel, uint8_t polar)
{
	if (channel == 1)
	{
		REG_LPTIM_CCMCFG1 |= (0x1<<0);	
		REG_LPTIM_LPTCFG1 |= (polar<<0);                            	
	}
	
	if (channel == 2)
	{
		REG_LPTIM_CCMCFG2 |= (0x1<<0);
		REG_LPTIM_CCMCFG2 |= (polar<<2);	
	}
}
/*******************************************************************
* Function		: lptimer_set_s
* Description	: lptimer_set_s
* Input			: xms:定时xms(时钟源为RCL低速时钟下最大计时时长为65535ms)
* Output		: none
* Return		: none
********************************************************************/
void lptimer_set_time(uint16_t target)
{   
    REG_LPTIM_LPTTARGETL = target%256;											
    REG_LPTIM_LPTTARGETH = target/256;         //定时xms

}

void lptimer_capture_init(uint8_t channel, uint8_t edge)
{
	
	if (channel == 1)
	{
		REG_LPTIM_CCMCFG1 = (1<<1)|(edge<<4);
	
		REG_LPTIM_CCMCFG1 |= (1<<0);   		//使能通道1捕获
	}
	
	if (channel == 2)
	{
		REG_LPTIM_CCMCFG2 = (1<<1)|(edge<<4);
		
		REG_LPTIM_CCMCFG2 |= (1<<0);   		//使能通道2捕获
	}

}

uint16_t lptimer_get_lptcmp(uint8_t channel)
{
	if (channel == 1)
	{
		return (REG_LPTIM_LPTCMPL+REG_LPTIM_LPTCMPH*256);
	}
	
	else
	{
		return (REG_LPTIM_LPTCMP2L+REG_LPTIM_LPTCMP2H*256);
	}
	
	
}

/*******************************************************************
* Function		: lptimer_on
* Description	: lptimer 计数启动
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptimer_start(void)
{
	REG_LPTIM_LPTCTRL |= (1<<0);						//打开LPTEN使能位，启动计数器  
}
/*******************************************************************
* Function		: lptimer_off
* Description	: lptimer 计数停止
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
void lptimer_stop(void)
{
	REG_LPTIM_LPTCTRL &= ~(1<<0);						//禁止计数器计数  
}

/*******************************************************************
* Function		: lptimer_trigger_edge_set
* Description	: lptimer_trigger边沿选择
* Input			: edge		上升沿/下降沿
* Output		: none
* Return		: none
********************************************************************/

void lptimer_trigger_edge_set(uint8_t edge)
{
	if(edge)
	{
		REG_LPTIM_LPTCFG0|= (1<<6);                   //设置下降沿tirgger计数 
	}
	else
	{
		REG_LPTIM_LPTCFG0&= ~(1<<6);                   	//设置上升沿tirgger计数
	}
}
/*******************************************************************
* Function		: lptimer_lptin_edge_set
* Description	: lptimer_lptin边沿选择
* Input			:edge		上升沿/下降沿
* Output		: none
* Return		: none
********************************************************************/
void lptimer_lptin_edge_set(uint8_t edge)
{
	if(edge)
	{
		REG_LPTIM_LPTCFG0|= (1<<5);                   	//设置下降沿tirgger计数 
	}
	else
	{
		REG_LPTIM_LPTCFG0&= ~(1<<5);                   	//设置上升沿tirgger计数 
	}
}
/*******************************************************************
* Function		: lptim_get_cnt_value
* Description	: 获取计数值
* Input			: none
* Output		: none
* Return		: none
********************************************************************/
uint16_t lptim_get_cnt_value(void)
{
	return REG_LPTIM_LPTCNTL+(REG_LPTIM_LPTCNTH*256);
}
/*******************************************************************
* Function		: lptimer_outio
* Description	: lptimer 输出管脚配置
* Input			: none
* Output		: none
* Return		: none
********************************************************************/

void lptimer_io_config(uint8_t io)
{
	switch(io)
    {
		
        case 0:
                REG_LPTIM_LPTCFG1 |= (1<<5);                   //P1_0作为LPIN
                break;           
        case 1:
                REG_LPTIM_LPTCFG1 |= (1<<7);                   //P1_1作为EXTRIGGER_IO_IEN
                break;  
		case 2: 
				REG_LPTIM_LPTCFG1 |= (1<<6);                 //P0_3作为LPOUT1
				break;
		case 3: 
				REG_P04_CFG = 0x04;                           //P0_4作为LPOUT2
				break;
		case 4: 
				REG_P12_CFG = 0x04;                           //P1_2作为LPCAP1
				break;
		case 5: 
				REG_P15_CFG = 0x07;                       	  //P1_5作为LPCAP2
				break;	
        default:
                 break;            
      }        

}

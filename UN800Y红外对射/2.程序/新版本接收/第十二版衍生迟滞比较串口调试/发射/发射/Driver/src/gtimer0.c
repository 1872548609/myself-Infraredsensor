/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : gtimer0.c
 * Description : gtimer0 driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-30
 ***********************************************************************/
#include "gtimer.h"
#include "gpio.h"

void (*gtimer0_callback[3])(void) = { 0 };


/***********************************************************************
 * Function   	: GTIM0_IRQHandler
 * Description	: GTIM0 interrupt handling
 * Input 		: none
 * Output		: none   
 * Return		: none
 ***********************************************************************/
void GTIM0_IRQHandler(void) interrupt 21
{
	if((REG_GTIM0_SR & GTIMER0_UIF)  && (REG_GTIM0_IER & GTIMER0_UIE))		//UE中断
	{
		if(gtimer0_callback[0] != NULL)
		{
			gtimer0_callback[0]();
		}
		REG_GTIM0_SR = GTIMER0_UIF;						//清除UE中断标志
	}
	else if((REG_GTIM0_SR & GTIMER0_CCIF)  && (REG_GTIM0_IER & GTIMER0_CCIE))		//比较中断/捕捉中断
	{
		if(gtimer0_callback[1] != NULL)
		{
			gtimer0_callback[1]();
		}
		REG_GTIM0_SR = GTIMER0_CCIF;					//清除中断/捕捉中断
	}
	else if((REG_GTIM0_SR & GTIMER0_BKE_IF) && (REG_GTIM0_IER & GTIMER0_BKE_IE))			//刹车中断
	{
		if(gtimer0_callback[2] != NULL)
		{
			gtimer0_callback[2]();
		}
		REG_GTIM0_SR = GTIMER0_BKE_IF;			//清除刹车中断标志
	}
	else
	{
		REG_GTIM0_SR = 0x7;									//清除所有中断标志
	}
}  


/************************************************************************
 * Function   	: gtimer0_count_init
 * Description	: gtimer0_count_init GTIMER0 初始化
 * Input 		: uint8_t arr	GTIMER0重载值
							uint8_t psc GTIMER0分频值
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer0_count_init(uint16_t arr,uint16_t psc)
{
	PCLK1 |= (1<<3); 									  //开Gtim0时钟使能 	
	PRESET1 |= (1<<3);									//GTim0正常工作
	
	REG_GTIM0_CR0 |= (1<<6);						//使能auto-repload 

	REG_GTIM0_PSC1 = (psc>>8)&0xFF;			//高8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK 
	REG_GTIM0_PSC0 = psc&0xFF;					//低8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK 
	
	REG_GTIM0_ARR1 = ((arr-1)>>8)&0xFF;			//高8位 自动装载值
	REG_GTIM0_ARR0 = (arr-1)&0xFF;					//低8位 自动装载值
	
	REG_GTIM0_EGR = 0x1;								//产生UE事件，将PSC的值立即载入shadow寄存器
	REG_GTIM0_SR |= (1<<0);							//清除UE产生的中断，否则会直接进入中断服务函数
	
}


/************************************************************************
 * Function   	: gtimer0_pwm_init
 * Description	: gtimer0_pwm_init GTIMER0 中断初始化
 * Input 		: uint16_t arr	GTIMER0重载值
							uint16_t psc  GTIMER0分频值
							uint16_t ccr  GTIMER0比较值
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer0_pwm_init(uint16_t arr, uint16_t psc, uint16_t ccr)
{
	PCLK1 |= (1<<3); 									  //开Gtim0时钟使能 
	PRESET1 |= (1<<3);									//GTim0正常工作 
	
	REG_P13_CFG = 0x06;									//P13 CH 
	REG_P15_CFG = 0x04;									//P15 CH 
//	REG_P25_CFG = 0x04;								//P25 CH 
	
	REG_P14_CFG = 0x05;									//P14 CHN
	REG_P20_CFG = 0x06;									//P20 CHN
//	REG_P23_CFG = 0x05;								//P23 CHN 
	
	REG_GTIM0_CR0 |= (1<<6);						//使能auto-repload 
	
	REG_GTIM0_PSC1 = (psc>>8)&0xFF;			//高8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK 
	REG_GTIM0_PSC0 = psc&0xFF;					//低8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK 
	
	REG_GTIM0_ARR1 = ((arr-1)>>8)&0xFF;			//高8位 自动装载值 
	REG_GTIM0_ARR0 = (arr-1)&0xFF;					//低8位 自动装载值 
	
	REG_GTIM0_ARRN1 = ((arr-1)>>8)&0xFF;		//高8位 自动装载值 
	REG_GTIM0_ARRN0 = (arr-1)&0xFF;					//低8位 自动装载值 
				
	REG_GTIM0_CCR1 = ((ccr)>>8)&0xFF;		//高8位 比较寄存器 
	REG_GTIM0_CCR0 = (ccr)&0xFF;					//低8位 比较寄存器 
				
	REG_GTIM0_CCRN1 = ((ccr)>>8)&0xFF;	//高8位 比较寄存器
	REG_GTIM0_CCRN0 = (ccr)&0xFF;				//低8位 比较寄存器
				
	REG_GTIM0_CR1 |= (1<<0);						//互补PWM和原PWM反相位
				
	REG_GTIM0_CR1 |= (1<<1);						//避免死区功能使能
				
	REG_GTIM0_EGR = 0x1;								//产生UE事件，将PSC的值立即载入shadow寄存器
	REG_GTIM0_SR |= (1<<0);							//清除UE产生的中断，否则会直接进入中断服务函数
	
	REG_GTIM0_CR2 |= (1<<2);						//输出总使能
	
	REG_GTIM0_CCER |= (1<<0);						//使能输出
	
	
}

/***********************************************************************
 * Function   	: gtimer0_capture_init
 * Description	: gtimer0_capture_init GTIMER0输入捕获
 * Input 		: uint16_t arr          装载值
 * 					: uint8_t capture_div   捕捉源预分频位
 * 					: uint8_t capssel  			捕捉源选择位
 * Output		: none
 * Return		: none
 ***********************************************************************/
void gtimer0_capture_init(uint16_t arr,uint8_t capture_div,uint8_t capssel)
{
	PCLK1 |= (1<<3); 									  	//开Gtim0时钟使能
	PRESET1 |= (1<<3);										//GTim0正常工作
		
	REG_P13_CFG = 0x06;										//P13 GTIMER0_CH 

	REG_GTIM0_ARR1 = ((arr-1)>>8)&0xFF;				//高8位 自动装载值 
	REG_GTIM0_ARR0 = (arr-1)&0xFF;						//低8位 自动装载值 
		
	REG_GTIM0_CCMR0 &= ~(1<<7);						//外部计数源滤波使能：无滤波功能 
	REG_GTIM0_CCMR0 |= (1<<0);						//配置CCS为输入 
	
	REG_GTIM0_CCMR1 = (REG_GTIM0_CCMR1&(~(0x03<<0)))|(capssel<<0);			//捕捉源选择位 	

	REG_GTIM0_CCMR1 = (REG_GTIM0_CCMR1&(~(3<<5)))|(capture_div<<5);	//分频配置
		
	REG_GTIM0_CCMR1 &= ~(0x03<<2);				//捕获沿触发控制位：上升沿触发 
		
	REG_GTIM0_CCER |= (1<<0);							//使能捕捉功能 
	
}

void gtimer0_bke_init(void)
{
	PCLK1 |= (1<<3); 								//开Gtim0时钟使能 	
	PRESET1 |= (1<<3);							//GTim0正常工作 

	REG_P01_CFG = 0x05;							//P01 GTIMER0_BKE 
//	REG_P22_CFG = 0x05;							//P22 GTIMER0_BKE 	
//	REG_P25_CFG = 0x05;							//P25 GTIMER0_BKE 
	
	REG_GTIM0_CR1 &= ~(3<<6);				//PWM刹车触发后，PWM正向电平状态设置位:低电平 
	REG_GTIM0_CR2 &= ~(3<<0);				// PWM刹车触发后，PWM互补电平状态设置位:低电平 

	REG_GTIM0_CR1 &= ~(1<<3);				//刹车信号高电平有效
	
	REG_GTIM0_CR1 |= (1<<2);				//使能刹车功能 

	REG_GTIM0_CCER |= (1<<0);				//使能输出 
}

/************************************************************************
 * Function   	: gtimer0_irq_init
 * Description	: gtimer0_irq_init 	GTIMER0 中断初始化
 * Input 		: uint8_t irq_enable		GTIMER0中断使能开关 1：使能 0：禁止
 *						uint8_t gtimer_irq_type GTIMER0中断类型选择
 *            void (*pfunc)()					GTIMER0中断回调函数
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer0_irq_init(uint8_t irq_enable,uint8_t gtimer_irq_type,void (*pfunc)())
{
	REG_GTIM0_SR = 0x7;		//清除状态标志
	
	if(irq_enable == GTIMER_IRQ_ENABLE)
	{
		switch(gtimer_irq_type)
		{
			case GTIMER0_UIE_IRQ:
					gtimer0_callback[0] = pfunc;
					REG_GTIM0_IER |= GTIMER0_UIE;				//使能UPDATE中断
					break;
			case GTIMER0_CCIE_IRQ:
					gtimer0_callback[1] = pfunc;
					REG_GTIM0_IER |= GTIMER0_CCIE;			//使能比较/捕捉中断
					break;	
			case GTIMER0_BKE_IE_IRQ:
					gtimer0_callback[2] = pfunc;
					REG_GTIM0_IER |= GTIMER0_BKE_IE;		//使能刹车中断
					break;
			default:
					break;				
		}	
		EA = 1 ;																	//开启总中断
		IEN1 |= (1<<0);														//开启gtimer0中断 
	}
	else
	{
		switch(gtimer_irq_type)
		{
			case GTIMER0_UIE_IRQ:
					gtimer0_callback[0] = pfunc;
					REG_GTIM0_IER |= ~GTIMER0_UIE;			//禁止UPDATE中断
					break;
			case GTIMER0_CCIE_IRQ:
					gtimer0_callback[1] = pfunc;
					REG_GTIM0_IER |= ~GTIMER0_CCIE;			//禁止比较/捕捉中断
					break;	
			case GTIMER0_BKE_IE_IRQ:
					gtimer0_callback[2] = pfunc;
					REG_GTIM0_IER |= ~GTIMER0_BKE_IE;		//禁止刹车中断
					break;		
			default:
					break;				
		}	
	}
	
}

/************************************************************************
 * Function   	: gtimer0_start
 * Description	: gtimer0_start 启动GTIMER0计数
 * Input 		: none
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer0_start(void)
{
	REG_GTIM0_CR0 |= (1<<0);				//使能计数器 
}



/***********************************************************************
 * Function   	: gtimer0_stop
 * Description	: gtimer0_stop  停止GTIMER0计数器
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void gtimer0_stop(void)
{
	REG_GTIM0_CR0 &= ~(1<<0);				//不使能计数器
}



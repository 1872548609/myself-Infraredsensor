/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : gtimer1.c
 * Description : gtimer2 driver source file
 * Author(s)   : shengyu
 * version     : V1.0
 * Modify date : 2021-12-30
 ***********************************************************************/
#include "gtimer.h"

void (*gtimer1_callback[3])(void) = { 0 };

/***********************************************************************
 * Function   	: GTIM1_IRQHandler
 * Description	: GTIM1 interrupt handling
 * Input 		: none
 * Output		: none   
 * Return		: none
 ***********************************************************************/
void GTIM1_IRQHandler(void) interrupt 20
{
	if((REG_GTIM1_SR & GTIMER1_UIF)&& (REG_GTIM1_IER & GTIMER1_UIE))				//UE中断
	{
		if(gtimer1_callback[0] != NULL)
		{
			gtimer1_callback[0]();
		}
		REG_GTIM1_SR = GTIMER1_UIF;							//清除UE中断标志
	}
	else if((REG_GTIM1_SR & GTIMER1_CCIF) && (REG_GTIM1_IER & GTIMER1_CCIE))		//比较中断/捕捉中断
	{
		if(gtimer1_callback[1] != NULL)
		{
			gtimer1_callback[1]();
		}
		REG_GTIM1_SR = GTIMER1_CCIF;							//清除中断/捕捉中断
	}
	else if((REG_GTIM1_SR & GTIMER1_BKE_IF) && (REG_GTIM1_IER & GTIMER1_BKE_IE))		//刹车中断
	{
		if(gtimer1_callback[2] != NULL)
		{
			gtimer1_callback[2]();
		}
		REG_GTIM1_SR = GTIMER1_BKE_IF;							//清除刹车中断标志
	}
	else
	{
		REG_GTIM1_SR = 0x7;								//清除所有中断标志
	}
}  


/************************************************************************
 * Function   	: gtimer1_count_init
 * Description	: gtimer1_count_init GTIMER1 count初始化
 * Input 		: uint16_t arr	GTIMER1重载值
							uint16_t psc 	GTIMER1分频值
 * Output 	: none
 * Return		: none
 ************************************************************************/	 
void gtimer1_count_init(uint16_t arr,uint16_t psc)
{
	PCLK1 |= (1<<5); 										//开Gtim1时钟使能	
	PRESET1 |= (1<<5);									//GTim1正常工作
		
	REG_GTIM1_CR0 |= (1<<6);						//使能auto-repload
	
	REG_GTIM1_PSC1 = (psc>>8)&0xFF;			//高8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK
	REG_GTIM1_PSC0 = psc&0xFF;					//低8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK
		
	REG_GTIM1_ARR1 = ((arr-1)>>8)&0xFF;			//高8位 自动装载值
	REG_GTIM1_ARR0 = (arr-1)&0xFF;					//低8位 自动装载值
		
	REG_GTIM1_EGR = 0x1;								//产生UE事件，将PSC的值立即载入shadow寄存器
	REG_GTIM1_SR |= (1<<0);							//清除UE产生的中断，否则会直接进入中断服务函数
	
}



/************************************************************************
 * Function   	: gtimer1_pwm_init
 * Description	: gtimer1_pwm_init GTIMER1 PWM初始化
 * Input 		: uint16_t arr	GTIMER1重载值
							uint16_t psc 	GTIMER1分频值
							uint16_t ccr 	GTIMER1比较值
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer1_pwm_init(uint16_t arr, uint16_t psc, uint16_t ccr)
{
	PCLK1 |= (1<<5); 									  //开Gtim1时钟使能 
	PRESET1 |= (1<<5);									//GTim1正常工作 
	
	REG_P03_CFG = 0x06;									//P03 CH 
	REG_P12_CFG = 0x05;									//P12 CH 
//	REG_P26_CFG = 0x05;								//P26 CH 
	
	REG_P00_CFG = 0x04;									//P00 CHN 
	REG_P11_CFG = 0x06;									//P11 CHN 
//	REG_P27_CFG = 0x05;								//P27 CHN 
	
	REG_GTIM1_CR0 |= (1<<6);						//使能auto-repload 
					
	REG_GTIM1_PSC1 = (psc>>8)&0xFF;			//高8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK
	REG_GTIM1_PSC0 = psc&0xFF;					//低8位 FCK_CNT=FCK_PSC/(PSC[15:0]+1);  FCK_PSC = APBCLK
				
	REG_GTIM1_ARR1 = ((arr-1)>>8)&0xFF;			//高8位 自动装载值
	REG_GTIM1_ARR0 = (arr-1)&0xFF;					//低8位 自动装载值 
					
	REG_GTIM1_ARRN1 = ((arr-1)>>8)&0xFF;		//高8位 自动装载值
	REG_GTIM1_ARRN0 = (arr-1)&0xFF;					//低8位 自动装载值 
					
	REG_GTIM1_CCR1 = (ccr>>8)&0xFF;			//高8位 比较寄存器 
	REG_GTIM1_CCR0 = ccr&0xFF;					//低8位 比较寄存器
					
	REG_GTIM1_CCRN1 = (ccr>>8)&0xFF;		//高8位 比较寄存器 
	REG_GTIM1_CCRN0 = ccr&0xFF;					//低8位 比较寄存器
					
	REG_GTIM1_CR1 |= (1<<0);						//互补PWM和原PWM反相位
					
	REG_GTIM1_CR1 |= (1<<1);						//避免死区功能使能
				
	REG_GTIM1_EGR = 0x1;								//产生UE事件，将PSC的值立即载入shadow寄存器
	REG_GTIM1_SR |= (1<<0);							//清除UE产生的中断，否则会直接进入中断服务函数
				
	REG_GTIM1_CR2 |= (1<<2);						//输出总使能
					
	REG_GTIM1_CCER |= (1<<0);						//使能输出
	
	
}


/***********************************************************************
 * Function   	: gtimer1_capture_init
 * Description	: gtimer1_capture_init GTIMER1输入捕获
 * Input 		: uint16_t arr         装载值
 * 					: uint8_t capture_div  捕捉源预分频位
 * 					: uint8_t capssel  		 捕捉源选择位
 * Output		: none
 * Return		: none
 ***********************************************************************/
void gtimer1_capture_init(uint16_t arr,uint8_t capture_div,uint8_t capssel)
{
	PCLK1 |= (1<<5); 									    //开Gtim1时钟使能 
	PRESET1 |= (1<<5);										//GTim1正常工作 
	
	REG_P03_CFG = 0x06;										//P03 CH 
	REG_P12_CFG = 0x05;										//P12 CH
//	REG_P26_CFG = 0x05;									//P26 CH 
	
	REG_GTIM1_ARR1 = ((arr-1)>>8)&0xFF;				//高8位 自动装载值 
	REG_GTIM1_ARR0 = (arr-1)&0xFF;						//低8位 自动装载值 
					
	REG_GTIM1_CCMR0 &= ~(1<<7);						//外部计数源滤波使能：无滤波功能 
	REG_GTIM1_CCMR0 |= (1<<0);						//配置CCS为输入 
				
	REG_GTIM1_CCMR1 = (REG_GTIM1_CCMR1 &(~(0x03<<0)))|(capssel<<0);	//捕捉源选择位 	
	
	REG_GTIM1_CCMR1 = (REG_GTIM1_CCMR1&(~(3<<5)))|(capture_div<<5);
					
	REG_GTIM1_CCMR1 &= ~(0x03<<2);				//捕获沿触发控制位：上升沿触发 
					
	REG_GTIM1_CCER |= (1<<0);							//使能捕捉功能 
	
}

/***********************************************************************
 * Function   	: gtimer1_bke_init
 * Description	: gtimer1_bke_init gtimer1刹车
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void gtimer1_bke_init(void)
{
	PCLK1 |= (1<<5); 									 	//开Gtim1时钟使能 	
	PRESET1 |= (1<<5);									//GTim1正常工作 

	REG_P04_CFG = 0x05;									//P04 GTIMER1_BKE 
//	REG_P14_CFG = 0x06;								//P14 GTIMER1_BKE 
//	REG_P15_CFG = 0x05;								//P15 GTIMER1_BKE 
	
	REG_GTIM1_CR1 &= ~(3<<6);						//PWM刹车触发后，PWM正向电平状态设置位:低电平 
	REG_GTIM1_CR2 &= ~(3<<0);						//PWM刹车触发后，PWM互补电平状态设置位:低电平 
					
	REG_GTIM1_CR1 &= ~(1<<3);						//刹车信号高电平有效
			
	REG_GTIM1_CR1 |= (1<<2);						//使能刹车功能 
		
	REG_GTIM1_CCER |= (1<<0);						//使能输出 
}



/************************************************************************
 * Function   	: gtimer1_irq_init
 * Description	: gtimer1_irq_init 	GTIMER1 中断初始化
 * Input 		: uint8_t irq_enable		GTIMER1中断使能开关 1：使能 0：禁止
							uint8_t gtimer_irq_type GTIMER1中断类型选择
 *            void (*pfunc)()					GTIMER1中断回调函数
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer1_irq_init(uint8_t irq_enable,uint8_t gtimer_irq_type,void (*pfunc)())
{
	REG_GTIM1_SR = 0x7;													//清除状态标志
	
	if(irq_enable == GTIMER_IRQ_ENABLE)
	{
		switch(gtimer_irq_type)
		{
			case GTIMER1_UIE_IRQ:
					gtimer1_callback[0] = pfunc;
					REG_GTIM1_IER |= GTIMER1_UIE;				//使能UPDATE中断
					break;
			case GTIMER1_CCIE_IRQ:
					gtimer1_callback[1] = pfunc;
					REG_GTIM1_IER |= GTIMER1_CCIE;			//使能比较/捕捉中断
					break;	
			case GTIMER1_BKE_IE_IRQ:
					gtimer1_callback[2] = pfunc;
					REG_GTIM1_IER |= GTIMER1_BKE_IE;		//使能刹车中断
					break;
			default:
					break;				
		}	
		EA = 1 ;																	//开启总中断
		IEN1 |= (1<<2);														//开启gtimer1中断
	}
	else
	{
		switch(gtimer_irq_type)
		{
			case GTIMER1_UIE_IRQ:
					gtimer1_callback[0] = pfunc;
					REG_GTIM1_IER |= ~GTIMER1_UIE;			//禁止UPDATE中断
					break;
			case GTIMER1_CCIE_IRQ:
					gtimer1_callback[1] = pfunc;
					REG_GTIM1_IER |= ~GTIMER1_CCIE;			//禁止比较/捕捉中断
					break;	
			case GTIMER1_BKE_IE_IRQ:
					gtimer1_callback[2] = pfunc;
					REG_GTIM1_IER |= ~GTIMER1_BKE_IE;		//禁止刹车中断
					break;		
			default:
					break;				
		}	
	}
	
}

/************************************************************************
 * Function   	: gtimer1_start
 * Description	: gtimer1_start 启动GTIMER1计数
 * Input 		: none
 * Output 	: none
 * Return		: none
 ************************************************************************/	
void gtimer1_start(void)
{
	REG_GTIM1_CR0 |= (1<<0);						//使能计数器
}

/***********************************************************************
 * Function   	: gtimer1_stop
 * Description	: gtimer1_stop	停止GTIMER1计数器
 * Input 		: none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void gtimer1_stop(void)
{
	REG_GTIM1_CR0 &= ~(1<<0);					//不使能计数器
}



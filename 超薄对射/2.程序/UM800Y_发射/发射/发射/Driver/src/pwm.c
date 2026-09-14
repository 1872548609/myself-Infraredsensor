/***********************************************************************
 * Copyright (c)  2017 - 2020, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : pwm.c
 * Description : pwm driver source file
 * Author(s)   : yanhaihua
 * version     : V1.0
 * Modify date : 2021-2-26
 ***********************************************************************/
#include "pwm.h"

void (*pwmfunc_callback[3])(void) = { 0 };
/***********************************************************************
 * Function     : PWM_IRQHandler
 * Description  : PWM中断处理函数   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void PWM_IRQHandler(void) interrupt 5
{
	if((PWM0CON & (1<<1)))
	{
		PWM0CON &= ~(1<<1);					// 清除PWM0溢出标志
		if(pwmfunc_callback[0] != NULL)
		{
			pwmfunc_callback[0]();
		}
	}

	if((PWM1CON & (1<<1)))
	{
		PWM1CON &= ~(1<<1);					// 清除PWM1溢出标志	
		if(pwmfunc_callback[1] != NULL)
		{
			pwmfunc_callback[1]();
		}
	}

	if((PWM2CON & (1<<1)))
	{
		PWM2CON &= ~(1<<1);					// 清除PWM2溢出标志
		if(pwmfunc_callback[2] != NULL)
		{
			pwmfunc_callback[2]();
		}
	}
}

/***********************************************************************
 * Function     : pwm0_init
 * Description  : 初始化PWM0   
 * Input        : uint16_t cycle 周期（周期时长计算 = 系统时钟/cycle）
 *                uint16_t duty  占空比
 *                uint8_t  level HIGH:占空比期间输出高电平；LOW:占空比期间输出低电平
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm0_init(uint16_t cycle, uint16_t duty, uint8_t level)  
{
	REG_P10_CFG = 0x03;
	if(level == LOW)
	{
	    PWM0CON |= (1<<6);        	// 占空比期间输出低电平 
	}
	else
	{ 
	    PWM0CON &= ~(1<<6);       	// 占空比期间输出高电平 
	}
	 
    PWM0DL = duty&0xFF;   			// 占空比低8位 
    PWM0DH = (duty>>8)&0xFF;  		// 占空比高8位 
	
    PWM0PL = cycle&0xFF;   			// 周期低8位 
    PWM0PH = (cycle>>8)&0xFF;		// 周期高8位 
		
    PWM0CON |= (1<<7);				//使能PWM0模块
}	

/***********************************************************************
 * Function     : pwm0_irq_init
 * Description  : 初始化PWM0中断   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm0_irq_init(em_pwm_irq mode,void (*pfunc)())
{
	if(mode == PWM_IRQ_ENABLE)
	{
		EA = 1;								// 打开总中断 
		EPWM = 1;							// 打开PWM中断 
		PWM0CON |= (1<<2);					// 允许PWM0中断 
		pwmfunc_callback[0] = pfunc;
	}
	else
	{
		EA = 0;								// 关闭总中断，注意如果存在中断，关闭总中断时需要注意 
		EPWM = 0;							// 关闭PWM中断 
		PWM0CON &= ~(1<<2);					//不允许PWM0中断 
	}
}

/***********************************************************************
 * Function     : pwm0_start
 * Description  : 启动PWM0输出   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm0_start()  
{
    PWM0CON |= (1<<0);													//PWM0输出允许
}

/***********************************************************************
 * Function     : pwm0_stop
 * Description  : 停止PWM输出   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm0_stop()  
{
    PWM0CON &= ~(1<<0);		 											//PWM0输出禁止 
}

/***********************************************************************
 * Function     : pwm1_init
 * Description  : pwm1_init   
 * Input        : uint16_t cycle 周期（周期时长计算 = 系统时钟/cycle）
 *                uint16_t duty  占空比
 *                uint8_t  level HIGH:占空比期间输出高电平；LOW:占空比期间输出低电平
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm1_init(uint16_t cycle, uint16_t duty, uint8_t level)  
{
	REG_P11_CFG = 0x03;
	if(level == LOW)
	{
	    PWM1CON |= (1<<6);          //占空比期间输出低电平
	}
	else
	{
		PWM1CON &= ~(1<<6);         // 占空比期间输出高电平 
	}
	
	PWM1DL = duty&0xFF;   			// 占空比低8位 
    PWM1DH = (duty>>8)&0xFF;  		// 占空比高8位 
   
    PWM1PL = cycle&0xFF;   			// 周期低8位
    PWM1PH = (cycle>>8)&0xFF;		// 周期高8位 

	PWM1CON |= (1<<7);				// 使能PWM1模块 
}

/***********************************************************************
 * Function     : pwm1_irq_init
 * Description  : 初始化PWM1中断   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm1_irq_init(em_pwm_irq mode,void (*pfunc)())
{
	if(mode == PWM_IRQ_ENABLE)
	{
		EA = 1;								// 打开总中断 
		EPWM = 1;							// 打开PWM中断 
		PWM1CON |= (1<<2);					// 允许PWM1中断 
		pwmfunc_callback[1] = pfunc;
	}
	else
	{
		EA = 0;								// 关闭总中断，注意如果存在中断，关闭总中断时需要注意 
		EPWM = 0;							//关闭PWM中断 
		PWM1CON &= ~(1<<2);					// 不允许PWM1中断
	}
}

/***********************************************************************
 * Function     : pwm1_start
 * Description  : 启动PWM1输出   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm1_start()  
{	 
    PWM1CON |= (1<<0);				// PWM1输出允许
}

/***********************************************************************
 * Function     : pwm1_stop
 * Description  : 停止PWM1输出   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm1_stop()  
{
    PWM1CON &= ~(1<<0);				// PWM1输出禁止 
}

/***********************************************************************
 * Function     : pwm2_init
 * Description  : pwm2_init   
 * Input        : uint16_t 周期（周期时长计算 = 系统时钟/cycle）
 *                uint16_t duty  占空比
 *                uint8_t  level HIGH:占空比期间输出高电平；LOW:占空比期间输出低电平
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm2_init(uint16_t cycle, uint16_t duty, uint8_t level)  
{
	REG_P14_CFG = 0x02;
	if(level == LOW)
	{
	    PWM2CON |= (1<<6);         		// 占空比期间输出低电平 
	}
	else
	{
		PWM2CON &= ~(1<<6);         	// 占空比期间输出高电平 
	}
	 
    PWM2DL = duty&0xFF;   				// 占空比低8位 
    PWM2DH = (duty>>8)&0xFF;  			// 占空比高8位
   
    PWM2PL = cycle&0xFF;   				// 周期低8位
    PWM2PH = (cycle>>8)&0xFF;			// 周期高8位 
    
    PWM2CON |= (1<<7);					// 使能PWM2模块
}

/***********************************************************************
 * Function     : pwm2_irq_init
 * Description  : 初始化PWM2中断   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm2_irq_init(em_pwm_irq mode,void (*pfunc)())
{
	if(mode == PWM_IRQ_ENABLE)
	{
		EA = 1;							// 打开总中断 
		EPWM = 1;						// 打开PWM中断 
		PWM2CON |= (1<<2);				// 允许PWM2中断 
		pwmfunc_callback[2] = pfunc;
	}
	else
	{
		EA = 0;							// 关闭总中断，注意如果存在中断，关闭总中断时需要注意 
		EPWM = 0;						// 关闭PWM中断 
		PWM2CON &= ~(1<<2);				//不允许PWM2中断
	}
}

/***********************************************************************
 * Function     : pwm2_start
 * Description  : 启动PWM2输出   
 * input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm2_start()  
{	 
	PWM2CON |= (1<<0);				// PWM2输出允许
}

/***********************************************************************
 * Function     : pwm2_stop
 * Description  : 停止PWM2输出   
 * Input        : none
 * Output		: none
 * Return		: none
 ***********************************************************************/
void pwm2_stop()  
{
    PWM2CON &= ~(1<<0);		    	// PWM2输出禁止 
}

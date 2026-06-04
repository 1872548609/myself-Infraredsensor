#include "lvd.h"

volatile uint8_t lvd_flag=0;

void EFC_IRQHandler(void) interrupt 11
{			
	if(OSTATUS & 0x08)             			//检测到低电压状态								
	{	
		lvd_flag = 1;
		OINTUS = 0x20;			//清LVD低电压中断状态位
	}	
	
	OINTUS = 0xff;			//清中断状态位

}



void lvd_init(void)
{
	LVDCON |=(0x4<<1);		//2.85V-Lvd  000:4.12V ; 001:3.69V ;010:3.38V ; 011:3.09V ;  100:2.85V；   101:2.65V； 110: 2.48V；  111: 2.32V;
	LVDCON &=~(1<<4); 		//使能LVD模块
	OINTUS |= (1<<5);			//清LVD低压中断状态位 
	OINTEN |= (1<<5);			//LVD低电压中断使能
	IEN1 |= (1<<3);				//EFC中断使能
	
	EA = 1;			

}



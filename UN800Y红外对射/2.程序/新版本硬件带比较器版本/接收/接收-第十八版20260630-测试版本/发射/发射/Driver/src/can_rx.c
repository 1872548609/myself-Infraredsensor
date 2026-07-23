/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : can.c
 * Description : can driver source file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-06-24
 ***********************************************************************/
#include "can_rx.h"

void (*can_callback)(void) = { 0 };

/************************************************************************
 * function   : CAN_IRQHandler
 * Description: CAN IRQ Handler
 * input : none
 * return: none
 ************************************************************************/
void CAN_IRQHandler(void) interrupt 8
{
	uint8_t irq_status;
	
	irq_status = REG_CAN_ISR;

	if(irq_status & CAN_IMR_RIM) 
	{
		if(can_callback != NULL)
		{
			((void(*)())(can_callback))();  					//can的回调函数
		}
		REG_CAN_ISR = CAN_IMR_RIM;
	}
	else 
	{
		REG_CAN_ISR = 0x7f;
	}

}
	
/************************************************************************
* function   : can_init
* Description: can_init
* input 	 : baudrate 速率可以选择为1M/500k/250k/125k bps
  
	Fpclk=16Mhz,
	CAN波特率 BitRate = Fpclk/2*((BRP+1)*(TS1+TS2+3)), 有个约定:TS1>=TS2

	设波特率为1M的参数：
    设置BRP=0(2分频), BitRate = 1M = 16M/2((0+1)*(TS1+TS2+3)),所以可以设置TS1=3,TS2=2

	设波特率为500K的参数：
    设置BRP=1(4分频), BitRate = 0.5M = 16M/2((1+1)*(TS1+TS2+3)),所以可以设置TS1=3,TS2=2

	设波特率为250K的参数：
    设置BRP=3(8分频), BitRate = 0.25M = 16M/2((3+1)*(TS1+TS2+3)),所以可以设置TS1=3,TS2=2
	
	设波特率为125K的参数：
    设置BRP=7(16分频), BitRate = 0.125M = 16M/2((7+1)*(TS1+TS2+3)),所以可以设置TS1=3,TS2=2


 * return: none

 ************************************************************************/
void can_init(uint8_t baudrate)
{
	BEEPCTR |= (1<<5);				//CAN时钟打开
	BEEPCTR |= (1<<3);				//CAN复位释放
	
	REG_P12_CFG = 0x7;				//P12复用为CAN_TX

	REG_P23_CFG = 0x7;				//P23复用为CAN_RX

	REG_CAN_MR |= (1<<2); 			//CAN工作复位模式

	switch(baudrate)
    {
         case 0:                                					//1M bps
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x03<<6)))|(1<<6);	//SJW同步跳跃宽度 2个时间单元
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x3f<<0)))|(0<<0);	//BRP 2分频
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x0f<<0)))|(3<<0);	//TS1 3个时间单元
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x07<<4)))|(2<<4);	//TS2 2个时间单元间单元
		 
				break;
		 
		 case 1:                                					//500k bps
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x03<<6)))|(1<<6);	//SJW同步跳跃宽度 2个时间单元
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x3f<<0)))|(1<<0);	//BRP 4分频
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x0f<<0)))|(3<<0);	//TS1 4个时间单元
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x07<<4)))|(2<<4);	//TS2 3个时间单元
				break;
		 
		 case 2:                                					//250k bps		 
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x03<<6)))|(1<<6);		//SJW同步跳跃宽度 2个时间单元
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x3f<<0)))|(3<<0);	//BRP 8分频
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x0f<<0)))|(3<<0);	//TS1 4个时间单元
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x07<<4)))|(2<<4);	//TS2 3个时间单元
				break;
		 
		 case 3:                                					//125k bps 
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x03<<6)))|(1<<6);	//SJW同步跳跃宽度 2个时间单元
				REG_CAN_BTR0 = (REG_CAN_BTR0&(~(0x3f<<0)))|(7<<0);	//BRP 16分频
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x0f<<0)))|(3<<0);	//TS1 4个时间单元
				REG_CAN_BTR1 = (REG_CAN_BTR1&(~(0x07<<4)))|(2<<4);	//TS2 3个时间单元
				break;
		 
		 default:
			 
				break;
	}
	
	
	REG_CAN_ISR = 0x7F;				//清除中断标志
	
	REG_CAN_AMR0 = 0xFF;
	REG_CAN_AMR1 = 0xFF;
	REG_CAN_AMR2 = 0xFF;
	REG_CAN_AMR3 = 0xFF;
	
	REG_CAN_MR &= ~(1<<1);			//CAN进入正常模式
	REG_CAN_MR &= ~(1<<2); 			//CAN退出复位模式
}

/************************************************************************
 * function   : can_irq_init
 * Description: can_irq_init
 * input : filter_value, 
 * return: none
 ************************************************************************/
void can_irq_init(uint8_t irq_able, void (*pfunc)())
{
	if(irq_able)
	{
		EA = 1;
		IEN2 |= (1<<1); 
		can_callback = pfunc;
		REG_CAN_IMR |= (1<<3);
		
	}
	else
	{
		EA = 0;
		IEN2 &= ~(1<<1);
		REG_CAN_IMR &= ~(1<<3);	
		can_callback = NULL;		
	}
	
	
}
/************************************************************************
 * function   : can_filter_config
 * Description: can filter config
 * input : *can_filter_msg, data_mode,data_mode
 * return: none
 ************************************************************************/
void can_filter_config(S_Can_Filter_Msg *can_filter_msg,uint8_t filter_mode, uint8_t filter_en)
{
	REG_CAN_MR |= (1<<2); 		//CAN工作复位模式
	
	REG_CAN_ACR0 = 0;
	REG_CAN_ACR1 = 0;
	REG_CAN_ACR2 = 0;
	REG_CAN_ACR3 = 0;
	REG_CAN_AMR0 = 0;
	REG_CAN_AMR1 = 0;
	REG_CAN_AMR2 = 0;
	REG_CAN_AMR3 = 0;
	
	if(filter_en)				//使能can过滤器
	{
		if(filter_mode)				
		{
			REG_CAN_MR |= (1<<0);												//使用单过滤器
			if(can_filter_msg->ide == 0)										//标准格式
			{
				
				REG_CAN_ACR1 |= ((can_filter_msg->rtr &0x01) << 4);				//设置RTR
				REG_CAN_ACR1 |= ((can_filter_msg->std_id1 & 0x7)<<5);			//设置标准ID低三位
				REG_CAN_ACR0 |= ((can_filter_msg->std_id1 & 0x7f8)>>3);			//设置标准ID高八位
				
				REG_CAN_AMR3 = 0xFF;											//不对比ACR3(即不对比第2个数据)
				REG_CAN_AMR2 = 0xFF;											//不对比ACR2(即不对比第1个数据)
				REG_CAN_AMR1 = 0;												//对比ACR1(即对比ID低三位和RTR)
				REG_CAN_AMR0 = 0;												//对比ACR0(即对比ID高八位)
			}
			else																//扩展格式
			{
				REG_CAN_ACR0 |= ((can_filter_msg->ext_id1 & 0x1FE00000) >> 21);	//设置扩展ID的[28:21]
				REG_CAN_ACR1 |=	((can_filter_msg->ext_id1 & 0x1FE000) >> 13);	//设置扩展ID的[20:13]	
				REG_CAN_ACR2 |= ((can_filter_msg->ext_id1 & 0x1FE0) >> 5);		//设置扩展ID的[12:5]
				REG_CAN_ACR3 |=	((can_filter_msg->ext_id1 & 0x1F)<<3);			//设置扩展ID的[4:0]
				REG_CAN_ACR3 |=	((can_filter_msg->rtr &0x01) << 2);				//设置RTR
				
				REG_CAN_AMR0 = 0;												//对比ACR0(即对比扩展ID的[28:21])
				REG_CAN_AMR1 = 0;           									//对比ACR1(即对比扩展ID的[20:13])
				REG_CAN_AMR2 = 0;           									//对比ACR2(即对比扩展ID的[12:5])
				REG_CAN_AMR3 = 0;           									//对比ACR3(即对比扩展ID的[4:0]和RTR位)
			}
		}
		else
		{
			REG_CAN_MR &= ~(1<<0);												//双过滤									
			if(can_filter_msg->ide == 0)										//标准格式
			{
		
				REG_CAN_ACR0 |=	((can_filter_msg->std_id1 & 0x7F8) >> 3);		//设置过滤器1的标准ID高八位
				REG_CAN_ACR1 |=	((can_filter_msg->std_id1 & 0x7)<<5);			//设置过滤器1的标准ID低三位
				REG_CAN_ACR1 |= ((can_filter_msg->rtr &0x01) << 4);				//设置过滤器1的RTR
				
				REG_CAN_AMR0 = 0;												//对比过滤器1的标准ID高八位
				REG_CAN_AMR1 = 0xF;												//对比过滤器1的标准ID低三位和RTR,不对比过滤器1的第1个数据高四位
				
				
				REG_CAN_ACR2 |=	((can_filter_msg->std_id2 & 0x7F8) >> 3);		//设置过滤器2的标准ID高八位
				REG_CAN_ACR3 |=	((can_filter_msg->std_id2 & 0x7)<<5);			//设置过滤器2的标准ID低三位
				REG_CAN_ACR3 |= ((can_filter_msg->rtr &0x01) << 4);				//设置过滤器2的RTR
				
				REG_CAN_AMR2 = 0;												//对比过滤器2的标准ID高八位
				REG_CAN_AMR3 = 0xF;												//对比过滤器2的标准ID低三位和RTR,不对比过滤器1的第1个数据低四位
									
			}
			else																//扩展格式
			{	
				REG_CAN_ACR0 |= ((can_filter_msg->ext_id1 & 0x1FE00000) >> 21);	//设置过滤器1的扩展ID的[28:21]
				REG_CAN_ACR1 |=	((can_filter_msg->ext_id1 & 0x1FE000) >> 13);	//设置过滤器1的扩展ID的[20:14]
				
				REG_CAN_AMR0 = 0;												//对比过滤器1的扩展ID的[28:21]
				REG_CAN_AMR1 = 0;   											//对比过滤器1的扩展ID的[20:14]
				
				REG_CAN_ACR2 |= ((can_filter_msg->ext_id2 & 0x1FE00000) >> 21);	//设置过滤器2的扩展ID的[28:21]
				REG_CAN_ACR3 |=	((can_filter_msg->ext_id2 & 0x1FE000) >> 13);	//设置过滤器2的扩展ID的[20:14]
				
				REG_CAN_AMR2 = 0;												//对比过滤器2的扩展ID的[28:21]
				REG_CAN_AMR3 = 0;   											//对比过滤器2的扩展ID的[20:14]
				
				
			}
		}
	}
	else
	{
		REG_CAN_AMR0 = 0xff;
		REG_CAN_AMR1 = 0xff;
		REG_CAN_AMR2 = 0xff;
		REG_CAN_AMR3 = 0xff;
	}
	REG_CAN_MR &= ~(1<<1);													//CAN进入正常模式
	REG_CAN_MR &= ~(1<<2); 													//CAN退出复位模式

}
/************************************************************************
 * function   : can_send_data
 * Description: can send data
 * input : can_tx_msg
 * return: none
 ************************************************************************/
void can_send_data(S_Can_Tx_Msg *can_tx_msg)
{
	if(can_tx_msg->ide)																			//29bit扩展格式
	{	
		/*1st*/
		REG_CAN_TXBUF0 = (can_tx_msg->len) | (can_tx_msg->rtr << 6) | (can_tx_msg->ide << 7);	//写入数据长度、RTR位、IDE位
		REG_CAN_TXBUF1 = ((can_tx_msg->ext_id & 0x1FE00000)>> 21);								//写入扩展ID的[28:21]
		REG_CAN_TXBUF2 = ((can_tx_msg->ext_id & 0x1FE000)>> 13);                                //写入扩展ID的[20:13]
		REG_CAN_TXBUF3 = ((can_tx_msg->ext_id & 0x1FE0) >> 5);	                                //写入扩展ID的[12:5]
		
		/*2nd*/
		REG_CAN_TXBUF0 = (can_tx_msg->rtr << 2) | ((can_tx_msg->ext_id & 0x1F) << 3);			//写入RTR位和扩展ID的[4:0]
		REG_CAN_TXBUF1 = can_tx_msg->send_data[0];												//写入第1个数据
		REG_CAN_TXBUF2 = can_tx_msg->send_data[1];                                              //写入第2个数据
		REG_CAN_TXBUF3 = can_tx_msg->send_data[2];                                              //写入第3个数据
		
		/*3rd*/
		REG_CAN_TXBUF0 = can_tx_msg->send_data[3];												//写入第4个数据
		REG_CAN_TXBUF1 = can_tx_msg->send_data[4];												//写入第5个数据
		REG_CAN_TXBUF2 = can_tx_msg->send_data[5];												//写入第6个数据
		REG_CAN_TXBUF3 = can_tx_msg->send_data[6];												//写入第7个数据
		
		/*4th*/
		REG_CAN_TXBUF0 = can_tx_msg->send_data[7];												//写入第8个数据
		REG_CAN_TXBUF1 = 0;
		REG_CAN_TXBUF2 = 0;
		REG_CAN_TXBUF3 = 0;
	}
	else																						//11bit标准格式
	{
		/*1st*/
		REG_CAN_TXBUF0 = (can_tx_msg->len) | (can_tx_msg->rtr << 6) | (can_tx_msg->ide << 7);	//写入数据长度、RTR位、IDE位
		REG_CAN_TXBUF1 = ((can_tx_msg->std_id & 0x7F8) >> 3);									//写入标准ID高八位
		REG_CAN_TXBUF2 = (can_tx_msg->rtr << 4)	| ((can_tx_msg->std_id & 0x7)<< 5);				//写入RTR位和标准ID低三位
		REG_CAN_TXBUF3 = can_tx_msg->send_data[0];												//写入第1个数据
		
		/*2nd*/
		REG_CAN_TXBUF0 = can_tx_msg->send_data[1];												//写入第2个数据			
		REG_CAN_TXBUF1 = can_tx_msg->send_data[2];												//写入第3个数据	
		REG_CAN_TXBUF2 = can_tx_msg->send_data[3];												//写入第4个数据	
		REG_CAN_TXBUF3 = can_tx_msg->send_data[4];												//写入第5个数据	
		
		/*3rd*/
		REG_CAN_TXBUF0 = can_tx_msg->send_data[5];												//写入第6个数据	
		REG_CAN_TXBUF1 = can_tx_msg->send_data[6];												//写入第7个数据	
		REG_CAN_TXBUF2 = can_tx_msg->send_data[7];												//写入第8个数据	
		REG_CAN_TXBUF3 = 0;
	}
	REG_CAN_CMR |= (1<<2);																		//启动发送
	while(!(REG_CAN_SR & (1<<5)));																//等待发送完成
}
/************************************************************************
 * function   : can_recv_data
 * Description: can_recv_data
 * input :*can_rx_msg
 * return: none
 ************************************************************************/

void can_recv_data(S_Can_Rx_Msg *can_rx_msg)
{
	uint8_t rx_data;

	rx_data = REG_CAN_RXBUF0;
	can_rx_msg->ide = (uint8_t)((rx_data & 0x80)>>7);		//获取IDE,判断ID格式
	can_rx_msg->len = (uint8_t)((rx_data & 0xF));				//获取数据len
	can_rx_msg->rtr = (uint8_t)((rx_data & 0x40)>>6);		//获取RTR
	
	if(can_rx_msg->ide)											//扩展格式
	{	
		/*1st*/
		rx_data = REG_CAN_RXBUF1;
		can_rx_msg->ext_id = ((uint32_t)rx_data << 21);			//获取扩展ID的[28:21]
		
		rx_data = REG_CAN_RXBUF2;
		can_rx_msg->ext_id |= ((uint32_t)rx_data << 13);		//获取扩展ID的[20:13]
		
		rx_data = REG_CAN_RXBUF3;
		can_rx_msg->ext_id |= ((uint32_t)rx_data << 5);			//获取扩展ID的[12:5]
		
		/*2nd*/
		rx_data = REG_CAN_RXBUF0;
		can_rx_msg->ext_id |= ((uint32_t)rx_data>>3);			//获取扩展ID的[4:0]
		
		can_rx_msg->recv_data[0] = REG_CAN_RXBUF1;			//获取第1个数据
		can_rx_msg->recv_data[1] = REG_CAN_RXBUF2;      //获取第2个数据
		can_rx_msg->recv_data[2] = REG_CAN_RXBUF3;      //获取第3个数据
		
		if(can_rx_msg->len >= 8)
		{
			/*3rd*/
			can_rx_msg->recv_data[3] = REG_CAN_RXBUF0;			//获取第4个数据

			can_rx_msg->recv_data[4] = REG_CAN_RXBUF1;       //获取第5个数据
			can_rx_msg->recv_data[5] = REG_CAN_RXBUF2;       //获取第6个数据
			can_rx_msg->recv_data[6] = REG_CAN_RXBUF3;			 //获取第7个数据
		
			/*4th*/
			can_rx_msg->recv_data[7] = REG_CAN_RXBUF0;				//获取第8个数据
			rx_data = REG_CAN_RXBUF1;
			rx_data = REG_CAN_RXBUF2;
			rx_data = REG_CAN_RXBUF3;
		}
		else if(can_rx_msg->len >= 4)
		{
			can_rx_msg->recv_data[3] = REG_CAN_RXBUF0;				//获取第1个数据
			can_rx_msg->recv_data[4] = REG_CAN_RXBUF1;        //获取第2个数据
			can_rx_msg->recv_data[5] = REG_CAN_RXBUF2;       	//获取第3个数据
			can_rx_msg->recv_data[6] = REG_CAN_RXBUF3;				//获取第4个数据
		}
	}
	else														//标准格式
	{	
		/*1st*/
		can_rx_msg->len = (uint8_t)((rx_data & 0xF));			//获取数据len
		can_rx_msg->rtr = (uint8_t)((rx_data & 0x40)>>6);		//获取RTR
		
		rx_data = REG_CAN_RXBUF1;
		can_rx_msg->std_id = ((uint32_t)rx_data << 3);			//获取标准ID高八位
		
		rx_data = REG_CAN_RXBUF2;
		can_rx_msg->std_id |= (((uint32_t)rx_data & 0xE0)>>5);	//获取标准ID低三位
		can_rx_msg->rtr = ((rx_data & 0x10) >> 4);				//获取RTR位
		
		can_rx_msg->recv_data[0] = REG_CAN_RXBUF3;				//获取第1个数据	
		
		if(can_rx_msg->len >= 6)
		{
			can_rx_msg->recv_data[1] = REG_CAN_RXBUF0;				//获取第2个数据
			can_rx_msg->recv_data[2] = REG_CAN_RXBUF1;				//获取第3个数据
			can_rx_msg->recv_data[3] = REG_CAN_RXBUF2;				//获取第4个数据
			can_rx_msg->recv_data[4] = REG_CAN_RXBUF3;				//获取第5个数据
			can_rx_msg->recv_data[5] = REG_CAN_RXBUF0;				//获取第6个数据
			can_rx_msg->recv_data[6] = REG_CAN_RXBUF1;				//获取第7个数据
			can_rx_msg->recv_data[7] = REG_CAN_RXBUF2;				//获取第8个数据
			rx_data = REG_CAN_RXBUF3;
		}
		else
		{
			can_rx_msg->recv_data[1] = REG_CAN_RXBUF0;				//获取第2个数据
			can_rx_msg->recv_data[2] = REG_CAN_RXBUF1;				//获取第3个数据
			can_rx_msg->recv_data[3] = REG_CAN_RXBUF2;				//获取第4个数据
			can_rx_msg->recv_data[4] = REG_CAN_RXBUF3;				//获取第5个数据
		}
	}
	
	REG_CAN_ISR = 0x08;

}

/************************************************************************
 * function   : can_get_sr_reg
 * Description: can get_sr_reg, 获取can状态寄存器值
 * input : 
 * return: temp
 ************************************************************************/
uint8_t can_get_sr_reg(void)
{
	uint8_t temp;
	
	temp = REG_CAN_SR;
	
	return temp;
}

/************************************************************************
 * function   : can_get_rmc_reg
 * Description: can get rmc reg, 获取can rx fifo data 个数
 * input : 
 * return: temp
 ************************************************************************/
uint8_t can_get_rmc_reg(void)
{
	uint8_t temp;
	
	temp = REG_CAN_RMC;
	
	return temp;
}

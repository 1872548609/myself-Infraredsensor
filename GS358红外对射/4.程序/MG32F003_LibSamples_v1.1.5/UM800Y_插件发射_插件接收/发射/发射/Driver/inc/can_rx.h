/***********************************************************************
 * Copyright (c)  2017 - 2021, Unicmicro Co.,Ltd .
 * All rights reserved.
 * Filename    : can.h
 * Description : can driver header file
 * Author(s)   : Will
 * version     : V1.0
 * Modify date : 2021-06-24
 ***********************************************************************/
#ifndef __CAN_H__
#define __CAN_H__

#include "um800y.h"

#define CAN_SINGLE_FILTER	1
#define CAN_DOUBLE_FILTER	0

#define CAN_IMR_ALIM		(1<<6)	//仲裁丢失中断使能位
#define CAN_IMR_EWIM		(1<<5)	//错误警告中断使能位
#define CAN_IMR_EPIM		(1<<4)	//错误被动中断使能位
#define CAN_IMR_RIM			(1<<3)	//接收中断使能位
#define CAN_IMR_TIM			(1<<2)	//发送中断使能位
#define CAN_IMR_BEIM		(1<<1)	//总线错误中断使能位
#define CAN_IMR_DOIM		(1<<0)	//接收数据溢出中断使能位

#define CAN_SR_RBS			(1<<7)	//FIFO中至少有一条消息
#define CAN_SR_DSO			(1<<6)	//数据溢出
#define CAN_SR_TBS			(1<<5)	//发送buffer状态
#define CAN_SR_RS			(1<<3)	//正在接受
#define CAN_SR_TS			(1<<2)	//正在传输
#define CAN_SR_ES			(1<<1)	//错误警告
#define CAN_SR_BS			(1<<0)	//总线状态

typedef struct
{
	uint32_t std_id;		/* 报文ID */		
	uint32_t ext_id;		/* 扩展ID */
	uint8_t ide;     		/* 报文ID类型 */
	uint8_t rtr;     		/* 消息类型 */
	uint8_t len;     		/* 指定将要使用的帧的长度传输。此参数可以是介于0到8 */
	uint8_t send_data[8]; 	/* 数据 */
}S_Can_Tx_Msg;

typedef struct
{
	uint32_t std_id;		/* 报文ID */			
	uint32_t ext_id;		/* 扩展ID */
	uint8_t ide;     		/* 报文ID类型 */
	uint8_t rtr;     		/* 消息类型 */
	uint8_t len;     		/* 指定将要使用的帧的长度传输。此参数可以是介于0到8 */
	uint8_t recv_data[8]; 	/* 数据 */
}S_Can_Rx_Msg;

typedef struct
{
	uint32_t std_id1;		/* 报文ID1  filter1*/   
	uint32_t std_id2;		/* 报文ID2  filter2*/
	uint32_t ext_id1;		/* 扩展ID1  filter1*/
	uint32_t ext_id2;		/* 扩展ID2  filter2*/
	uint8_t ide;     		/* 报文ID类型 */
	uint8_t rtr;     		/* 消息类型 */

}S_Can_Filter_Msg;

#define CAN_IRQ_ENABLE  	1
#define CAN_IRQ_DISABLE  	0

#define CAN_RX_ADDR			0x100	//接收缓存基地址设置为(0x20003000+CAN_RX_ADDR)

#define CAN_FILTER_ENABLE	1		//过滤器使能
#define CAN_FILTER_DISABLE	0		//过滤器关闭

#define CAN_SINGLE_FILTER	1		//单过滤器
#define CAN_DOUBLE_FILTER	0		//双过滤器

#define CAN0_FORMAT     	1
#define CAN0_STD_FORMAT     0		//标准模式
#define CAN0_EXT_FORMAT     1 		//扩展模式

#define CAN_IDE				0		//IDE设置
#define CAN_IDE_STD_FORMAT	0		//标准模式设置0;
#define CAN_IDE_EXT_FORMAT	1		//扩展模式设置1;

#define CAN_RTR				0		//RTR设置
#define CAN_RTR_DATA		0		//数据帧设置为0
#define CAN_RTR_REMOTE		1		//遥控帧设置为1



#define CAN_RATE_1M       	0
#define CAN_RATE_500K     	1
#define CAN_RATE_250K     	2
#define CAN_RATE_125K     	3


void can_init(uint8_t baudrate);
void can_irq_init(uint8_t irq_able,void (*pfunc)());
void can_filter_config(S_Can_Filter_Msg *can_filter_msg,uint8_t filter_mode, uint8_t filter_en);
void can_send_data(S_Can_Tx_Msg *can_tx_msg);
void can_recv_data(S_Can_Rx_Msg *can_rx_msg);
uint8_t can_get_sr_reg(void);
uint8_t can_get_rmc_reg(void);
	
#endif
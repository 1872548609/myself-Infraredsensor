/************************************************************************************************/
/**
* @file               i2c_bsp.h
* @author             MCU Ecosystem Development Team
* @brief              I2C BSP头文件。
*
*
**************************************************************************************************
* @attention
* Copyright (c) CEC Huada Electronic Design Co.,Ltd. All rights reserved.
*
**************************************************************************************************
*/

/* 避免头文件重复引用 */
#ifndef I2C_BSP_H
#define I2C_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------includes--------------------------------------------*/
#include "common.h"

/*-------------------------------------------define---------------------------------------------*/
/* I2C延时操作 */
#define CLK_DELAY                       (16U)
#define ACK_DELAY                       (6U)
#define USER_DELAY                      (5U)

/* I2C返回ACK的类型 */
#define RET_ACK                         (0x00000000U)
#define RET_NACK                        (0x00000001U)

/* I2C主机读写请求 */
#define REQ_WRITE                       (0U)
#define REQ_READ                        (1U)

/*-------------------------------------------functions------------------------------------------*/
void bsp_i2c_generate_start(void);
void bsp_i2c_generate_stop(void);
void bsp_i2c_generate_ack(uint32_t ack_type);
uint32_t bsp_i2c_master_send_byte(uint8_t data);
uint8_t bsp_i2c_master_receive_byte(void);
void bsp_i2c_master_send(void);
void bsp_i2c_master_receive(void);


#ifdef __cplusplus
}
#endif

#endif /* I2C_BSP_H */


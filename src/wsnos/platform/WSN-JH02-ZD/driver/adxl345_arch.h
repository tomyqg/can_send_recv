/***************************************************************************
* File        : adxl345_arch.h
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/


#ifndef __ADXL345_ARCH_H
#define __ADXL345_ARCH_H
#include "common/dev/dev.h"

#define I2C_OK              0
#define I2C_FAIL            1

/* I2C address of the device */
#define ADXL345_ADDRESS			0x53
#define ADXL345_ADDRESS_W       0xA6 // (ADXL345_ADDRESS << 1) || 0x00
#define ADXL345_ADDRESS_R       0xA7 // (ADXL345_ADDRESS << 1) || 0x01

void acc_power_open(bool_t para);
void accelerated_iic_init(void);
void accelerated_iic_deinit(void);
void accelerated_port1_init(void);
void accelerated_port2_init(void);
void accelerated_int1_cfg(void);
void accelerated_int2_cfg(void);

bool_t adxl345_read_register( uint8_t reg_add , uint8_t *pvalue);
bool_t adxl345_write_register( uint8_t reg_add , uint8_t reg_value);
bool_t adxl345_read_buff(uint8_t reg_add , uint8_t *pregbuf , uint8_t  len);
#endif
/**
 * @brief       : this
 * @file        : bma253_arch.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017/06/09
 * change logs  :
 * Date       Version     Author        Note
 * 2017/06/09  v0.0.1  gang.cheng    first version
 */
#ifndef __BMA253_ARCH_H__
#define __BMA253_ARCH_H__

#define BMA253_I2C_TIMEOUT          1000

#define BMA253_SDA	                GPIO_PIN_7
#define BMA253_SCL	                GPIO_PIN_6

void bma253_iic_init(void);

void bma253_int_init(void (*int_1)(void), void (*int_2)(void));

int8_t bma253_read_regs(uint8_t i2c_addr,  uint8_t reg, uint8_t len, uint8_t *buf);

int8_t bma253_write_regs(uint8_t i2c_addr,  uint8_t reg, uint8_t len, uint8_t *buf);


#endif

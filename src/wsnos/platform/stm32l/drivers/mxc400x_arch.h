/**
 * @brief       : this
 * @file        : mxc400x_arch.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-06-01
 * change logs  :
 * Date       Version     Author        Note
 * 2017-06-01  v0.0.1  gang.cheng    first version
 */
#ifndef __MXC400X_ARCH_H__
#define __MXC400X_ARCH_H__

#define I2C_TIMEOUT                 (1000ul)

#define I2C1_SDA                    GPIO_PIN_7
#define I2C1_SCL                    GPIO_PIN_6

bool_t mxc400x_power_ctrl(bool_t res);

void mxc400x_iic_init(void);

void mxc400x_int_init(gpio_irq_handler_t handler);

uint8_t mxc400x_read_register(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *buffer);

uint8_t mxc400x_write_register(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *buffer);


#endif

#ifndef __MPL3115A2_ARCH_H__
#define __MPL3115A2_ARCH_H__

#define MPL3115A2_ADDR                  (0x60)
#define I2C_TIMEOUT                     1000

#define I2C1_SDA	                GPIO_PIN_7
#define I2C1_SCL	                GPIO_PIN_6

bool_t mpl3115a2_read_reg(uint8_t reg_add, uint8_t *pvalue);
bool_t mpl3115a2_write_reg(uint8_t reg_add , uint8_t value);
bool_t mpl3115a2_read_regs(uint8_t reg_add, uint8_t *pvalue, uint8_t size);
bool_t mpl3115a2_write_regs(uint8_t reg_add, uint8_t *pvalue, uint8_t size);
void pressure_port1_init(void);
void pressure_int1_cfg(void);
void dev_iic_init(void);
void dev_iic_deinit(void);
#endif

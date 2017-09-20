#pragma once

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define ADXL345_I2C_ADDRESS                         0x38
#define I2C_SPEEDCLOCK                             400000            // I2C通信速率(最大为400K)
#define I2C_DUTYCYCLE                              I2C_DUTYCYCLE_2   // I2C占空比模式：1/2 

#define ADXL345_I2Cx                                I2C1
#define ADXL345_I2C_RCC_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define ADXL345_I2C_RCC_CLK_DISABLE()               __HAL_RCC_I2C1_CLK_DISABLE()

#define ADXL345_I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADXL345_I2C_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOB_CLK_DISABLE()   
#define ADXL345_I2C_GPIO_PORT                       GPIOB   
#define ADXL345_I2C_SCL_PIN                         GPIO_PIN_6
#define ADXL345_I2C_SDA_PIN                         GPIO_PIN_7

extern I2C_HandleTypeDef hi2c_adxl345;

#define I2C_OK              0
#define I2C_FAIL            1


void adxl345_i2c_init(void);

bool_t adxl345_read_register( uint8_t reg_add , uint8_t *pvalue);

bool_t adxl345_write_register( uint8_t reg_add , uint8_t reg_value);

bool_t adxl345_read_buff(uint8_t reg_add , uint8_t *pregbuf , uint8_t  len);

void accelerated_int_cfg(void);



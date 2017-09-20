/***************************************************************************
* File        : mamx845x_arch.h
* Summary     : MMA8451,2,3Q Driver
* Version     : v0.1
* Author      : wangjifang
* Date        : 2017/8/18
* Change Logs :
* Date       Versi on     Author     Notes
* 2017/8/7         v0.1            wangjifang        first version
***************************************************************************/
#ifndef _MMA845X_ARCH_H_
#define _MMA845X_ARCH_H_

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define MMA845X_I2C_ADDRESS                         0x38
#define I2C_SPEEDCLOCK                             400000                   // I2C通信速率(最大为400K)
#define I2C_DUTYCYCLE                              I2C_DUTYCYCLE_2          // I2C占空比模式：1/2 

#define MMA845X_I2Cx                                I2C2
#define MMA845X_I2C_RCC_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
#define MMA845X_I2C_RCC_CLK_DISABLE()               __HAL_RCC_I2C2_CLK_DISABLE()

#define MMA845X_I2C_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define MMA845X_I2C_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOB_CLK_DISABLE()   
#define MMA845X_I2C_GPIO_PORT                       GPIOB   
#define MMA845X_I2C_SCL_PIN                         GPIO_PIN_10
#define MMA845X_I2C_SDA_PIN                         GPIO_PIN_11

extern I2C_HandleTypeDef hi2c_mma845x;


void mma845x_i2c_init(void);

bool_t mma845x_read_register( uint8_t reg_add , uint8_t *pvalue);

bool_t mma845x_write_register( uint8_t reg_add , uint8_t reg_value);

bool_t mma845x_read_buff(uint8_t reg_add , uint8_t *pregbuf , uint8_t  len);

void accelerated_int_cfg(void);

#endif  /* _MMA845X_H_ */

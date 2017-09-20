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
#include "gz_sdk.h"
#include "mxc400x_arch.h"

static I2C_HandleTypeDef I2C1_struct;

bool_t mxc400x_power_ctrl(bool_t res)
{
    return TRUE;
}

void mxc400x_iic_init(void)
{
    GPIO_InitTypeDef I2C1_GPIO_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE(); // 初始化GPIO

    I2C1_GPIO_struct.Pin = I2C1_SDA | I2C1_SCL;
    I2C1_GPIO_struct.Mode = GPIO_MODE_AF_OD;
    I2C1_GPIO_struct.Pull = GPIO_PULLUP;
    I2C1_GPIO_struct.Speed = GPIO_SPEED_LOW;
    I2C1_GPIO_struct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &I2C1_GPIO_struct);


    __HAL_RCC_I2C1_CLK_ENABLE(); // 初始化I2C
    I2C1_struct.Instance = I2C1;
    I2C1_struct.Init.ClockSpeed = 100000;
    I2C1_struct.Init.DutyCycle = I2C_DUTYCYCLE_2;
    I2C1_struct.Init.OwnAddress1 = 0;
    I2C1_struct.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2C1_struct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    I2C1_struct.Init.OwnAddress2 = 0;
    I2C1_struct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    I2C1_struct.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(&I2C1_struct);
}

void mxc400x_int_init(gpio_irq_handler_t handler)
{
    ;
}

uint8_t mxc400x_read_register(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *buffer)
{
    uint8_t size = 0;

    size = (HAL_I2C_Mem_Read( &I2C1_struct, (uint16_t)dev_addr << 1, reg_addr,
                              I2C_MEMADD_SIZE_8BIT, buffer, len, I2C_TIMEOUT ) == HAL_OK) ? len : 0;

    return size;
}

uint8_t mxc400x_write_register(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *buffer)
{
    uint8_t size = 0;

    size = (HAL_I2C_Mem_Write( &I2C1_struct, (uint16_t)dev_addr << 1, reg_addr,
                               I2C_MEMADD_SIZE_8BIT, buffer, len, I2C_TIMEOUT ) == HAL_OK) ? len : 0;

    return size;
}
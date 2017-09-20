/***************************************************************************
* File        : pressure_defs.h
* Summary     : 
* Version     : v0.1
* Author      : zhouyimeng
* Date        : 2017/02/28
* Change Logs :
* Date       Versi on     Author     Notes
* 2017/02/28         v0.1            zhouyimeng        first version
***************************************************************************/
//#include "./mac/LoRaMac_arch.h"
#include "gz_sdk.h"
#include "mpl3115a2_arch.h"

I2C_HandleTypeDef I2C1_struct;

bool_t mpl3115a2_read_regs(uint8_t reg_add, uint8_t *pvalue, uint8_t size)
{
    bool_t status = FALSE;
    
    status = (HAL_I2C_Mem_Read( &I2C1_struct, (uint16_t)MPL3115A2_ADDR<<1, reg_add, 
                               I2C_MEMADD_SIZE_8BIT, pvalue, size, I2C_TIMEOUT ) == HAL_OK) ? TRUE : FALSE;
    
    return status;
}

bool_t mpl3115a2_read_reg(uint8_t reg_add, uint8_t *pvalue)
{
    return mpl3115a2_read_regs(reg_add, pvalue, 1);
}

bool_t mpl3115a2_write_regs(uint8_t reg_add, uint8_t *pvalue, uint8_t size)
{
    bool_t status = FALSE;
    
    status = (HAL_I2C_Mem_Write(&I2C1_struct, (uint16_t)MPL3115A2_ADDR<<1, reg_add, 
                               I2C_MEMADD_SIZE_8BIT, pvalue, size, I2C_TIMEOUT )== HAL_OK) ? TRUE : FALSE;
    
    return status;
}

bool_t mpl3115a2_write_reg(uint8_t reg_add , uint8_t value)
{
    return mpl3115a2_write_regs(reg_add, &value, 1);
}

void pressure_port1_init(void)
{
    
}

void pressure_int1_cfg(void)
{
    
}

void dev_iic_init(void)
{
    GPIO_InitTypeDef I2C1_GPIO_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE(); // 初始化GPIO

    I2C1_GPIO_struct.Pin = I2C1_SDA|I2C1_SCL;
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

void dev_iic_deinit(void)
{
    HAL_GPIO_DeInit(GPIOB, I2C1_SDA);
    HAL_GPIO_DeInit(GPIOB, I2C1_SCL);
    HAL_I2C_DeInit(&I2C1_struct);
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_I2C1_CLK_DISABLE();
}
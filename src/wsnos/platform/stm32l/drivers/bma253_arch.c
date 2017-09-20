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
#include "common/lib/lib.h"
#include "board.h"
#include "gpio.h"
#include "bma253_arch.h"

static I2C_HandleTypeDef I2C1_struct;

//static pin_id_t bma253_int1_gpio;
static pin_id_t bma253_int2_gpio;

static void i2c_lock_init(void)
{    
    pin_id_t i2c_lock_gpio;
    GPIO_NAME_TO_PIN(&i2c_lock_gpio, PB_6);
    gpio_init(i2c_lock_gpio, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);

    for (uint8_t i = 0; i < 9; i++ )
    {
        gpio_set(i2c_lock_gpio);
        delay_us(30);
        gpio_clr(i2c_lock_gpio);
        delay_us(30);
    }
}

void bma253_iic_init(void)
{
    i2c_lock_init();
    
    GPIO_InitTypeDef I2C1_GPIO_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE(); // 初始化GPIO
    __HAL_RCC_I2C1_CLK_ENABLE(); // 初始化I2C
    
    I2C1_GPIO_struct.Pin = BMA253_SDA|BMA253_SCL;
    I2C1_GPIO_struct.Mode = GPIO_MODE_AF_OD;
    I2C1_GPIO_struct.Pull = GPIO_PULLUP;
    I2C1_GPIO_struct.Speed = GPIO_SPEED_HIGH;
    I2C1_GPIO_struct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &I2C1_GPIO_struct);

    I2C1_struct.Instance = I2C1;
    I2C1_struct.Init.ClockSpeed = 400000;
    I2C1_struct.Init.DutyCycle = I2C_DUTYCYCLE_2;
    I2C1_struct.Init.OwnAddress1 = 0;
    I2C1_struct.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2C1_struct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    I2C1_struct.Init.OwnAddress2 = 0;
    I2C1_struct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    I2C1_struct.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(&I2C1_struct);
}

void bma253_int_init(void (*int_1)(void), void (*int_2)(void))
{
//    GPIO_NAME_TO_PIN(&bma253_int1_gpio, PA_2);
//    gpio_init(bma253_int1_gpio, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0);
//    gpio_interrupt_set(bma253_int1_gpio, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, int_1 );
    
    GPIO_NAME_TO_PIN(&bma253_int2_gpio, PB_5);
    gpio_init(bma253_int2_gpio, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0);
    gpio_interrupt_set(bma253_int2_gpio, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, int_2 );
    
}

int8_t bma253_read_regs(uint8_t i2c_addr,  uint8_t reg, uint8_t len, uint8_t *buf)
{
    if(HAL_I2C_Mem_Read(&I2C1_struct, (uint16_t)i2c_addr<<1, reg, 
                               I2C_MEMADD_SIZE_8BIT, buf, len, BMA253_I2C_TIMEOUT )!= HAL_OK)
    {
        return -1;
    }
    
    return 0;
}

int8_t bma253_write_regs(uint8_t i2c_addr,  uint8_t reg, uint8_t len, uint8_t *buf)
{
    if(HAL_I2C_Mem_Write(&I2C1_struct, (uint16_t)i2c_addr<<1, reg, 
                               I2C_MEMADD_SIZE_8BIT, buf, len, BMA253_I2C_TIMEOUT )!= HAL_OK)
    {
        return -1;
    }
    
    return 0;
}

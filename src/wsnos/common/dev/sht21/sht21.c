/***************************************************************************
* File        : sht21.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/
#include "gz_sdk.h"
#include "sht21.h"

osel_device_t sht21_device;
static ht_senser_read_type_e sht21_cmd_type = TYPE_READ_HUM;

static bool_t sht21_read_user_reg(uint8_t *val)
{
    HUMITURE_I2C_WRITE_START();
    HUMITURE_I2C_SEND_CHAR(READ_REG_COMMAND);
    HUMITURE_I2C_WAIT_ADDR_ACK();

    HUMITURE_I2C_READ_START();
    HUMITURE_I2C_WAIT_ADDR_ACK();
    HUMITURE_I2C_SEND_STOP_BIT();
    HUMITURE_I2C_RECCEIVE_CHAR(*val);
    HUMITURE_I2C_WAIT_STOP();
    if (HUMITURE_I2C_IS_NO_ACK())
    {
        return FALSE;
    }
    return TRUE;
}

static bool_t sht21_write_user_reg(uint8_t val)
{
    HUMITURE_I2C_WRITE_START();
    HUMITURE_I2C_SEND_CHAR(WRITE_REG_COMMAND);
    HUMITURE_I2C_WAIT_ADDR_ACK();

    HUMITURE_I2C_SEND_CHAR(val);
    HUMITURE_I2C_SEND_STOP_BIT();
    HUMITURE_I2C_WAIT_STOP();
    if (HUMITURE_I2C_IS_NO_ACK())
    {
        return FALSE;
    }
    return TRUE;
}

/**************************************************************************/
static osel_int8_t sht21_driver_init(osel_device_t *dev)
{
    DBG_ASSERT((HUMITURE_RESOLUTION == HUMITURE_HIGH_RESOLUTION)
               || (HUMITURE_RESOLUTION == HUMITURE_LOW_RESOLUTION) __DBG_LINE);
    uint8_t val;
    sht20_pow_open(TRUE);
    sht20_i2c_init();
    delay_ms(15);
    if (HUMITURE_RESOLUTION == HUMITURE_LOW_RESOLUTION)
    {
        if (FALSE == sht21_read_user_reg(&val))
        {
            return FALSE;
        }
        val &= ~BIT7;
        val |= BIT0;
        return sht21_write_user_reg(val);
    }
    else
    {
        if (FALSE == sht21_read_user_reg(&val))
        {
            return FALSE;
        }
        val &= ~(BIT7 + BIT0);
        return sht21_write_user_reg(val);
    }
}

bool_t humiture_sensor_read_h(uint16_t *pval)
{
    uint8_t rh_hi;
    uint8_t rh_lo;
    uint8_t check;

    HUMITURE_I2C_WRITE_START();
    HUMITURE_I2C_SEND_CHAR(TRIGGER_RH_MEASUREMENT_HOLD_COMMAND);
    HUMITURE_I2C_WAIT_ADDR_ACK();
    HUMITURE_I2C_READ_START();
    HUMITURE_I2C_WAIT_ADDR_ACK();
    HUMITURE_I2C_RECCEIVE_CHAR(rh_hi);
    HUMITURE_I2C_RECCEIVE_CHAR(rh_lo);
    HUMITURE_I2C_SEND_STOP_BIT();
    HUMITURE_I2C_RECCEIVE_CHAR(check);
    HUMITURE_I2C_WAIT_STOP();
    if (HUMITURE_I2C_IS_NO_ACK())
    {
        return FALSE;
    }
    check = check;
    *pval = (((uint16_t)rh_hi) << 8) | rh_lo;
    *pval &= ~0x0003;
    return TRUE;
}

bool_t humiture_sensor_read_t(uint16_t *pval)
{
    uint8_t t_hi;
    uint8_t t_lo;
    uint8_t check;

    HUMITURE_I2C_WRITE_START();
    HUMITURE_I2C_SEND_CHAR(TRIGGER_T_MEASUREMENT_HOLD_COMMAND);
    HUMITURE_I2C_WAIT_ADDR_ACK();
    HUMITURE_I2C_READ_START();
    HUMITURE_I2C_WAIT_ADDR_ACK();
    HUMITURE_I2C_RECCEIVE_CHAR(t_hi);
    HUMITURE_I2C_RECCEIVE_CHAR(t_lo);
    HUMITURE_I2C_SEND_STOP_BIT();
    HUMITURE_I2C_RECCEIVE_CHAR(check);
    HUMITURE_I2C_WAIT_STOP();
    if (HUMITURE_I2C_IS_NO_ACK())
    {
        return FALSE;
    }
    check = check;
    *pval = (((uint16_t)t_hi) << 8) | t_lo;
    *pval &= ~0x0003;
    return TRUE;
}

osel_uint32_t sht21_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    if(buffer == NULL)
    {
        return FALSE;
    }
    
    bool_t read_result = FALSE;
    sht21_cmd_type = (ht_senser_read_type_e)pos;
    switch(sht21_cmd_type)
    {
    case TYPE_READ_HUM:
        read_result = humiture_sensor_read_h(buffer);
        break;
        
    case TYPE_READ_TMP:
        read_result = humiture_sensor_read_t(buffer);
        break;
        
    default:
        break;
    }
    return read_result;
}

static osel_uint32_t sht21_driver_control(osel_device_t *dev,
                         osel_uint8_t cmd,
                         void *args)
{
    sht21_cmd_type = (ht_senser_read_type_e)cmd;
    return 0;
}

static osel_int8_t sht21_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start sht21
    return 0;
}


static osel_int8_t sht21_driver_close(osel_device_t *dev)
{
    //*< make sht21 in sleep mode
    return 0;
}

void sht21_device_init(void)
{
    sht21_device.init = sht21_driver_init;
    sht21_device.read = sht21_driver_read;
    sht21_device.control = sht21_driver_control;
    sht21_device.open = sht21_driver_open;
    sht21_device.close = sht21_driver_close;
    
    osel_device_register(&sht21_device, HT_SENSOR_NAME, OSEL_DEVICE_FLAG_RDONLY);
}
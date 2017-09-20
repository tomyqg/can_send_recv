/**
 * @brief       : this implement of monolithic and wafer level packaged three-
 *              : axis accelerometer
 * @file        : mxc400x.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-05-31
 * change logs  :
 * Date       Version     Author        Note
 * 2017-05-31  v0.0.1  gang.cheng    first version
 */
#include "gz_sdk.h"
#include "mxc400x.h"

osel_device_t mxc400x_device;
static MXC400X_ADDRESS_CODE_E mxc400x_addr_code = MXC400X_0;

static void mxc400x_int_handler(void)
{
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);

    //*< do something in interrupt
    //

    OSEL_EXIT_CRITICAL(s);
}

static osel_int8_t mxc400x_driver_init(osel_device_t *dev)
{
    uint8_t reg_val = 0x00;
    MXC400X_ADDRESS_CODE_E *addr_code = (MXC400X_ADDRESS_CODE_E *)dev->user_data;
    mxc400x_power_ctrl(TRUE);

    mxc400x_iic_init();

    mxc400x_int_init(&mxc400x_int_handler);

    if (mxc400x_read_register(*addr_code, MAX400X_REG_DEVICE_ID, 1, &reg_val) == 0)
        return -1;

    if (reg_val != MAC400X_DEVICE_ID_VAL)
        return -2;

    //*< close interrupt
    reg_val = 0x00;
    if (mxc400x_write_register(*addr_code, MAX400X_REG_INT_MASK0, 1, &reg_val) == 0)
        return -1;
    if (mxc400x_write_register(*addr_code, MAX400X_REG_INT_MASK1, 1, &reg_val) == 0)
        return -1;

    reg_val = MXC400X_CMD_RANGE_8G;
    if (mxc400x_write_register(*addr_code, MAX400X_REG_CONTROL, 1, &reg_val) == 0)
        return -1;

    return 0;
}

static osel_int8_t mxc400x_driver_open(osel_device_t *dev)
{
    uint8_t reg_val = 0x00;
    MXC400X_ADDRESS_CODE_E *addr_code = (MXC400X_ADDRESS_CODE_E *)dev->user_data;

    reg_val = MXC400X_CMD_RANGE_8G;
    if (mxc400x_write_register(*addr_code, MAX400X_REG_CONTROL, 1, &reg_val) == 0)
        return -1;

    return 0;
}

static osel_int8_t mxc400x_driver_close(osel_device_t *dev)
{
    uint8_t reg_val = 0x00;
    MXC400X_ADDRESS_CODE_E *addr_code = (MXC400X_ADDRESS_CODE_E *)dev->user_data;

    reg_val = MAC400X_CMD_POWER_DOWN;
    if (mxc400x_write_register(*addr_code, MAX400X_REG_CONTROL, 1, &reg_val) == 0)
        return -1;

    return 0;
}

static osel_uint32_t osel_device_read (osel_device_t *dev,
                                       osel_uint32_t  pos,
                                       void          *buffer,
                                       osel_uint32_t  size)
{
    uint8_t data_reg[7] = {0x00};
    int16_t data_acc[3] = {0x00};
    int8_t data_temp = 0;

    uint8_t output[16]; //*< 4 float

    if (size < 16)
        return 0;

    MXC400X_ADDRESS_CODE_E *addr_code = (MXC400X_ADDRESS_CODE_E *)dev->user_data;

    if (mxc400x_read_register(*addr_code, MAX400X_REG_XOUT_UPPER, 1, &data_reg) == 0)
        return -1;

    for (uint8_t i = 0; i < 3; i++)
    {
        data_acc[i] = (int16_t)(((uint16_t)data_reg[2 * i] << 8) | data_reg[2 * i + 1]) >> 4;
    }

    data_temp = (int16_t)data_reg[2 * i];

    fp32_t x_acc = data_acc / MXC400X_8G_SENSITIVITY;
    fp32_t y_acc = data_acc / MXC400X_8G_SENSITIVITY;
    fp32_t z_acc = data_acc / MXC400X_8G_SENSITIVITY;

    fp32_t temp = (fp32_t)data_temp * MXC400X_T_SENSITIVITY + MXC400X_T_ZERO;

    osel_memcpy(&output[0], (uint8_t *)&x_acc, 4);
    osel_memcpy(&output[4], (uint8_t *)&y_acc, 4);
    osel_memcpy(&output[8], (uint8_t *)&z_acc, 4);
    osel_memcpy(&output[12], (uint8_t *)&temp, 4);

    osel_memcpy(buffer, output, 16);

    return 16;
}

static osel_uint32_t mxc400x_driver_control(osel_device_t *dev, osel_uint8_t cmd, void *arg)
{
    uint8_t reg_val = 0x00;
    MXC400X_ADDRESS_CODE_E *addr_code = (MXC400X_ADDRESS_CODE_E *)dev->user_data;

    switch (cmd)
    {
    case MAC400X_CMD_POWER_DOWN:
    {
        reg_val = MAC400X_CMD_POWER_DOWN;
        if (mxc400x_write_register(*addr_code, MAX400X_REG_CONTROL, 1, &reg_val) == 0)
            return -1;
    }
    break;

    case MXC400X_CMD_RANGE_8G:
    {
        reg_val = MXC400X_CMD_RANGE_8G;
        if (mxc400x_write_register(*addr_code, MAX400X_REG_CONTROL, 1, &reg_val) == 0)
            return -1;
    }
    break;

    default:
        break;
    }

    return 0;
}

void mxc400x_device_init(MXC400X_ADDRESS_CODE_E mxc400x_type)
{
    mxc400x_device.init    = mxc400x_driver_init;
    mxc400x_device.read    = mxc400x_driver_read;
    mxc400x_device.open    = mxc400x_driver_open;
    mxc400x_device.close   = mxc400x_driver_close;
    mxc400x_device.control = mxc400x_driver_control;

    if ((mxc400x_type > MXC400X_7) || (mxc400x_type < MXC400X_0))
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }

    mxc400x_addr_code = mxc400x_type;
    mxc400x_device.user_data = &mxc400x_addr_code;

    osel_device_register(&mxc400x_device, ACC_SENSOR_NAME, OSEL_DEVICE_FLAG_RDWR);
}
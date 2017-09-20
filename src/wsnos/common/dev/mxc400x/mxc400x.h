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
#ifndef __MXC400X_H__
#define __MXC400X_H__

typedef enum
{
    MXC400X_0 = 0x10,
    MXC400X_1 = 0x11,
    MXC400X_2 = 0x12,
    MXC400X_3 = 0x13,
    MXC400X_4 = 0x14,
    MXC400X_5 = 0x15,
    MXC400X_6 = 0x16,
    MXC400X_7 = 0x17,
} MXC400X_ADDRESS_CODE_E;

/* MXC400X Register Map */
#define MXC400X_REG_INT_SRC0        0x00 // R   shake & change in orientation interrupt sources. 
#define MAX400X_REG_INT_CLR0        0x00 // W   shake & change in orientation interrupt clear. 
#define MAX4000_REG_INT_SRC1        0x01 // R   data ready interrupt status, tilt & orientation status.
#define MAX400X_REG_INT_CLR1        0x01 // W   data ready, tilt & data ready interrupt clear.   
#define MAX400X_REG_STATUS          0x02 // R   instataneous orientation status.
#define MAX400X_REG_XOUT_UPPER      0x03 // R   X-axis acceleration output MSB
#define MAX400X_REG_XOUT_LOWER      0x04 // R   X-axis acceleration output LSB
#define MAX400X_REG_YOUT_UPPER      0x05 // R   Y-axis acceleration output MSB
#define MAX400X_REG_YOUT_LOWER      0x06 // R   Y-axis acceleration output LSB
#define MAX400X_REG_ZOUT_UPPER      0x07 // R   Z-axis acceleration output MSB
#define MAX400X_REG_ZOUT_LOWER      0x08 // R   Z-axis acceleration output LSB
#define MAX400X_REG_TOUT            0x09 // R   Temperature output
#define MAX400X_REG_INT_MASK0       0x0a // W   shake & orientation detection interrupt mask.
#define MAX400X_REG_INT_MASK1       0x0b // W   data ready interrupt mask.
#define MAX400X_REG_DETECTION       0x0c // R/W orientation and shake detection parameters.
#define MAX400X_REG_CONTROL         0x0d // R/W operating mode control and full-scale range.
#define MAX400X_REG_DEVICE_ID       0x0e // R   currently has a value of 0x02.
#define MAX400X_REG_WHO_AM_I        0x0f // R   version code.

#define MXC400X_CMD_RANGE_8G        0x40
#define MAC400X_CMD_RANGE_4G        0x20
#define MAC400X_CMD_RANGE_2G        0x00
#define MAC400X_CMD_POWER_DOWN      0x01

#define MAC400X_DEVICE_ID_VAL       0x02

#define MXC400X_2G_SENSITIVITY      1024.0
#define MXC400X_4G_SENSITIVITY      512.0
#define MXC400X_8G_SENSITIVITY      256.0
#define MXC400X_T_ZERO              25.0
#define MXC400X_T_SENSITIVITY       0.586

typedef enum {
    MXC400X_CTRL_PD,
    MXC400X_CTRL_RANGE8G,
    MXC400X_CTRL_RANGE4G,
    MXC400X_CTRL_RANGE2G,
    MXC400X_CTRL_TEMP,
    MXC400X_CTRL_XOUT,
    MXC400X_CTRL_YOUT,
    MXC400X_CTRL_ZOUT,
    MXC400X_CTRL_
} mxc400x_ctrl_cmd_e;


#define ACC_SENSOR_NAME             "MXC400X"

void mxc400x_device_init(MXC400X_ADDRESS_CODE_E mxc400x_type);

#endif

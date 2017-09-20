/**
 * @brief       : this
 * @file        : rm3000.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-04-13
 * change logs  :
 * Date       Version     Author        Note
 * 2016-04-13  v0.0.1  gang.cheng    first version
 */
#ifndef __RM3000_H__
#define __RM3000_H__

#define MAGSENSOR_DEVICE            rm3000_driver

#define STANDARD_MODE               (0x9A)
#define LEGACY_MODE                 (0x9B)


#define LEGACY_PS_OFFSET            (4)
#define LEGACY_PS_32                (0)
#define LEGACY_PS_64                (1)
#define LEGACY_PS_128               (2)
#define LEGACY_PS_256               (3)
#define LEGACY_PS_512               (4)
#define LEGACY_PS_1024              (5)
#define LEGACY_PS_2048              (6)
#define LEGACY_PS_4096              (7)

#define LEGACY_AS_OFFSET            (0)
#define LEGACY_AS_NO                (0)
#define LEGACY_AS_X_AXIS            (1)
#define LEGACY_AS_Y_AXIS            (2)
#define LEGACY_AS_Z_AXIS            (3)

#define LEGACY_CC_TO_GAIN_32        (0.488)
#define LEGACY_CC_TO_GAIN_64        (0.976)
#define LEGACY_CC_TO_GAIN_128       (1.953)
#define LEGACY_CC_TO_GAIN_256       (3.905)
#define LEGACY_CC_TO_GAIN_512       (7.81)
#define LEGACY_CC_TO_GAIN_1024      (15.62)
#define LEGACY_CC_TO_GAIN_2048      (31.24)
#define LEGACY_CC_TO_GAIN_4096      (62.48)

#define MAG_SENSOR_NAME             "rm3000"

void rm3000_device_init(void);

#endif

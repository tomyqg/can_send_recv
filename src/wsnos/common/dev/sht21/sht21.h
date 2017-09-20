/***************************************************************************
* File        : sht21.h
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/


#ifndef __HT_SENSER_H__
#define __HT_SENSER_H__

typedef enum
{
    TYPE_READ_HUM,
    TYPE_READ_TMP,
} ht_senser_read_type_e;

#define TRIGGER_T_MEASUREMENT_HOLD_COMMAND          0xE3
#define TRIGGER_RH_MEASUREMENT_HOLD_COMMAND         0xE5
#define TRIGGER_T_MEASUREMENT_NO_HOLD_COMMAND       0xF3
#define TRIGGER_RH_MEASUREMENT_NO_HOLD_COMMAND      0xF5
#define WRITE_REG_COMMAND                           0xE6
#define READ_REG_COMMAND                            0xE7
#define SOFTWARE_RESET                              0xFE

#define HUMITURE_RESOLUTION_AT_12_BIT               40
#define HUMITURE_RESOLUTION_AT_8_BIT                700
#define T_RESOLUTION_AT_14_BIT                      10
#define T_RESOLUTION_AT_12_BIT                      40

#define HUMITURE_HIGH_RESOLUTION                    0
#define HUMITURE_LOW_RESOLUTION                     1

#ifndef HUMITURE_RESOLUTION
#define HUMITURE_RESOLUTION                         HUMITURE_HIGH_RESOLUTION
#endif

void sht21_device_init(void);

#define HT_SENSOR_NAME               "sht21_driver"

extern const struct ht_senser_driver sht21_driver;


#endif


/***************************************************************************
* File        : adxl345.h
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/

#ifndef __ADXL345_H
#define __ADXL345_H

typedef struct
{
    uint8_t single_tap : 1;
    uint8_t double_tap : 1;
    uint8_t activity   : 1;
    uint8_t inactivity : 1;
    uint8_t free_fall  : 1;
}acc_detect_t;

typedef void (*acc_event_callback_t)(const uint8_t type);

typedef struct
{
    uint8_t      data_rate;       //数据速率
    uint8_t      power_mode;      //功耗模式
    acc_detect_t enabled_detects; //使能的检测功能
    uint8_t      range;           //测量范围
    acc_event_callback_t acc_cb;  //通知事件的回调函数
}acc_config_t;

typedef enum
{
    ACC_SUCCESS,
    ACC_FAIL,
    
    ACC_NORMAL_OPERATION_MODE,
    ACC_LOW_POWER_OPERATION_MODE,
    
    ACC_AUTO_SLEEP_STATUS,//这个功能需要与链接位一起设置
    ACC_MEASURE_STATUS,//测量状态
    ACC_SLEEP_STATUS,
    ACC_STANDBY_STATUS,//待机状态
    
    ACC_INT_EVENT_TYPE,
    ACC_SINGLE_TAP_EVENT_TYPE,
    ACC_DOUBLE_TAP_EVENT_TYPE,
    ACC_ACTIVITY_EVENT_TYPE,
    ACC_INACTIVITY_EVENT_TYPE,
    ACC_FREE_FALL_EVENT_TYPE,
}e_acc_t;

//中断类型
typedef enum
{
    ACC_DATA_READY = 100,
    ACC_SINGLE_TAP,
    ACC_DOUBLE_TAP,
    ACC_ACTIVITY,
    ACC_INACTIVITY,
    ACC_FREE_FALL,
    ACC_WATERMARK,
    ACC_OVERRUN,
    ACC_FIFO_TRIG,
}e_acc_int_type_t;


typedef struct
{
    uint8_t type; // ACC_INT_EVENT_TYPE, ACC_SINGLE_TAP_EVENT_TYPE, ACC_DOUBLE_TAP_EVENT_TYPE,
                  // ACC_ACTIVITY_EVENT_TYPE, ACC_FREE_FALL_EVENT_TYPE,
}acc_event_t;

/* ADXL345 Register Map */
#define	ADXL345_DEVID			0x00 // R   Device ID.
#define ADXL345_THRESH_TAP		0x1D // R/W Tap threshold.
#define ADXL345_OFSX			0x1E // R/W X-axis offset.
#define ADXL345_OFSY			0x1F // R/W Y-axis offset.
#define ADXL345_OFSZ			0x20 // R/W Z-axis offset.
#define ADXL345_DUR                     0x21 // R/W Tap duration.
#define ADXL345_LATENT			0x22 // R/W Tap latency.
#define ADXL345_WINDOW			0x23 // R/W Tap window.
#define ADXL345_THRESH_ACT		0x24 // R/W Activity threshold.
#define ADXL345_THRESH_INACT	        0x25 // R/W Inactivity threshold.
#define ADXL345_TIME_INACT		0x26 // R/W Inactivity time.
#define ADXL345_ACT_INACT_CTL	        0x27 // R/W Axis enable control for activity.
									 //     and inactivity detection.
#define ADXL345_THRESH_FF		0x28 // R/W Free-fall threshold.
#define ADXL345_TIME_FF			0x29 // R/W Free-fall time.
#define ADXL345_TAP_AXES		0x2A // R/W Axis control for tap/double tap.
#define ADXL345_ACT_TAP_STATUS	        0x2B // R   Source of tap/double tap.
#define ADXL345_BW_RATE			0x2C // R/W Data rate and power mode control.
#define ADXL345_POWER_CTL		0x2D // R/W Power saving features control.
#define ADXL345_INT_ENABLE		0x2E // R/W Interrupt enable control.
#define ADXL345_INT_MAP			0x2F // R/W Interrupt mapping control.
#define ADXL345_INT_SOURCE		0x30 // R   Source of interrupts.
#define ADXL345_DATA_FORMAT		0x31 // R/W Data format control.
#define ADXL345_DATAX0			0x32 // R   X-Axis Data 0.
#define ADXL345_DATAX1			0x33 // R   X-Axis Data 1.
#define ADXL345_DATAY0			0x34 // R   Y-Axis Data 0.
#define ADXL345_DATAY1			0x35 // R   Y-Axis Data 1.
#define ADXL345_DATAZ0			0x36 // R   Z-Axis Data 0.
#define ADXL345_DATAZ1			0x37 // R   Z-Axis Data 1.
#define ADXL345_FIFO_CTL		0x38 // R/W FIFO control.
#define ADXL345_FIFO_STATUS		0x39 // R   FIFO status.

/* ADXL345_ACT_INACT_CTL Bits */
#define ADXL345_ACT_ACDC		(1 << 7)
#define ADXL345_ACT_X_EN		(1 << 6)
#define ADXL345_ACT_Y_EN		(1 << 5)
#define ADXL345_ACT_Z_EN		(1 << 4)
#define ADXL345_INACT_ACDC		(1 << 3)
#define ADXL345_INACT_X_EN		(1 << 2)
#define ADXL345_INACT_Y_EN		(1 << 1)
#define ADXL345_INACT_Z_EN		(1 << 0)

/* ADXL345_TAP_AXES Bits */
#define ADXL345_SUPPRESS		(1 << 3)
#define ADXL345_TAP_X_EN		(1 << 2)
#define ADXL345_TAP_Y_EN		(1 << 1)
#define ADXL345_TAP_Z_EN		(1 << 0)

/* ADXL345_ACT_TAP_STATUS Bits */
#define ADXL345_ACT_X_SRC		(1 << 6)
#define ADXL345_ACT_Y_SRC		(1 << 5)
#define ADXL345_ACT_Z_SRC		(1 << 4)
#define ADXL345_ASLEEP			(1 << 3)
#define ADXL345_TAP_X_SRC		(1 << 2)
#define ADXL345_TAP_Y_SRC		(1 << 1)
#define ADXL345_TAP_Z_SRC		(1 << 0)

/* ADXL345_BW_RATE Bits */
#define ADXL345_LOW_POWER		(1 << 4)
#define ADXL345_RATE(x)			((x) & 0xF)

/* ADXL345_POWER_CTL Bits */
#define ADXL345_PCTL_LINK               (1 << 5)
#define ADXL345_PCTL_AUTO_SLEEP         (1 << 4)
#define ADXL345_PCTL_MEASURE            (1 << 3)
#define ADXL345_PCTL_SLEEP              (1 << 2)
#define ADXL345_PCTL_WAKEUP(x)          ((x) & 0x3)

/* ADXL345_INT_ENABLE / ADXL345_INT_MAP / ADXL345_INT_SOURCE Bits */
#define ADXL345_DATA_READY              (1 << 7)
#define ADXL345_SINGLE_TAP              (1 << 6)
#define ADXL345_DOUBLE_TAP              (1 << 5)
#define ADXL345_ACTIVITY                (1 << 4)
#define ADXL345_INACTIVITY              (1 << 3)
#define ADXL345_FREE_FALL               (1 << 2)
#define ADXL345_WATERMARK               (1 << 1)
#define ADXL345_OVERRUN                 (1 << 0)

/* ADXL345_DATA_FORMAT Bits */
#define ADXL345_SELF_TEST               (1 << 7)
#define ADXL345_SPI                     (1 << 6)
#define ADXL345_INT_INVERT              (1 << 5)
#define ADXL345_FULL_RES                (1 << 3)
#define ADXL345_JUSTIFY                 (1 << 2)
#define ADXL345_RANGE(x)                ((x) & 0x3)
#define ADXL345_RANGE_PM_2G             (0)
#define ADXL345_RANGE_PM_4G             (1)
#define ADXL345_RANGE_PM_8G             (2)
#define ADXL345_RANGE_PM_16G            (3)

/* ADXL345_FIFO_CTL Bits */
#define ADXL345_FIFO_MODE(x)            (((x) & 0x3) << 6)
#define ADXL345_FIFO_BYPASS             0
#define ADXL345_FIFO_FIFO               1
#define ADXL345_FIFO_STREAM             2
#define ADXL345_FIFO_TRIGGER            3
#define ADXL345_TRIGGER                 (1 << 5)
#define ADXL345_SAMPLES(x)              ((x) & 0x1F)

/* ADXL345_FIFO_STATUS Bits */
#define ADXL345_FIFO_TRIG               (1 << 7)
#define ADXL345_ENTRIES(x)              ((x) & 0x3F)

/* ADXL345 ID */
#define ADXL345_ID                      0xE5

//#define ACC_SENSOR_NAME                 "ADXL345"

void adxl345_device_init(void);

#endif

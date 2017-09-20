/**
 * @brief       : this
 * @file        : bq32000.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-11-18
 * change logs  :
 * Date       Version     Author        Note
 * 2016-11-18  v0.0.1  gang.cheng    first version
 */
#ifndef __BQ32000_H__
#define __BQ32000_H__

typedef enum
{
    RTC_YEAD,
    RTC_MON,
    RTC_MDAY,
    RTC_HOUR,
    RTC_MIN,
    RTC_SEC,
} rtc_cmd_t;

typedef enum
{
    RTC_RESULT_OK,
    RTC_RESULT_NOT_SUPPORTED,
    RTC_RESULT_INVALID_VALUE,
    RTC_RESULT_ERROR,
} rtc_result_t;

typedef struct
{
    uint8_t year;       //*< 年
    uint8_t mon;        //*< 月
    uint8_t mday;       //*< 日
    uint8_t hour;       //*< 时
    uint8_t min;        //*< 分
    uint8_t sec;        //*< 秒
} rtc_t;

#define RTC_REG_SEC             0x00
#define RTC_REG_MIN             0x01
#define RTC_REG_HOURS           0x02
#define RTC_REG_DAY             0x03
#define RTC_REG_DATE            0x04
#define RTC_REG_MONTH           0x05
#define RTC_REG_YEARS           0x06
#define RTC_REG_CAL_CFG1        0x07
#define RTC_REG_TCH1            0x08
#define RTC_REG_TCH2            0x09
#define RTC_REG_CFG2            0x0a


#define RTC_ADDR_W              0xD0
#define RTC_ADDR_R              0xD1

#define ACK_ERROR               0x55
#define OUT_OF_RANG             0xaa
#define OUT_OF_ADDR             0xbb

#define RTC_NAME                "BQ32000"

void bq32000_device_init(void);

#endif
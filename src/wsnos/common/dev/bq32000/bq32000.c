/**
 * @brief       : this
 * @file        : bq32000.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-11-18
 * change logs  :
 * Date       Version     Author        Note
 * 2016-11-18  v0.0.1  gang.cheng    first version
 */
#include "gz_sdk.h"
#include "bq32000.h"

osel_device_t bq32000_device;

void bq32000_set_rtc_reg(uint8_t reg, int8_t value)
{
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);

    bq32000_i2c_start();

    bq32000_i2c_send_byte(RTC_ADDR_W);
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);

    bq32000_i2c_send_byte(reg);
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);

    bq32000_i2c_send_byte(value);
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);

    bq32000_i2c_stop();

    OSEL_EXIT_CRITICAL(s);
}

uint8_t bq32000_get_rtc_reg(uint8_t reg)
{
    uint8_t value = 0;
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);

    bq32000_i2c_start();

    bq32000_i2c_send_byte(RTC_ADDR_W);
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);

    bq32000_i2c_send_byte(reg);
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);

    bq32000_i2c_start();
    bq32000_i2c_send_byte(RTC_ADDR_R);
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);

    value = bq32000_i2c_recv_byte();
    WHILE(!bq32000_i2c_check_ack(), 80, FALSE);
    bq32000_i2c_stop();

    OSEL_EXIT_CRITICAL(s);

    return value;
}

static bool_t bcd_to_asc(rtc_t *rtc)
{
    rtc->sec  = ((rtc->sec  / 10) << 4) + (rtc->sec  % 10);
    rtc->min  = ((rtc->min  / 10) << 4) + (rtc->min  % 10);
    rtc->hour = ((rtc->hour / 10) << 4) + (rtc->hour % 10);
    rtc->mday = ((rtc->mday / 10) << 4) + (rtc->mday % 10);
    rtc->mon  = ((rtc->mon  / 10) << 4) + (rtc->mon  % 10);
    rtc->year = ((rtc->year / 10) << 4) + (rtc->year % 10);
    return TRUE;
}

static void asc_to_bcd(rtc_t *rtc)
{
    rtc->sec  = (rtc->sec  & 0x0f) + (((rtc->sec  & 0x70) >> 4) * 10);
    rtc->min  = (rtc->min  & 0x0f) + (((rtc->min  & 0x70) >> 4) * 10);
    rtc->hour = (rtc->hour & 0x0f) + (((rtc->hour & 0x30) >> 4) * 10);
    rtc->mday = (rtc->mday & 0x0f) + (((rtc->mday & 0x30) >> 4) * 10);
    rtc->mon  = (rtc->mon  & 0x0f) + (((rtc->mon  & 0x10) >> 4) * 10);
    rtc->year = (rtc->year & 0x0f) + (((rtc->year & 0xf0) >> 4) * 10);
}

osel_uint32_t bq32000_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    rtc_t rtc = {0};
    
    rtc.sec  = bq32000_get_rtc_reg(RTC_REG_SEC);
    rtc.min  = bq32000_get_rtc_reg(RTC_REG_MIN);
    rtc.hour = bq32000_get_rtc_reg(RTC_REG_HOURS);
    rtc.mday = bq32000_get_rtc_reg(RTC_REG_DAY);
    rtc.mon  = bq32000_get_rtc_reg(RTC_REG_MONTH);
    rtc.year = bq32000_get_rtc_reg(RTC_REG_YEARS);
    
    bcd_to_asc(&rtc);
    
    osel_memcpy(buffer, (uint8_t *)&rtc, sizeof(rtc_t));
    return TRUE;
}

osel_uint32_t bq32000_driver_write(osel_device_t *dev, osel_uint32_t pos, const void *buffer, osel_uint32_t size)
{
    DBG_ASSERT(NULL != buffer __DBG_LINE);

    rtc_t rtc = {0};
    osel_memcpy((uint8_t *)&rtc, buffer, sizeof(rtc_t));
    
    if ( (rtc.sec > 59) || (rtc.min > 59) || (rtc.hour > 23)
            || (rtc.mday > 31) || (rtc.mon > 12) || (rtc.year > 99))
    {
        return FALSE;
    }

    asc_to_bcd(&rtc);
    bq32000_set_rtc_reg(RTC_REG_SEC, rtc.sec);
    bq32000_set_rtc_reg(RTC_REG_MIN, rtc.min);
    bq32000_set_rtc_reg(RTC_REG_HOURS, rtc.hour);
    bq32000_set_rtc_reg(RTC_REG_DAY, rtc.mday);
    bq32000_set_rtc_reg(RTC_REG_MONTH, rtc.mon);
    bq32000_set_rtc_reg(RTC_REG_YEARS, rtc.year);

    return TRUE;
}

static osel_int8_t bq32000_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start bq32000
    return 0;
}


static osel_int8_t bq32000_driver_close(osel_device_t *dev)
{
    //*< make bq32000 in sleep mode
    return 0;
}

static osel_int8_t bq32000_driver_init(osel_device_t *dev)
{
    bq32000_i2c_init();
    return 0;
}

void bq32000_device_init(void)
{
    bq32000_device.init = bq32000_driver_init;
    bq32000_device.read = bq32000_driver_read;
    bq32000_device.write = bq32000_driver_write;
    bq32000_device.open = bq32000_driver_open;
    bq32000_device.close = bq32000_driver_close;
    
    osel_device_register(&bq32000_device, RTC_NAME, OSEL_DEVICE_FLAG_RDONLY);
}


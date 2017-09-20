#include "driver.h"
#include "sys_arch/osel_arch.h"

#include "time.h"

#include "../rtc_defs.h"


bool_t sim_rtc_init(void)
{
    return TRUE;
}

bool_t sim_rtc_get_time(rtc_t *p_rtc)
{
    time_t real_time = time(NULL);
    struct tm* local_time = localtime(&real_time);
    p_rtc->year = local_time->tm_year + 1900 - 2000;
    p_rtc->mon  = local_time->tm_mon + 1;
    p_rtc->mday = local_time->tm_mday;
    p_rtc->hour = local_time->tm_hour;
    p_rtc->min  = local_time->tm_min;
    p_rtc->sec  = local_time->tm_sec;

    return TRUE;
}

bool_t sim_rtc_set_time(rtc_t *p_rtc)
{
    printf("SET--year:%d,mon:%d,day:%d,hour:%d,min:%d,sec:%d\r\n",
           p_rtc->year + 2000, p_rtc->mon, p_rtc->mday, p_rtc->hour,
           p_rtc->min, p_rtc->sec);
    return TRUE;
}


static bool_t sim_rtc_int_cfg(rtc_alarm_int_t alarm_int_cb)
{
    return FALSE;
}



const struct rtc_driver sim_rtc_driver =
{
    sim_rtc_init,
    sim_rtc_set_time,
    sim_rtc_get_time,
    sim_rtc_int_cfg,
};

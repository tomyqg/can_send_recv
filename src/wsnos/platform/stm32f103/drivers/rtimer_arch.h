/**
 * @brief       : this
 * @file        : rtimer_arch.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-12-07
 * change logs  :
 * Date       Version     Author        Note
 * 2015-12-07  v0.0.1  gang.cheng    first version
 */
#ifndef __RTIMER_ARCH_H__
#define __RTIMER_ARCH_H__

#define TIMER_DBG_EN            (0u)
#define OVERFLOW_INT_EN			(0u)

#define MS_TO_TICK(time_ms)     ((uint32_t)((time_ms) * 32.768 + 0.5))
#define US_TO_TICK(time_us)     ((uint32_t)(((time_us) / 1000.0) * 32.768 + 0.5))
#define TICK_TO_US(tick_num)    ((uint32_t)((tick_num) * 30.518 + 0.5))


#if TIMER_DBG_EN > 0
#define DBG_TIMER(id)            DBG_LOG(DBG_LEVEL_INFO, "%d\r\n", id)
#else
#define DBG_TIMER(id)
#endif

#define TIMER_START()               rtimer_start();

#define HTIMER_SW_INC(x)            st(x = RTC_ReadTimeCounter(&rtim) >> 16;)
#define HTIMER_HWCOUNT(x)           (x = RTC_ReadTimeCounter(&rtim))

#define HTIMER_SET_COMPREG(x)       do{                         \
    RTC_WriteAlarmCounter(&rtim, (x)->w);                       \
    HTIMER_COMPINT_ENABLE();                                    \
}while(__LINE__ == -1)

#define HTIMER_GET_COMPREG()        RTC_ReadAlarmCounter(&rtim)
#define HTIMER_COMPINT_ENABLE()     __HAL_RTC_ALARM_EXTI_ENABLE_IT();__HAL_RTC_ALARM_ENABLE_IT(&rtim, RTC_IT_ALRA)
#define HTIMER_COMPINT_DISABLE()    __HAL_RTC_ALARM_EXTI_DISABLE_IT();__HAL_RTC_ALARM_DISABLE_IT(&rtim, RTC_IT_ALRA)
#define HTIMER_CMP_FLG()            __HAL_RTC_ALARM_EXTI_GET_FLAG()     //__HAL_RTC_ALARM_GET_FLAG(&rtim, RTC_IT_ALRA)
#define HTIMER_CLEAR_CMP_FLG()      __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();__HAL_RTC_ALARM_CLEAR_FLAG(&rtim, RTC_IT_ALRA)

#if OVERFLOW_INT_EN == 0
#define HTIMER_OF_FLG()             __HAL_RTC_ALARM_EXTI_GET_FLAG()     //__HAL_RTC_ALARM_GET_FLAG(&rtim, RTC_IT_ALRA)
#define HTIMER_CLEAR_OF_FLG()       __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();__HAL_RTC_ALARM_CLEAR_FLAG(&rtim, RTC_IT_ALRA)
#else
#define HTIMER_OF_FLG()             __HAL_RTC_OVERFLOW_GET_FLAG(&rtim, RTC_FLAG_OW)
#define HTIMER_CLEAR_OF_FLG()       __HAL_RTC_OVERFLOW_CLEAR_FLAG(&rtim, RTC_FLAG_OW)
#endif

#define TIMER_SET_TH                0x07
#define TIMER_SET_TH2               0x03    // 判断硬件时间是否即将翻转的阈值
#define TIMER_SET_TH3               0x07    // 允许返回定时(时间已过期)的阈值
#define TIMER_SET_TH4               0x00    // 硬件定时的设置点与当前点的最小间隔 ： 5 +det 2

extern RTC_HandleTypeDef  rtim;

void rtimer_start(void);

uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef* hrtc);

HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef* hrtc, uint32_t TimeCounter);

uint32_t RTC_ReadAlarmCounter(RTC_HandleTypeDef* hrtc);

HAL_StatusTypeDef RTC_WriteAlarmCounter(RTC_HandleTypeDef* hrtc, uint32_t AlarmCounter);

#endif

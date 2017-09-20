/**
 * @brief       : this
 * @file        : LoRaMac_arch.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017/01/18
 * change logs  :
 * Date       Version     Author        Note
 * 2017/01/18  v0.0.1  gang.cheng    实现LoRaWAN的初始化配置
 */
#include "LoRaMac_arch.h"
#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"

PROCESS(loramac_timercallback_process, "Lora Mac Timer Callback Thread");
PROCESS_THREAD(loramac_timercallback_process, ev, data)
{
    PROCESS_BEGIN();
    
    while(1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_MSG){
            TimerEvent_t *obj = (TimerEvent_t *)data;
            obj->Callback();
        }
    }

    PROCESS_END();
}


static void TimerCallback(void *p)
{
    TimerEvent_t *obj = (TimerEvent_t *)p;
    obj->hal_timer = NULL;
//    obj->Callback();
    //*< 中断不做回调处理，通过消息发送，让出控制权，由线程来异步处理
    osel_event_t event;
    event.sig = PROCESS_EVENT_MSG;
    event.param = p;
    osel_post(NULL, &loramac_timercallback_process, &event);
}

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
    DBG_ASSERT(NULL != obj __DBG_LINE);
    DBG_ASSERT(NULL != callback __DBG_LINE);
    obj->hal_timer   = NULL;
    obj->ReloadValue = 0;
    obj->Callback    = callback;
}

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStart( TimerEvent_t *obj )
{
    DBG_ASSERT(NULL != obj __DBG_LINE);
    DBG_ASSERT(NULL != obj->Callback __DBG_LINE);
    DBG_ASSERT(0 != obj->ReloadValue __DBG_LINE);

    if (obj->ReloadValue == 0)
        return;

    if (NULL != obj->hal_timer)
        hal_timer_cancel( &(obj->hal_timer));

    osel_int_status_t s = 0;
    OSEL_ENTER_CRITICAL(s);
    HAL_TIMER_SET_REL(MS_TO_TICK(obj->ReloadValue), TimerCallback, obj, obj->hal_timer);
    //DBG_ASSERT(NULL != obj->hal_timer __DBG_LINE);


    OSEL_EXIT_CRITICAL(s);
}

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStop( TimerEvent_t *obj )
{
//    DBG_ASSERT(NULL != obj->hal_timer __DBG_LINE);
    if (NULL != obj->hal_timer)
        hal_timer_cancel( &(obj->hal_timer));
}

/*!
 * \brief Resets the timer object
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerReset( TimerEvent_t *obj )
{
    TimerStop(obj);
    TimerStart(obj);
}

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    DBG_ASSERT(NULL != obj __DBG_LINE);
    DBG_ASSERT(NULL != obj->Callback __DBG_LINE);

    obj->ReloadValue = value;
}

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
TimerTime_t TimerGetCurrentTime( void )
{
    hal_time_t now;
    now = hal_timer_now();

    return TICK_TO_US(now.w) / 1000;
}

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \param [IN] savedTime    fix moment in Time
 * \retval time             returns elapsed time
 */
TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime )
{
    TimerTime_t elapsedTime = 0;

    // Needed at boot, cannot compute with 0 or elapsed time will be equal to current time
    if ( savedTime == 0 )
    {
        return 0;
    }

    elapsedTime = TimerGetCurrentTime();

    if (elapsedTime < savedTime)
    {   // roll over of the counter
        return (0xFFFFFFFF - savedTime + elapsedTime);
    }
    else
    {
        return (elapsedTime - savedTime);
    }
}

/*!
 * \brief Return the Time Futured since a fix moment in Time
 *
 * \param [IN] eventInFuture    fix moment in the future
 * \retval time             returns difference between now and future event
 */
TimerTime_t TimerGetFutureTime( TimerTime_t eventInFuture )
{
    return ( TimerGetCurrentTime() + eventInFuture );
}

/*!
 * \brief Manages the entry into ARM cortex deep-sleep mode
 */
// void TimerLowPowerHandler( void );


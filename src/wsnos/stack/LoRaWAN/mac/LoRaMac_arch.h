#ifndef __LORAMAC_ARCH_H__
#define __LORAMAC_ARCH_H__

#include "common/hal/hal.h"
#include "common/lib/lib.h"
#include "platform/platform.h"

typedef struct TimerEvent_s
{
    hal_timer_t *hal_timer;     //*< 
    uint32_t ReloadValue;       //! Timer delay value
    void ( *Callback )( void ); //! Timer IRQ callback function
} TimerEvent_t;
/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#endif

PROCESS_NAME(loramac_timercallback_process);

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) );

/*!
 * Timer IRQ event handler
 */
// void TimerIrqHandler( void );

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStart( TimerEvent_t *obj );

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStop( TimerEvent_t *obj );

/*!
 * \brief Resets the timer object
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerReset( TimerEvent_t *obj );

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
void TimerSetValue( TimerEvent_t *obj, uint32_t value );

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
TimerTime_t TimerGetCurrentTime( void );

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \param [IN] savedTime    fix moment in Time
 * \retval time             returns elapsed time
 */
TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime );

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \param [IN] eventInFuture    fix moment in the future
 * \retval time             returns difference between now and future event
 */
TimerTime_t TimerGetFutureTime( TimerTime_t eventInFuture );

/*!
 * \brief Manages the entry into ARM cortex deep-sleep mode
 */
void TimerLowPowerHandler( void );

#include "stack/LoRaWAN/radio/radio.h"

#if PF_CORE_POSIX
#include "../radio/sim_radio/sim_radio.h"
#else
#include "stack/LoRaWAN/radio/sx1276/sx1276_hal_arch.h"
#include "stack/LoRaWAN/radio/sx1276/sx1276.h"
#endif

#endif
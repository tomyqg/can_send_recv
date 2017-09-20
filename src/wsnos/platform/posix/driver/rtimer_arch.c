/**
 * @brief       : this
 * @file        : rtimer_arch.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-01-20
 * change logs  :
 * Date       Version     Author        Note
 * 2016-01-20  v0.0.1  gang.cheng    first version
 */
#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "driver.h"

#include <stdlib.h>

static pthread_t rtimer_thread;
uint16_t hardware_cnt = 0;	//*< Simulator hardware register
uint16_t compare_cnt = 0;   //*< simulator compare register
bool_t compare_flag = FALSE;
static bool_t rtimer_is_trigger = FALSE;
static void *rtimer_thread_routine(void *arg)
{
	while (1)
	{
		struct timeval timeout = {0};
		timeout.tv_usec = TICK_TO_US(1);	// 1000US one tick
		select(0, 0, 0, 0, &timeout);
		if (hardware_cnt++ == compare_cnt)
		{
			compare_flag = TRUE;
			rtimer_is_trigger = TRUE;
			sem_post(&sem_id);
		}
	}
}

void rtimer_start(void)
{
	hardware_int_init(&rtimer_thread, rtimer_thread_routine, NULL);
}


void rtimer_int_handle(void)
{
	if (rtimer_is_trigger)
	{
		// printf("rtimer_int_handle()--hardware_cnt:%d, compare_cnt:%d\r\n",
		       // hardware_cnt, compare_cnt);
		rtimer_is_trigger = FALSE;
		extern void htimer_int_handler(void);
		htimer_int_handler();
	}
}

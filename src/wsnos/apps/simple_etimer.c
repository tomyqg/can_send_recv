/**
* @brief       : 事件定时器的演示:4-5
 *
 * @file        : demo4.c
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/12/21
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/12/21    v0.0.1      gang.cheng    first version
 */
#include "sys_arch/osel_arch.h"
#include "common/hal/hal.h"
#include "common/lib/lib.h"
#include <demo.h>

DBG_THIS_MODULE("demo4")

/**
 * @brief 有一个线程1，周期性1s翻转LED，当运行1分钟以后，关闭翻转LED的操作。
 *        当运行2分钟以后，再次启动翻转LED。
 */
#define TASK_A_LED_EVENT        (0x3000)
#define TASK_A_STOP_EVENT       (0x3001)
#define TASK_A_START_EVENT      (0x3002)

static osel_task_t *demo4_a_tcb = NULL;
static osel_event_t demo4_a_event_store[10];

static osel_etimer_t cycle_etimer;   //*< 该定时器用于周期性翻转LED
static osel_etimer_t stop_etimer;    //*< 该定时器用于超时停止
static osel_etimer_t start_etimer;   //*< 该定时器用于超时重启

PROCESS_NAME(demo4_a_thread1_process);

PROCESS(demo4_a_thread1_process, "demo4 a thread 1 process");
PROCESS_THREAD(demo4_a_thread1_process, ev, data)
{
    PROCESS_BEGIN();

    static bool_t stop_flag = FALSE;    //*< 这里为什么用的静态变量？

    while(1)
    {
        if(ev == TASK_A_LED_EVENT)
        {
            if(stop_flag == FALSE)
            {
                hal_led_toggle(HAL_LED_GREEN);
                DBG_LOG(DBG_LEVEL_INFO, "demo4 led event handle\r\n");
            }
        }
        else if(ev == TASK_A_STOP_EVENT)
        {
            DBG_LOG(DBG_LEVEL_INFO, "demo4 led stop handle\r\n");
            if(osel_etimer_disarm(&cycle_etimer) == FALSE)//*< 定时器已经被触发
            {
                stop_flag = TRUE;   //*< 这里考虑用一个变量作为标志位，是否合适，为什么？
            }
        }
        else if(ev == TASK_A_START_EVENT)
        {
            DBG_LOG(DBG_LEVEL_INFO, "demo4 led restart handle\r\n");
            osel_etimer_rearm(&cycle_etimer, 100);
        }

        PROCESS_YIELD();     //*< 释放线程控制权，进行任务切换
    }

    PROCESS_END();
}

void simple_etimer_init(void)
{
    demo4_a_tcb = osel_task_create(NULL, DEMO4_PRIO, demo4_a_event_store, 10);
    osel_pthread_create(demo4_a_tcb, &demo4_a_thread1_process, NULL);

    osel_etimer_ctor(&cycle_etimer, &demo4_a_thread1_process, TASK_A_LED_EVENT, NULL);
    osel_etimer_ctor(&stop_etimer, &demo4_a_thread1_process, TASK_A_STOP_EVENT, NULL);
    osel_etimer_ctor(&start_etimer, &demo4_a_thread1_process, TASK_A_START_EVENT, NULL);

    osel_etimer_arm(&cycle_etimer, 100, 100);   //*< 周期性触发
    osel_etimer_arm(&stop_etimer, 600, 0);     //*< 一次性触发
    osel_etimer_arm(&start_etimer, 1200, 0);    //*< 一次性触发
}


/**
* @brief       : 消息机制：消息发送的DEMO
 *
 * @file        : demo3.c
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
#include "demo.h"

DBG_THIS_MODULE("demo3");

#define TASK_A_LED_EVENT            (0x3000)

static osel_task_t *demo3_a_tcb = NULL;
static osel_event_t demo3_a_event_store[10];
static osel_task_t *demo3_b_tcb = NULL;
static osel_event_t demo3_b_event_store[10];

PROCESS_NAME(demo3_a_thread1_process);
PROCESS_NAME(demo3_a_thread2_process);
PROCESS_NAME(demo3_b_thread1_process);

PROCESS(demo3_a_thread1_process, "demo3 a thread 1 process");
PROCESS_THREAD(demo3_a_thread1_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == TASK_A_LED_EVENT)
        {
            //*< 打印事件处理
            DBG_LOG(DBG_LEVEL_INFO, "demo3 a thread 1 led handle\r\n");
        }
        PROCESS_YIELD();     //*< 释放线程控制权，进行任务切换
    }
    PROCESS_END();
}

PROCESS(demo3_a_thread2_process, "task a thread 2 process");
PROCESS_THREAD(demo3_a_thread2_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == TASK_A_LED_EVENT)
        {
            //*< 打印事件处理
            DBG_LOG(DBG_LEVEL_INFO, "demo3 a thread 2 led handle\r\n");
        }
        PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
    }
    PROCESS_END();
}

PROCESS(demo3_b_thread1_process, "demo3 b thread 1 process");
PROCESS_THREAD(demo3_b_thread1_process, ev, data)
{
    PROCESS_BEGIN();
    while (1)
    {
        if (ev == PROCESS_EVENT_INIT) //*< 系统自定义事件
        {
            osel_event_t event;
            event.sig   = TASK_A_LED_EVENT;
            event.param = NULL;
            //@note 把消息发送给任务A，可以自己修改发送给任务A的某个线程
            osel_post(demo3_a_tcb, NULL, &event);    
        }
        PROCESS_YIELD();       //*< 释放线程控制权，进行任务切换
    }
    PROCESS_END();
}

void multi_pthread_init(void)
{
    demo3_a_tcb = osel_task_create(NULL, DEMO3_1_PRIO, demo3_a_event_store, 10);
    osel_pthread_create(demo3_a_tcb, &demo3_a_thread1_process, NULL);
    osel_pthread_create(demo3_a_tcb, &demo3_a_thread2_process, NULL);
    demo3_b_tcb = osel_task_create(NULL, DEMO3_2_PRIO, demo3_b_event_store, 10);
    osel_pthread_create(demo3_b_tcb, &demo3_b_thread1_process, NULL);
}

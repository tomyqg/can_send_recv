/**
 * @brief       : 简单线程创建  2.2
 *
 * @file        : simple_pthread.c
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

DBG_THIS_MODULE("simple_pthread");

static osel_task_t *simple_pthread_tcb = NULL;
static osel_event_t simple_pthread_event_store[10];

PROCESS_NAME(simple_pthread_thread1_process);

PROCESS(simple_pthread_thread1_process, "simple_pthread thread1 process");

PROCESS_THREAD(simple_pthread_thread1_process, ev, data)
{
    PROCESS_BEGIN();

    while (1)
    {
        if (ev == PROCESS_EVENT_INIT)
        {
            //@noto 打印日志是延时打印的方式，通过idle子任务输出，详细设计请参考5.2.3
            //      守护线程机制
            DBG_LOG(DBG_LEVEL_INFO, "simple_pthread entry thread\r\n");
        }

        PROCESS_YIELD();
    }

    PROCESS_END();
}

static void simple_pthread_tcb_entery(void *param)
{
    //*< 这里可以添加任务每次进入的日志打印信息，用于调试
}


void simple_pthread_init(void)
{
    simple_pthread_tcb = osel_task_create(&simple_pthread_tcb_entery, //*< 任务的入口函数
                                          DEMO2_PRIO,        //*< 优先级，数字越大，优先级越高
                                          simple_pthread_event_store, //*< 任务的缓冲队列起始地址
                                          10);               //*< 任务的蝗虫队列大小

    if (simple_pthread_tcb == NULL)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }

    osel_pthread_create(simple_pthread_tcb, &simple_pthread_thread1_process, NULL);
}

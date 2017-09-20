/**
* @brief       : 阻塞式编程:4-6
 *
 * @file        : demo5.c
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

DBG_THIS_MODULE("demo5");

static osel_task_t *demo5_tcb = NULL;
static osel_event_t demo5_event_store[10];

PROCESS_NAME(demo5_thread1_process);

PROCESS(demo5_thread1_process, "demo5 thread1 process");

PROCESS_THREAD(demo5_thread1_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t cycle_etimer;
    
    while(1)
    {
        hal_led_toggle(HAL_LED_BLUE);
        DBG_LOG(DBG_LEVEL_INFO, "demo5 led thread handle\r\n");
        OSEL_ETIMER_DELAY(&cycle_etimer, 50);  //*< 10ms一个tick
    }
    
    PROCESS_END();
}



void simple_osel_delay_init(void)
{
    demo5_tcb = osel_task_create(NULL,              //*< 任务的入口函数
                                 DEMO5_PRIO,        //*< 优先级，数字越大，优先级越高
                                 demo5_event_store, //*< 任务的缓冲队列起始地址
                                 10);               //*< 任务的蝗虫队列大小
    
    if(demo5_tcb == NULL)
    {
        DBG_LOG(DBG_LEVEL_INFO, "demo5_init failed\r\n");
        DBG_ASSERT(FALSE __DBG_LINE);
    }
    
    osel_pthread_create(demo5_tcb, &demo5_thread1_process, NULL);
}

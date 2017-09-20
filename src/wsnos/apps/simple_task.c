/**
 * @brief       : 简单任务创建  2.1
 *
 * @file        : simple_task.c
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

DBG_THIS_MODULE("simple_task")

static osel_task_t *simple_task_tcb = NULL;
static osel_event_t simple_task_event_store[10];

static void simple_task_tcb_entery(void *param)
{
    //@noto 这里可以添加任务每次进入的日志打印信息，用于调试
    //      任务创建以后并不会进入该流程
    DBG_LOG(DBG_LEVEL_INFO, "entry task\r\n");
}


void simple_task_init(void)
{
    simple_task_tcb = osel_task_create(&simple_task_tcb_entery, //*< 任务的入口函数
                                 DEMO1_PRIO,        //*< 优先级，数字越大，优先级越高
                                 simple_task_event_store, //*< 任务的缓冲队列起始地址
                                 10);               //*< 任务的蝗虫队列大小
    
    if(simple_task_tcb == NULL)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
}

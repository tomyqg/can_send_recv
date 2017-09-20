/**
 * @brief       : 作为unity测试的启动文件
 * @file        : app_test.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-03-06
 * change logs  :
 * Date       Version     Author        Note
 * 2017-03-06  v0.0.1  gang.cheng    first version
 */
#include "gz_sdk.h"

#define APP_TEST_TASK_PRIO      (3u)
#define APP_TEST_EVENT_MAX      (10u) //*< 最多储存10个事件

static osel_event_t app_test_event_store[APP_TEST_EVENT_MAX];
osel_task_t *app_test_task = NULL;

static void run_all_test(void)
{
    /* 在这里添加测试组 */
}

static char *arg_str[] = {
    "main",
    "-v",
};

PROCESS(app_test, "app_test");
PROCESS_THREAD(app_test, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;

    int argc = sizeof(arg_str)/sizeof(char *);
    UnityMain(argc, arg_str, run_all_test);

    OSEL_ETIMER_DELAY(&delay_timer, 10);  
    hal_board_reset(); //*< 这里为了让进程推出，否则集成测试无法停止输出结果
    PROCESS_END();
}


void app_init(void)
{    
    app_test_task = osel_task_create(NULL,
                                     APP_TEST_TASK_PRIO,
                                     app_test_event_store,
                                     APP_TEST_EVENT_MAX);
    DBG_ASSERT(NULL != app_test_task __DBG_LINE);

    osel_pthread_create(app_test_task, &app_test, NULL);
}
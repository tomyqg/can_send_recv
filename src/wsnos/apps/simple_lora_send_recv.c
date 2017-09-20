/**
 * @brief       : 该文件实现了lora的简单发送
 * @file        : lora_send.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-02-20
 * change logs  :
 * Date       Version     Author        Note
 * 2017-02-20  v0.0.1  gang.cheng    first version
 */
#include "gz_sdk.h"
DBG_THIS_MODULE("simple_lora_send_recv");
/**
 * 定义了任务的优先级
 */
#define APP_TASK_PRIO               (3u)


#define APP_DEMO_MAX_DATA_SIZE      (15u)
static uint8_t data_buff[APP_DEMO_MAX_DATA_SIZE];

#define APP_EVENT_MAX               (6u)
static osel_task_t *app_task_tcb = NULL;
static osel_event_t app_event_store[APP_EVENT_MAX];

static mbuf_t app_mbuf;

PROCESS_NAME(app_demo_process);

static bool_t app_tx_ok = TRUE;

static void app_tx_confirm(bool_t res, uint32_t seq, void *buffer, uint8_t len)
{
    app_tx_ok = TRUE;
}

static void app_rx_indicate(const void *buffer, uint8_t len)
{
    printf("[app recv]:");
    printf_string((uint8_t *)buffer, len);
}

static void app_join_confirm(bool_t res)
{
    ;
}

PROCESS(app_demo_process, "app send data to lora process");
PROCESS_THREAD(app_demo_process, ev, data)
{
    PROCESS_BEGIN();

    static osel_etimer_t delay_timer;
    static int8_t send_res = 0;
    while (1)
    {
        //*< delay 5 seconds ,wait os start up.
        OSEL_ETIMER_DELAY(&delay_timer, 100);
        if (app_tx_ok == FALSE)
            continue;
        app_tx_ok = FALSE;
        OSEL_ETIMER_DELAY(&delay_timer, 500);
        app_mbuf.data_p = app_mbuf.head;
        for (uint8_t i = 0; i < APP_DEMO_MAX_DATA_SIZE; i++)
        {
            *app_mbuf.data_p++ = i;
        }
        app_mbuf.data_len = APP_DEMO_MAX_DATA_SIZE;
        send_res = stack_send(MSG_UNCONFIRMED, &app_mbuf);
        if (send_res == -1)
        {
            DBG_LOG(DBG_LEVEL_INFO, "数据超过发送最大长度\r\n");   //*< 数据超过发送最大长度
        }
        else if (send_res == -2)
        {
            DBG_LOG(DBG_LEVEL_INFO, "当前网络还未入网\r\n");   //*< 当前网络还未入网
        }
        else if (send_res == -3)
        {
            DBG_LOG(DBG_LEVEL_INFO, "当前正在发送数据，无法发送，请主动退避\r\n");   //*< 当前正在发送数据，无法发送，请主动退避
        }
        else
        {
            printf_time();
        }
    }

    PROCESS_END();
}

void simple_lora_init(void)
{
    stack_run();
    app_task_tcb = osel_task_create(NULL, APP_EVENT_MAX, app_event_store, APP_EVENT_MAX);
    DBG_ASSERT(NULL != app_task_tcb __DBG_LINE);

    app_mbuf.head = &data_buff[0];
    app_mbuf.end  = &data_buff[APP_DEMO_MAX_DATA_SIZE - 1];

    stack_callback_t app_cbs;
    app_cbs.tx_confirm   = &app_tx_confirm;
    app_cbs.rx_indicate  = &app_rx_indicate;
    app_cbs.join_confirm = &app_join_confirm;
    stack_dependent_cfg(&app_cbs);

    osel_pthread_create(app_task_tcb, &app_demo_process, NULL);
}

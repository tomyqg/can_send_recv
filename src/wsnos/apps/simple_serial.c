#include "common/hal/hal.h"
#include "common/lib/lib.h"
#include "apps/node_cfg.h"

DBG_THIS_MODULE("simple_serial");

PROCESS_NAME(app_event_process);
#define APP_UART                           (SERIAL_1)
#define EVENT_NUM                       (10u)
#define APP_TASK_PRIO                   (1u)
typedef enum
{
    APP_UART_EVENT =  ((APP_TASK_PRIO << 8) | 0x01), //串口事件
    APP_PRINTF_EVENT,
} app_event_e;

static osel_task_t *app_task = NULL;
static osel_event_t app_event_store[EVENT_NUM];

/**
 * [uart_recv_cb 接收回调，响应：serial组件收到一条完整的数据帧]
 */
static void uart_recv_cb(void)
{
    osel_event_t event;
    event.sig = APP_UART_EVENT;
    event.param = NULL;
    osel_post(NULL, &app_event_process, &event);
}

/**
 * [uart_read 将数据按照指定格式存放到缓存区中]
 */
static void uart_read(void)
{
    uint8_t uart_data_buf[UART_LEN_MAX];
    uint16_t len = 0;
    uint8_t uart_pre = 4;
    serial_read(APP_UART, uart_data_buf, uart_pre);
    osel_memcpy((uint8_t *)&len, uart_data_buf + 2, sizeof(uint16_t));      //2字节帧头，2字节帧长
    len = S2B_UINT16(len);
    serial_read(APP_UART, uart_data_buf + uart_pre, len);
    DBG_LOG(DBG_LEVEL_INFO, "【串口数据】长度:%d\r\n", uart_pre + len);
    printf_string( uart_data_buf, uart_pre + len );
}

/**
 * [serial_init serial组件注册帧识别信息]
 */
static void serial_init(void)
{
    serial_reg_t app_serial_reg;
    osel_memset(&app_serial_reg, 0x00, sizeof(serial_reg_t));

    app_serial_reg.sd.valid             = TRUE;         //是否启用帧头识别符
    app_serial_reg.sd.len               = 2;
    app_serial_reg.sd.pos               = 0;
    app_serial_reg.sd.data[0]           = 0xD5;
    app_serial_reg.sd.data[1]           = 0xC8;

    app_serial_reg.ld.valid             = TRUE;
    app_serial_reg.ld.len               = 2;
    app_serial_reg.ld.pos               = 2;
    app_serial_reg.ld.little_endian     = FALSE;    //TRUE:小端识别     FALSE:大端识别

    app_serial_reg.argu.len_max         = UART_LEN_MAX;
    app_serial_reg.argu.len_min         = 4;

    app_serial_reg.echo_en              = FALSE;    //回显
    app_serial_reg.func_ptr             = uart_recv_cb;     //接收到完整数据后的回调

    serial_fsm_init(APP_UART);
    serial_reg(APP_UART, app_serial_reg);
}

/**
 * breif app状态线程
 *
 */
PROCESS(app_event_process, "app_event_process");
PROCESS_THREAD(app_event_process, ev, data)
{
    PROCESS_BEGIN();

    while (1)
    {
        if (ev == APP_UART_EVENT)
        {
            uart_read();
        }
        else if (ev == APP_PRINTF_EVENT)
        {

        }
        PROCESS_YIELD();
    }

    PROCESS_END();
}

PROCESS(uart_send_process, "uart_send_process");
PROCESS_THREAD(uart_send_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t cycle_etimer;
    while (1)
    {
        OSEL_ETIMER_DELAY(&cycle_etimer, 100);  //*< 10ms一个tick
        uint8_t send_buf[] = "hello\r\n";
        serial_write(APP_UART, send_buf, 7);
    }

    PROCESS_END();
}

void simple_serial(void)
{
    serial_init();
    app_task = osel_task_create(NULL, //*< 任务的入口函数
                                APP_TASK_PRIO,        //*< 优先级，数字越大，优先级越高
                                app_event_store, //*< 任务的缓冲队列起始地址
                                EVENT_NUM);               //*< 任务的蝗虫队列大小

    osel_pthread_create(app_task, &app_event_process, NULL);
    osel_pthread_create(app_task, &uart_send_process, NULL);    //串口发送线程，定时发送固定长度
}
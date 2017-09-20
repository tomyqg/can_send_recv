#include "gz_sdk.h"
#include "app.h"
#include "app_modules/app_module_cfg.h"
#include "app_modules/app_module_frame.h"
#include "app_modules/app_module_alg.h"
#include "app_modules/app_module_alg_handler.h"
#include "app_modules/app_module_send.h"
#include "app_product_test.h"

#define SERIAL_DATA_MAX                 (128)
#define SERIAL_HEAD_LEN                 (26u)
#define SERIAL_PRE_LEN                  (4u)

#define DEVICE_CFG_FRAME_LEN            (35u)
#define DEVICE_CFG_FRAME_ACK_LEN        (17u)

#define HB_PERIOD_TIME                  (60ul)   //5min
#define ACC_DATA_RATE                   (10u)     //10HZ

static acc_alg_config_t app_config_alg_local;
static uint8_t app_cmd_recv_array[SERIAL_DATA_MAX] = {0x00};
static osel_event_t app_event_store[APP_EVENT_MAX];
static osel_task_t *app_task_handle = NULL;

PROCESS_NAME(app_uart_event_process);

uint8_t debug_put_char(uint8_t ch)
{
    uart_send_char(DEBUG_PRINTF_ID, ch);
	return ch;
}

/*************************串口配置******************************/

static void app_uart_device_info_cfg(uint8_t cmd_pos, uint8_t cnt)
{
    app_cfg_info_t device_info_read;
    uint8_t app_cmd_send_array_1[SERIAL_DATA_MAX] = {0x00};
    uint8_t read_cnt = 0;
    uint8_t crc = 0;

    read_cnt = cnt;

    osel_memcpy(&device_info_read.unique_id, &app_cmd_recv_array[read_cnt], sizeof(uint64_t));
    read_cnt =  read_cnt + sizeof(uint64_t);
    uint32_t temp_id = 0;
    if (!id_general((uint8_t *)&device_info_read.unique_id, 8, &temp_id))
        return;
    device_info_read.device_id = ((device_info_read.unique_id) << 1) >> 57;

    osel_memcpy(&device_info_read.rf_channel, &app_cmd_recv_array[read_cnt], sizeof(uint8_t));
    read_cnt++;
    osel_memcpy(&device_info_read.rf_power, &app_cmd_recv_array[read_cnt], sizeof(uint8_t));
    read_cnt++;
    osel_memcpy(&device_info_read.speed_or_lane, &app_cmd_recv_array[read_cnt], sizeof(uint8_t));
    read_cnt++;
    osel_memcpy(&crc, &app_cmd_recv_array[cmd_pos - 1], sizeof(uint8_t));
    read_cnt++;

    uint8_t crc_temp = crc_compute(&app_cmd_recv_array[SERIAL_PRE_LEN],
                                   (cmd_pos - SERIAL_PRE_LEN - 1));
    if (crc_temp == crc)
    {
        config_module_write(&device_info_read);
        config_module_save();

        osel_memcpy(app_cmd_send_array_1, app_cmd_recv_array, SERIAL_HEAD_LEN);
        app_cmd_send_array_1[5] = app_cmd_recv_array[5] | 0x80;
        uint16_t frame_len_temp_1 = DEVICE_CFG_FRAME_ACK_LEN;
        frame_len_temp_1 = S2B_UINT16(frame_len_temp_1);
        osel_memcpy(&app_cmd_send_array_1[2], &frame_len_temp_1, sizeof(uint16_t)); 
        osel_memcpy(&app_cmd_send_array_1[6], &frame_len_temp_1, sizeof(uint16_t));
        uint16_t cnt_temp_1 = 0;
        cnt_temp_1 = SERIAL_HEAD_LEN;
        app_cmd_send_array_1[cnt_temp_1] = COMMON_FRAME_CFG_DEVICE_TYPE;
        cnt_temp_1 += sizeof(uint8_t);
        app_cmd_send_array_1[cnt_temp_1] = 0x00;
        cnt_temp_1 += sizeof(uint8_t);

        uint8_t crc_temp_2 = crc_compute(&app_cmd_send_array_1[SERIAL_PRE_LEN], (cnt_temp_1 - SERIAL_PRE_LEN));
        app_cmd_send_array_1[cnt_temp_1] = crc_temp_2;
        cnt_temp_1 += sizeof(uint8_t);

        serial_write(SERIAL_1, app_cmd_send_array_1, cnt_temp_1);
        hal_board_reset();
    }
    else
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
}

static void app_uart_device_info_query(void)
{
    app_cfg_info_t device_info_read;
    uint8_t app_cmd_send_array[SERIAL_DATA_MAX] = {0x00};

    config_module_read(&device_info_read);
    osel_memcpy(app_cmd_send_array, app_cmd_recv_array, SERIAL_HEAD_LEN);
    uint16_t frame_len_temp = DEVICE_CFG_FRAME_LEN;
    frame_len_temp = S2B_UINT16(frame_len_temp);
    osel_memcpy(&app_cmd_send_array[2], &frame_len_temp, sizeof(uint16_t));
    osel_memcpy(&app_cmd_send_array[6], &frame_len_temp, sizeof(uint16_t));
    app_cmd_send_array[5] = app_cmd_recv_array[5] | 0x80;
    app_cmd_send_array[SERIAL_HEAD_LEN] = COMMON_FRAME_QUERY_DEVICE_TYPE;
    uint16_t cnt_temp = 0;
    cnt_temp = SERIAL_HEAD_LEN + 1;

    osel_memcpy(&app_cmd_send_array[cnt_temp], &device_info_read.unique_id, sizeof(uint64_t));
    cnt_temp += sizeof(uint64_t);

    osel_memcpy(&app_cmd_send_array[cnt_temp], &device_info_read.rf_channel, sizeof(uint8_t));
    cnt_temp += sizeof(uint8_t);
    osel_memcpy(&app_cmd_send_array[cnt_temp], &device_info_read.rf_power, sizeof(uint8_t));
    cnt_temp += sizeof(uint8_t);
    osel_memcpy(&app_cmd_send_array[cnt_temp], &device_info_read.speed_or_lane, sizeof(uint8_t));
    cnt_temp += sizeof(uint8_t);

    uint8_t crc_temp_1 = crc_compute(&app_cmd_send_array[SERIAL_PRE_LEN], (cnt_temp - SERIAL_PRE_LEN));
    app_cmd_send_array[cnt_temp] = crc_temp_1;
    cnt_temp += sizeof(uint8_t);

    serial_write(SERIAL_1, app_cmd_send_array, cnt_temp);
}

static void app_uart_handle(void)
{
    uint8_t read_cnt = 0;
    uint8_t frame_type = 0;
    uint16_t frame_len = 0;
    uint8_t frame_ctl_type = 0;

    uint8_t app_cmd_recv_pos = 0;
    app_cmd_recv_pos = serial_read(SERIAL_1, app_cmd_recv_array, SERIAL_DATA_MAX);
    frame_len = app_cmd_recv_array[3] | app_cmd_recv_array[2];
    if(frame_len == 0)
        return;
    
    frame_type = app_cmd_recv_array[5];
    read_cnt += SERIAL_HEAD_LEN;
    frame_ctl_type = app_cmd_recv_array[SERIAL_HEAD_LEN];
    read_cnt += sizeof(uint8_t);

    if (frame_type == COMMON_FRAME_APP_CFG_TYPE)
    {
        if (frame_ctl_type == COMMON_FRAME_CFG_DEVICE_TYPE)
        {
            app_uart_device_info_cfg(app_cmd_recv_pos, read_cnt);
        }
        else if(frame_ctl_type == COMMON_FRAME_CFG_DEVICE_MODE)
        {
            operate_mode_flag_save(DEVICE_MODE_TEST);
            hal_board_reset();
        }
        else
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }
    }
    else if (frame_type == COMMON_FRAME_APP_QUERY_TYPE)
    {
        if (frame_ctl_type == COMMON_FRAME_QUERY_DEVICE_TYPE)
        {
            app_uart_device_info_query();
        }
        else
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }
    }
}

static void app_uart_frame_cb(void)
{
    osel_event_t event;
    event.sig = A_UART_EVENT;
    event.param = NULL;
    osel_post(NULL, &app_uart_event_process, &event);
}

static void app_uart_frame_cfg(void)
{
    serial_reg_t app_serial_reg;
    osel_memset(&app_serial_reg, 0x00, sizeof(serial_reg_t));

    app_serial_reg.sd.valid             = TRUE;
    app_serial_reg.sd.len               = 2;
    app_serial_reg.sd.pos               = 0;
    app_serial_reg.sd.data[0]           = 0xD5;
    app_serial_reg.sd.data[1]           = 0xC8;

    app_serial_reg.ld.valid             = TRUE;
    app_serial_reg.ld.len               = 2;
    app_serial_reg.ld.pos               = 2;
    app_serial_reg.ld.little_endian     = FALSE;

    app_serial_reg.argu.len_max         = UART_LEN_MAX;
    app_serial_reg.argu.len_min         = 4;

    app_serial_reg.echo_en              = FALSE;
    app_serial_reg.func_ptr             = app_uart_frame_cb;

    serial_fsm_init(SERIAL_1);
    serial_reg(SERIAL_1, app_serial_reg);
}

/******************************************************************************/

static void start_work(void)
{
    adxl345_device_init();
    app_module_alg_handler_init(app_task_handle, &app_config_alg_local);
}

static void frm_recv_cb(uint8_t *buf, uint8_t len)
{
    start_work();
}

/**
*** 算法数据发送
**/
static void app_send_alg_result_frm(uint8_t alg_result, uint8_t *data, 
                                    uint16_t data_len)
{
    ;
}

static void app_alg_event_handler(alg_event_t *acc_alg_event)
{
    if (acc_alg_event == NULL)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
    uint8_t alg_result_tmp = TYPE_APP_FRM_ALARM;
    uint8_t feature_data_buf[22];
    
    switch (acc_alg_event->type)
    {
    case ALARM_EVENT_TYPE:
        //添加处理
        alg_result_tmp = TYPE_APP_FRM_ALARM;
        break;

    case FEATURE_EVENT_TYPE:
        //添加处理
        alg_result_tmp = TYPE_APP_FRM_FEATURE;
        break;

    default:
        break;
    }
    app_send_alg_result_frm(alg_result_tmp, feature_data_buf, 22);
}

PROCESS(app_send_hb_process, "app_send_hb_process");
PROCESS_THREAD(app_send_hb_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    start_work();
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    while (1)
    {
//        OSEL_ETIMER_DELAY(&delay_timer, (hb_cycle*1000ul) / OSEL_TICK_PER_MS);
//        app_send_hb_frm();
    }
    PROCESS_END();
}

PROCESS(app_uart_event_process, "app_uart_event_process");
PROCESS_THREAD(app_uart_event_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    app_uart_frame_cfg();
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == A_UART_EVENT);
        app_uart_handle();
    }
    PROCESS_END();
}

PROCESS(wdg_clear_process, "wdg_clear_process");
PROCESS_THREAD(wdg_clear_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t wdg_clear_timer;
    OSEL_ETIMER_DELAY(&wdg_clear_timer, 120 / OSEL_TICK_PER_MS);
    while (1)
    {
        hal_wdt_clear(16000);
        OSEL_ETIMER_DELAY(&wdg_clear_timer,(1000) / OSEL_TICK_PER_MS);
    }

    PROCESS_END();
}

static void app_init(void)
{
    app_cfg_info_t app_cfg_info_tmp;
    app_task_handle = osel_task_create(NULL, APP_TASK_PRIO, app_event_store, APP_EVENT_MAX);
    if (app_task_handle == NULL)
        DBG_ASSERT(FALSE __DBG_LINE);
    
    config_module_init();
    osel_pthread_create(app_task_handle, &wdg_clear_process, NULL);
    osel_pthread_create(app_task_handle, &app_uart_event_process, NULL);
    
    if(config_module_read(&app_cfg_info_tmp))
    {
        app_config_alg_local.data_rate = ACC_DATA_RATE;
        app_config_alg_local.acc_alg_cb = app_alg_event_handler;
        
        app_module_send_init(frm_recv_cb ,app_task_handle);
    }
}

void app_task_init(void)
{
    app_production_test_init();
//    if(operate_mode_flag_get() == DEVICE_MODE_TEST)
//    {
//        app_production_test_init();
//    }
//    else
//    {
//        app_init();
//    }
}
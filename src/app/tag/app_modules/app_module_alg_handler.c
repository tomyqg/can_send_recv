/***************************************************************************
* File        : app_module_alg_handler.c
* Summary     : 加速度数据模块
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/11/9
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/9         v0.1            wangjifang        first version
***************************************************************************/
#include "math.h"
#include "gz_sdk.h"
#include "app_module_alg.h"
#include "app_module_alg_handler.h"


#define FEATURE_CYCLE                   (1000)
#define NUM_FLAG                        (0x3f)
#define TIME_OVERTURN                   (0xffffffff)
#define FLASH_PAGE_ADDR_ALG_INFO        (INFO_C_ADDR)

static acc_alg_config_t config_alg_local;
static uint8_t acc_alg_opera_mode = OPERA_MODE_ALARM;
static osel_device_t *acc_sensor = NULL;
static acc_data_t acc_data_buf[32];

//static uint32_t get_time_stamp(void)
//{
//    return hal_timer_now().w;
//}

static uint8_t acc_get_detect_data(acc_data_t *acc_data_buf, uint8_t max_size)
{
    osel_device_open(acc_sensor, OSEL_DEVICE_OFLAG_RDONLY);
    
    uint8_t count;
    uint8_t fifo_status_reg = 0;
    uint8_t retvalue = adxl345_read_register(MMA845x_STATUS_00, 
                                             &fifo_status_reg);
    DBG_ASSERT(I2C_OK == retvalue __DBG_LINE);
    
    count = (fifo_status_reg & NUM_FLAG);
    
    if (count > 32)
    {
        count = 32;
    }

    for (uint8_t i = 0; i < count; i++)
    {
        osel_device_read(acc_sensor, 0, &acc_data_buf[i], 6);
    }
    osel_device_close(acc_sensor);
    
    return count;
}

void cycle_get_acc_data_handler(void)
{
    uint8_t count;
    acc_data_t acc_alg_data;
    count = acc_get_detect_data(acc_data_buf, 32); 
    for (uint8_t i = 0; i < count; i++)
    {
        acc_alg_data.x = acc_data_buf[i].x;
        acc_alg_data.y = acc_data_buf[i].y;
        acc_alg_data.z = acc_data_buf[i].z;
        
        if ((uint16_t)ABS(acc_alg_data.x) > 5000)
        {
            continue;
        }
        if ((uint16_t)ABS(acc_alg_data.y) > 5000)
        {
            continue;
        }
        if ((uint16_t)ABS(acc_alg_data.z) > 5000)
        {
            continue;
        }
//        motion_detect_algo_pulling(acc_alg_data);
    }
}

//static void acc_event_handler(void)
//{
//    if(acc_alg_opera_mode != OPERA_MODE_ALARM)
//        return;
//}

static void cycle_send_feture_handler(void)
{
    alg_event_t alg_event;
    alg_event.type = FEATURE_EVENT_TYPE;
//    get_feature(&alg_event.var_val);
    config_alg_local.acc_alg_cb(&alg_event);
}

PROCESS(app_feature_send_process, "app_feature_send_process");
PROCESS_THREAD(app_feature_send_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    
    while (1)
    {
        if(acc_alg_opera_mode == OPERA_MODE_DEBUG)
        {
            OSEL_ETIMER_DELAY(&delay_timer, (FEATURE_CYCLE) / OSEL_TICK_PER_MS);
            cycle_send_feture_handler();
        }

    }
    PROCESS_END();
}

PROCESS(app_acc_process, "app_acc_process");
PROCESS_THREAD(app_acc_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    acc_sensor = osel_device_find(ACC_SENSOR_NAME);
    if (acc_sensor == NULL)
    {
        PRINTF("device %s don't find\r\n", MAG_SENSOR_NAME);
        PROCESS_EXIT();
    }
    osel_device_init(acc_sensor);
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    
    while (1)
    {
        OSEL_ETIMER_DELAY(&delay_timer, 
                          (1000/config_alg_local.data_rate) / OSEL_TICK_PER_MS);
        cycle_get_acc_data_handler();

    }
    PROCESS_END();
}

void app_module_alg_handler_init(osel_task_t *task , acc_alg_config_t *config)
{
    device_info_t device_info = hal_board_info_look();
    
    config_alg_local.data_rate = config->data_rate;
    config_alg_local.acc_alg_cb = config->acc_alg_cb;

//    acc_alg_init();
//    acc_alg_config(&acc_event_handler);

    osel_pthread_create(task, &app_acc_process, NULL);
}

void app_module_acc_deinit(osel_task_t *task)
{
    osel_pthread_exit(task, &app_acc_process, NULL);
}
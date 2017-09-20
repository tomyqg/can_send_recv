#include <stdlib.h>
#include "gz_sdk.h"
#include "app_product_test.h"
#include "app_modules/app_module_alg.h"

#define MAC_HEAD_CTRL_SIZE                      1u
#define MAC_HEAD_SEQ_SIZE                       4u
#define MAC_ADDR_SIZE                           2u
#define DEBUG_MODE                              1
#define TX_MODE                                 1

static osel_event_t app_event_store[APP_TEST_EVENT_MAX];
static osel_task_t *app_task_handle = NULL;
static bool_t enter_lpm = TRUE;
static osel_device_t *acc_sensor = NULL;
static uint8_t data_buf_can_recv[11] = {0};

PROCESS_NAME(app_serial_process);
PROCESS_NAME(product_wdg_clear_process);
PROCESS_NAME(product_adxl345_read_process);
PROCESS_NAME(product_mma8451_read_process);
PROCESS_NAME(product_can_receive_process);
PROCESS_NAME(product_can_send_process);

DBG_THIS_MODULE("app")

#if DEBUG_MODE == 0
static void product_cmd_get_at_help(void);
#endif

static void test_can_data_recv_handler(uint8_t  *data_p, uint8_t data_len)
{
    data_buf_can_recv[0] = data_len;
    osel_memcpy(data_buf_can_recv + 1, data_p, data_len);
    osel_event_t event;
    event.sig = APP_CAN_EVENT;
    event.param = data_buf_can_recv;
    osel_post(NULL, &product_can_receive_process, &event);
}

static void app_cmd_adxl345_read(void)
{
    osel_device_open(acc_sensor, OSEL_DEVICE_OFLAG_RDONLY);
    acc_data_t data;
    uint8_t retvalue = 0;
    uint8_t fifo_status_reg = 0;
    uint8_t entries = 0;//采样点个数
  
    retvalue = adxl345_read_register(ADXL345_FIFO_STATUS,&fifo_status_reg);
    DBG_ASSERT(I2C_OK == retvalue __DBG_LINE);

    entries = fifo_status_reg&0x3f;
    if (entries > 32)
    {
        entries = 32;
    }
    for (uint8_t i = 0;i < entries;i++)
    {
        osel_device_read(acc_sensor, 0, &data, 6);
        DBG_LOG(DBG_LEVEL_ORIGIN, "%d %d %d\r\n", data.x , 
                data.y, data.z);
    }
    osel_device_close(acc_sensor);
}

static void app_cmd_mamx8451_read(void)
{
    osel_device_open(acc_sensor, OSEL_DEVICE_OFLAG_RDONLY);
    acc_data_t data;
    uint8_t retvalue = 0;
    uint8_t fifo_status_reg = 0;
    uint8_t entries = 0;//采样点个数
  
    retvalue = mma845x_read_register(0x00,&fifo_status_reg);
    DBG_ASSERT(I2C_OK == retvalue __DBG_LINE);
    
    entries = fifo_status_reg&0x3f;
    if (entries > 32)
    {
        entries = 32;
    }
    for (uint8_t i = 0;i < entries;i++)
    {
        osel_device_read(acc_sensor, 0, &data, 6);
        data.x = (int16_t)((int32_t)data.x *1000/2048);
        data.y = (int16_t)((int32_t)data.y *1000/2048);
        data.z = (int16_t)((int32_t)data.z *1000/2048);
    
        if(data.x > 2000 || data.x < -2000 || data.y > 2000|| data.y < -2000||
           data.z > 2000 || data.z < -2000)
            return;
        
        DBG_LOG(DBG_LEVEL_ORIGIN, "%d %d %d\r\n", data.x , 
                data.y, data.z);
    }
    osel_device_close(acc_sensor);
}

#if DEBUG_MODE == 0
static void app_cmd_exter_parse(uint8_t *buf, uint8_t len)
{
    if(osel_memcmp(buf, CMD_EXTER_ALOHA, sizeof(CMD_EXTER_ALOHA)-1))
    {
        DBG_LOG(DBG_LEVEL_ORIGIN, CMD_ALOHA);
    }
    else if(osel_memcmp(buf, CMD_EXTER_HELP, sizeof(CMD_EXTER_HELP)-1))
    {
        product_cmd_get_at_help();
    }
    else if(osel_memcmp(buf, CMD_EXTER_RST, sizeof(CMD_EXTER_RST)-1))
    {
        DBG_LOG(DBG_LEVEL_ORIGIN, CMD_OK);
        hal_board_reset();
    }
    else if(osel_memcmp(buf, CMD_EXTER_ADXL345S, sizeof(CMD_EXTER_ADXL345S)-1))
    {
        osel_pthread_create(app_task_handle, &product_adxl345_read_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_ADXL345E, sizeof(CMD_EXTER_ADXL345E)-1))
    {
        osel_pthread_exit(app_task_handle, &product_adxl345_read_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_MMA8451S, sizeof(CMD_EXTER_MMA8451S)-1))
    {
        osel_pthread_create(app_task_handle, &product_mma8451_read_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_MMA8451E, sizeof(CMD_EXTER_MMA8451E)-1))
    {
        osel_pthread_exit(app_task_handle, &product_mma8451_read_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_CANTXS, sizeof(CMD_EXTER_CANTXS)-1))
    {
        //添加
        osel_pthread_create(app_task_handle, &product_can_send_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_CANTXE, sizeof(CMD_EXTER_CANTXE)-1))
    {
        //添加
        osel_pthread_exit(app_task_handle, &product_can_send_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_CANRXS, sizeof(CMD_EXTER_CANRXS)-1))
    {
        //添加
        osel_pthread_create(app_task_handle, &product_can_receive_process, NULL);
    }
    else if(osel_memcmp(buf, CMD_EXTER_CANRXE, sizeof(CMD_EXTER_CANRXE)-1))
    {
        //添加
        osel_pthread_exit(app_task_handle, &product_can_receive_process, NULL);
    }
     else if(osel_memcmp(buf, CMD_EXTER_OPERATE_EXIT, sizeof(CMD_EXTER_OPERATE_EXIT)-1))
    {
        operate_mode_flag_clear();
        DBG_LOG(DBG_LEVEL_ORIGIN, CMD_OK);
        hal_board_reset();
    }
    else if(osel_memcmp(buf, CMD_EXTER_OPERATE_VER, sizeof(CMD_EXTER_OPERATE_VER)-1))
    {
        serial_write(SERIAL_2, "AT+VER", 6);
    }
    
    else
    {
        DBG_LOG(DBG_LEVEL_ORIGIN, CMD_ERROR01);
    }
}

static void app_cmd_parse(const uint8_t *buf, uint8_t len)
{
    switch(*buf)
    {
    case CMD_TEST_TYPE:
        DBG_LOG(DBG_LEVEL_ORIGIN, CMD_OK);
        break;
        
    case CMD_EXTER_TYPE:
        app_cmd_exter_parse((uint8_t *)(buf+1), len-1);
        break;
        
    default:
        DBG_LOG(DBG_LEVEL_ORIGIN, CMD_ERROR00);
        break;
    }
}

static void serial_1_event_handle(void)
{
	uint8_t frm_len = 0;
    uint8_t read_data = 0;
    uint8_t cmd_recv[UART_LEN_MAX];
    
    osel_memset(cmd_recv, 0x00, UART_LEN_MAX);
    
    while(serial_read(UART_1, &read_data, sizeof(uint8_t)))
    {
        cmd_recv[frm_len++] = read_data;
        if(read_data == CMD_CR)
        {
            break;
        }
    }
    
    if(frm_len > CMD_AT_HEAD_SIZE)
    {
        app_cmd_parse(&cmd_recv[CMD_AT_HEAD_SIZE], 
                      frm_len - CMD_AT_HEAD_SIZE);
    }
}

static void serial_2_event_handle(void)
{
	uint8_t frm_len = 0;
    uint8_t read_data = 0;
    uint8_t cmd_recv[UART_LEN_MAX];
    
    osel_memset(cmd_recv, 0x00, UART_LEN_MAX);
    
    while(serial_read(UART_2, &read_data, sizeof(uint8_t)))
    {
        cmd_recv[frm_len++] = read_data;
        if(read_data == CMD_LF)
        {
            break;
        }
    }
    
    if(frm_len > CMD_AT_HEAD_SIZE)
    {
        for(uint8_t i =  0; i < frm_len; i++)
            DBG_LOG(DBG_LEVEL_ORIGIN, "%c", cmd_recv[i]);
        
        DBG_LOG(DBG_LEVEL_ORIGIN, "\r\n");
    }
}


#elif DEBUG_MODE == 1
static void serial_event_handle(void)
{
    uint8_t frame_type = 0;
    uint16_t frame_len = 0;
    uint8_t app_cmd_recv_array[60] = {0x00};
    serial_read(DEBUG_PRINTF_ID, app_cmd_recv_array, 60);
    frame_len = app_cmd_recv_array[3] | app_cmd_recv_array[2];
    if(frame_len == 0 || frame_len > 10)
        return;
    
    frame_type = app_cmd_recv_array[4] & 0x0f;
    can_send_data(&app_cmd_recv_array[6], frame_len - 2, frame_type);
}
#endif

PROCESS(app_serial_process, "app_serial_process");
PROCESS_THREAD(app_serial_process, ev, data)
{
    PROCESS_BEGIN();
    
    while(1)
    {
        if(ev == APP_SERIAL_1_EVENT)
        {
//            serial_1_event_handle();
        } 
        else if(ev == APP_SERIAL_2_EVENT)
        {
//            serial_2_event_handle();
            serial_event_handle();
        } 
        PROCESS_YIELD();
    }
    
    PROCESS_END();
}

PROCESS(product_wdg_clear_process, "product_wdg_clear_process");
PROCESS_THREAD(product_wdg_clear_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t daemon_timer;
    OSEL_ETIMER_DELAY(&daemon_timer, 10);
    while (1)
    {

        hal_wdt_clear(16000);

        OSEL_ETIMER_DELAY(&daemon_timer,(3000) / OSEL_TICK_PER_MS);
    }

    PROCESS_END();
}

PROCESS(product_mma8451_read_process, "product_mma8451_read_process");
PROCESS_THREAD(product_mma8451_read_process, ev, data)
{
    PROCESS_BEGIN();
    acc_sensor = osel_device_find(ACC_SENSOR_NAME);
    if (acc_sensor == NULL)
    {
        PRINTF("device %s don't find\r\n", MAG_SENSOR_NAME);
        PROCESS_EXIT();
    }
    osel_device_init(acc_sensor);
    static osel_etimer_t acc_read_timer;
    while (1)
    {
        app_cmd_mamx8451_read();
        OSEL_ETIMER_DELAY(&acc_read_timer,(20) / OSEL_TICK_PER_MS);
    }

    PROCESS_END();
}

PROCESS(product_adxl345_read_process, "product_adxl345_read_process");
PROCESS_THREAD(product_adxl345_read_process, ev, data)
{
    PROCESS_BEGIN();
    acc_sensor = osel_device_find(ACC_SENSOR_NAME);
    if (acc_sensor == NULL)
    {
        PRINTF("device %s don't find\r\n", MAG_SENSOR_NAME);
        PROCESS_EXIT();
    }
    osel_device_init(acc_sensor);
    static osel_etimer_t acc_read_timer;
    while (1)
    {
        app_cmd_adxl345_read();
        OSEL_ETIMER_DELAY(&acc_read_timer,(800) / OSEL_TICK_PER_MS);
    }

    PROCESS_END();
}

PROCESS(product_can_send_process, "product_can_send_process");
PROCESS_THREAD(product_can_send_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    static uint8_t data_seq = 0;
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    while (1)
    {
        can_send_data(&data_seq, 1, 1);
        data_seq ++;
        hal_led_toggle(BLUE);
        DBG_LOG(DBG_LEVEL_ORIGIN, "CAN SEND PROCESS\r\n");
        OSEL_ETIMER_DELAY(&delay_timer,(10000) / OSEL_TICK_PER_MS);
    }

    PROCESS_END();
}

PROCESS(product_can_receive_process, "product_can_receive_process");
PROCESS_THREAD(product_can_receive_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    static uint8_t len = 0;
    static uint8_t data_buf[10] = {0};
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == APP_CAN_EVENT);
    
        if(ev == APP_CAN_EVENT)
        {
            len = *(uint8_t *)data;
            osel_memcpy(data_buf, (uint8_t *)data + 1, len);
            for(uint8_t i = 0; i < len; i++)
            {
//                DBG_LOG(DBG_LEVEL_ORIGIN, "0x%02x ", data_buf[i]);
//                DBG_LOG(DBG_LEVEL_ORIGIN, "%d ", data_buf[i]);
                hal_uart_send_char(HAL_UART_1, data_buf[i]);
            }
//            DBG_LOG(DBG_LEVEL_ORIGIN, "\r\nCAN RECV PROCESS\r\n");
        }
    }

    PROCESS_END();
}

static void sys_enter_lpm_handler(void *p)
{
    debug_info_printf();
    if(enter_lpm)
    {
//        LPM3;
    }
}

//static void uart1_event(void)
//{
//    osel_event_t event;
//    event.sig = APP_SERIAL_1_EVENT;
//    event.param = NULL;
//    osel_post(NULL, &app_serial_process, &event);
//}

static void uart2_event(void)
{
    osel_event_t event;
    event.sig = APP_SERIAL_2_EVENT;
    event.param = NULL;
    osel_post(NULL, &app_serial_process, &event);
}

#if DEBUG_MODE == 0
static void product_cmd_get_at_help(void)
{
    DBG_LOG(DBG_LEVEL_ORIGIN, "=================================================================\r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "    ********************************************************\r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "               ### HOW TO USE THIS AT ###          \r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "    ********************************************************\r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "           This version:%02d.%02d.%02d of AT commands usage:\r\n", 
                                                 SOFTWARE_VER_X, SOFTWARE_VER_Y, SOFTWARE_VER_Z);
    DBG_LOG(DBG_LEVEL_ORIGIN, "    01. AT+ALOHA   02. AT+RST     03. AT+HELP    04. AT+ACCS     \r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "    05. AT+ACCE    06. AT+CANTXS  07. AT+CANTXE  08. AT+CANRXS   \r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "    09. AT+CANRXE                                                \r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "=================================================================\r\n");
    DBG_LOG(DBG_LEVEL_ORIGIN, "Please enter the command...\r\n");
}

static void uart1_frame_cfg(void)
{
	serial_reg_t serial;
    
	serial.sd.valid = TRUE;
	serial.sd.len = 2;
	serial.sd.pos = 0;
	serial.sd.data[0] = 'A';
	serial.sd.data[1] = 'T';
    
	serial.ld.valid = FALSE;
	serial.ld.little_endian = TRUE;
    
	serial.argu.len_max = UART_LEN_MAX;
	serial.argu.len_min = 2;
    
	serial.ed.valid = TRUE;
    serial.ed.len = 1;
    serial.ed.data[0] = CMD_CR;
    
	serial.echo_en = FALSE;
	serial.func_ptr = uart1_event;
    
	serial_fsm_init(UART_1);
	serial_reg(UART_1, serial);
}

static void uart2_frame_cfg(void)
{
	serial_reg_t serial;
    
	serial.sd.valid = TRUE;
	serial.sd.len = 1;
	serial.sd.pos = 0;
	serial.sd.data[0] = '+';
    
	serial.ld.valid = FALSE;
	serial.ld.little_endian = TRUE;
    
	serial.argu.len_max = UART_LEN_MAX;
	serial.argu.len_min = 2;
    
	serial.ed.valid = TRUE;
    serial.ed.len = 1;
    serial.ed.data[0] = CMD_LF;
    
	serial.echo_en = FALSE;
	serial.func_ptr = uart2_event;
    
	serial_fsm_init(UART_2);
	serial_reg(UART_2, serial);
}

#elif DEBUG_MODE == 1
static void uart2_frame_cfg(void)
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
    app_serial_reg.func_ptr             = uart2_event;

    serial_fsm_init(DEBUG_PRINTF_ID);
    serial_reg(DEBUG_PRINTF_ID, app_serial_reg);
}
#endif
void app_production_test_init(void)
{
    debug_init(DBG_LEVEL_ORIGIN); //DBG_LEVEL_ORIGIN | DBG_LEVEL_ORIGIN

    app_task_handle = osel_task_create(NULL, APP_TEST_TASK_PRIO, app_event_store, APP_TEST_EVENT_MAX);
    osel_pthread_create(app_task_handle, &app_serial_process, NULL);
    osel_pthread_create(app_task_handle, &product_wdg_clear_process, NULL);
#if TX_MODE == 0
    osel_pthread_create(app_task_handle, &product_can_receive_process, NULL);
#elif TX_MODE == 1
    osel_pthread_create(app_task_handle, &product_can_send_process, NULL);
#endif
    
//    adxl345_device_init();
//    mamx845x_device_init();
//    uart1_frame_cfg();
    uart2_frame_cfg();
#if TX_MODE == 1
    can_init(0x11, HOST_DEVICE_ID);
#elif TX_MODE == 0
    can_init(HOST_DEVICE_ID, 0x11);
#endif
    can_int_cb_reg(test_can_data_recv_handler);
    osel_idle_hook(sys_enter_lpm_handler);
    DBG_LOG(DBG_LEVEL_ORIGIN, CMD_SYSSTART);
}
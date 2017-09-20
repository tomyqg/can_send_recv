#ifndef __APP_H__
#define __APP_H__

//帧类型
#define COMMON_FRAME_APP_CFG_TYPE       0x11 //应用配置指令
#define COMMON_FRAME_APP_QUERY_TYPE     0x12 //应用查询指令
#define COMMON_FRAME_APP_DATA_TYPE      0x13 //应用数据指令
#define COMMON_FRAME_RADIO_CFG_TYPE     0x14 //无线配置指令
//控制码
#define COMMON_FRAME_CFG_DEVICE_TYPE    0x02 //设备属性配置
#define COMMON_FRAME_CFG_DEVICE_MODE    0x08 //*< 设备配置工作模式
#define COMMON_FRAME_QUERY_DEVICE_TYPE  0x01 //设备属性查询

#define APP_TASK_PRIO                           3u
#define APP_EVENT_MAX                           10u

typedef enum
{
    A_UART_EVENT =  ((APP_TASK_PRIO<<8) | 0x01),
    A_ACK_TIMEOUT,
    APP_RESERVE_EVENT,
} app_task_sig_e;

void app_task_init(void);

PROCESS_NAME(app_send_hb_process);
#endif

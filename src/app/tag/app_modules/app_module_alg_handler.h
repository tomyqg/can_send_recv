/***************************************************************************
* File        : app_module_alg_handler.h
* Summary     :
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/11/9
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/9         v0.1            wangjifang        first version
***************************************************************************/
#ifndef __APP_MODULE_ACCELER_H
#define __APP_MODULE_ACCELER_H

typedef enum
{
    ALARM_EVENT_TYPE,
    FEATURE_EVENT_TYPE,
} alg_event_sig_enum_e;


typedef enum
{
    OPERA_MODE_ALARM,
    OPERA_MODE_DEBUG,
} acc_opera_mode_e;

#pragma pack(1)

typedef struct
{
    uint16_t                  data_rate;                //*< 数据速率
    alg_event_callback_t  acc_alg_cb;
} acc_alg_config_t;

#pragma pack()

void app_module_alg_handler_init(osel_task_t *task , acc_alg_config_t *config);

#endif
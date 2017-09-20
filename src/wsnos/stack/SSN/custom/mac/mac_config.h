#ifndef __MAC_CONFIG_H__
#define __MAC_CONFIG_H__

#include "../../common/pbuf.h"


typedef void (*mac_config_assoc_info_insert_t)(uint64_t *unique_id);

#define CONFIG_SEND_MAX             (3u)    //*< 配置模块，最大发送配置请求次数
#define CONFIG_SEND_TIMES           (4000u) //*< 3000ms
#define CONFIG_RF_CHANNEL           (45u)

void mac_config_init(osel_task_t *task);

void mac_config_parse(pbuf_t *pbuf); 

void mac_config_start(osel_task_t *task);

void mac_config_stop(osel_task_t *task);

void mac_config_dependent_cfg(mac_config_assoc_info_insert_t insert_cb);

void mac_config_update_handle(pbuf_t *pbuf);
#endif
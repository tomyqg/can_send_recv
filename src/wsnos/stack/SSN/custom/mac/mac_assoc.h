#ifndef __MAC_ASSOC_H__
#define __MAC_ASSOC_H__


#include "mac_packet.h"


/**
 * @brief 初始化关联模块
 * @param task 关联模块对应的任务
 */
void mac_assoc_init(osel_task_t *task);


void mac_assoc_request_parse(uint32_t assoc_addr, assoc_requ_pld_t pld);

void mac_assoc_respone_parse(uint32_t assoc_addr, int8_t rssi,
                             assoc_resp_pld_t pld);


void mac_assoc_start(osel_task_t *task);

void mac_assoc_stop(osel_task_t *task);

void mac_assoc_respone_can_send(bool_t res);

#endif

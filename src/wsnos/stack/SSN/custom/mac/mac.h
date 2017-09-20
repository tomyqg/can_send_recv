#ifndef __MAC_H__
#define __MAC_H__

#include "mac_interface.h"
#include "mac_handle.h"
#include "mac_assoc.h"
#include "mac_config.h"
#include "mac_ctrl.h"
#include "mac_neighbor.h"
#include "mac_assoc_table.h"
#include "mac_packet.h"
#include "mac_queue.h"

/**
 *@brief MAC数据发送接口，上层应用调用该接口发送数据
 */
bool_t mac_send(sbuf_t *sbuf);


/**
 * @brief       MAC与APP的交互回调注册
 * @para        mac_cfg mac跟app数据交互注册
 *  @return     true or false
 */
bool_t mac_dependent_cfg(mac_dependent_t *mac_cfg);


/**
 * @brief       MAC层初始化
 *  @return     true or false
 */
bool_t mac_init(mac_pib_t *mac_pib);

/**
 * @brief 启动关联流程
 * @return  [description]
 */
bool_t mac_run(void);

/**
 * @brief 关闭mac层
 * @return  [description]
 */
bool_t mac_stop(void);

#endif

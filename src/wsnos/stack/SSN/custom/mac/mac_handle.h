/**
 * @brief       : this
 * @file        : mac_handle.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */
#ifndef __MAC_HANDLE_H__
#define __MAC_HANDLE_H__

#include "mac_interface.h"

typedef mac_dependent_t asyn_mac_cb_t;


/**
 *  @brief      初始化相应参数，启动mac
 *  @return     void
 */
bool_t mac_handle_init(osel_task_t *task);


bool_t mac_handle_start(osel_task_t *task);

bool_t mac_handle_stop(osel_task_t *task);


/*
 * @brief check weather data is pending to addr.
 */
sbuf_t *mac_pend_get(uint32_t addr);

/**
 * @brief checkout to recv mode start recv down pending frame.
 * @return  [description]
 */
bool_t mac_pend_recv(void);

/**
 * @brief send pending frame to addr.
 * @param  addr the pending frame send to.
 * @return
 */
bool_t mac_pend_send(sbuf_t *sbuf);

#endif

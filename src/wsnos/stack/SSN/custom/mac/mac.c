/**
 * @brief       : this
 * @file        : mac.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */
#include "../../common/sbuf.h"
#include "../../module/mac_module/mac_module.h"
#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"

#include "mac.h"


#define MAC_EVENT_MAX       (10u)
osel_task_t *mac_task_tcb = NULL;
static osel_event_t mac_event_store[MAC_EVENT_MAX];

mac_dependent_t mac_cb;

DBG_THIS_MODULE("mac");

bool_t mac_dependent_cfg(mac_dependent_t *mac_cfg)
{
    if (mac_cfg->recv_cb == NULL || mac_cfg->send_cb == NULL)
    {
        DBG_LOG(DBG_LEVEL_WARNING, "mac dependent cfg false! \r\n");
        return FALSE;
    }
    mac_cb.recv_cb = mac_cfg->recv_cb;
    mac_cb.send_cb = mac_cfg->send_cb;
    mac_cb.mac_assoc_cb = mac_cfg->mac_assoc_cb;
    mac_cb.mac_assoc_vailed = mac_cfg->mac_assoc_vailed;
    return TRUE;
}

bool_t mac_send(sbuf_t *sbuf)
{
    //*< TODO: 完成数据发送到queue队列，通过queue队列发送到handle
    mac_queue_insert(sbuf);
    return FALSE;
}


bool_t mac_init(mac_pib_t *mac_pib)
{
    mac_pib_init(mac_pib);

    mac_task_tcb = osel_task_create(NULL, MAC_TASK_PRIO, mac_event_store, MAC_EVENT_MAX);
    m_tran_init(mac_task_tcb);

    mac_ctrl_init();
    mac_neighbor_init();
    mac_assoc_table_init();
    mac_queue_init();

    mac_handle_init(mac_task_tcb);
    mac_assoc_init(mac_task_tcb);
    mac_config_init(mac_task_tcb);

    return TRUE;
}


bool_t mac_run(void)
{
    // mac_assoc_start(mac_task_tcb);
    mac_config_start(mac_task_tcb); //*< config start-> assoc
    mac_handle_start(mac_task_tcb);
    mac_neighbor_start();
    mac_queue_start();
    return TRUE;
}


bool_t mac_stop(void)
{
    mac_assoc_stop(mac_task_tcb);
    mac_config_stop(mac_task_tcb);
    mac_handle_stop(mac_task_tcb);
    mac_neighbor_stop();
    mac_queue_stop();
    return TRUE;
}
















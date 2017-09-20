/**
 * @brief       : this
 * @file        : mac_ed_handle.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */
#include "sys_arch/osel_arch.h"
#include "common/hal/hal.h"
#include "common/dev/dev.h"
#include "stack/SSN/common/pbuf.h"
#include "stack/SSN/common/sbuf.h"
#include "stack/SSN/custom/phy/phy_packet.h"
#include "stack/SSN/module/mac_module/mac_module.h"

#include "mac_interface.h"
#include "mac_ctrl.h"
#include "mac_handle.h"
#include "mac_packet.h"
#include "mac_queue.h"
#include "mac.h"

DBG_THIS_MODULE("mac_rp_handles");

PROCESS_NAME(mac_handle_process);
PROCESS_NAME(mac_pending_process);

static bool_t mac_pend_flag = FALSE;

static void mac_data(pbuf_t *pbuf)
{
    DBG_ASSERT(pbuf != NULL __DBG_LINE);

    sbuf_t *sbuf = sbuf_alloc(__SLINE1);

    mac_prim_arg_t *mac_prim_arg = &(sbuf->primargs.prim_arg.mac_prim_arg);
    sbuf->primtype = M2N_DATA_INDICATION;
    sbuf->primargs.pbuf = pbuf;

    mac_prim_arg->src_addr = pbuf->attri.src_id;
    mac_prim_arg->dst_addr = mac_pib_src_saddr();

    mac_prim_arg->msdu = pbuf->data_p;
    mac_prim_arg->msdu_length = pbuf->data_len - MAC_OFFSET_SIZE;

    if (mac_cb.recv_cb != NULL)
    {
        mac_cb.recv_cb(sbuf);
    }
    else
    {
        DBG_LOG(DBG_LEVEL_WARNING, "mac recv cb is false! \r\n");
    }
}

static void mac_ack(pbuf_t *pbuf)
{
    if (mac_head_info.ctrl.frm_pending)
    {
        mac_pend_recv();
    }
    g_trans_info.father_rssi = pbuf->attri.rssi_dbm;

    pbuf_free(&pbuf __PLINE2);
}

static void mac_command(pbuf_t *pbuf)
{
    if (pbuf == NULL)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }

    uint8_t frm_type = 0;
    frm_type = *(uint8_t *)(pbuf->data_p);
    pbuf->data_p++;
    switch (frm_type)   //*< 偏移过类型
    {
    case MAC_CMD_ASSOC_REQ:
    {
        assoc_requ_pld_t requ_pld;
        osel_memcpy(&requ_pld, pbuf->data_p, sizeof(assoc_requ_pld_t));
        mac_assoc_request_parse(pbuf->attri.src_id, requ_pld);
        break;
    }

    case MAC_CMD_ASSOC_RESP:
    {
        assoc_resp_pld_t resp_pld;
        osel_memcpy(&resp_pld, pbuf->data_p, sizeof(assoc_resp_pld_t));
        mac_assoc_respone_parse(pbuf->attri.src_id, pbuf->attri.rssi_dbm,
                                resp_pld);
        break;
    }
    case MAC_CMD_CONTROL:
        mac_config_parse(pbuf);
        break;

    default:
        break;
    }

    pbuf_free(&pbuf __PLINE2);
}

static void data_tx_done(sbuf_t *sbuf, bool_t res)
{
    if ((sbuf->orig_layer == MAC_LAYER) || (sbuf->orig_layer == NWK_LAYER))
    {
        m_tran_recv();
        pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
        sbuf_free(&sbuf __SLINE2);
        return;
    }

    if (res == FALSE)
    {
        //*< TODO: judge resent.
        // sbuf->primargs.pbuf->attri.already_send_times.mac_send_times++;
        g_trans_info.mac_send_num++;
        g_trans_info.mac_resent_num += m_tran_sent_times();
        g_trans_info.mac_cca_num += m_tran_cca_times();
        /**
        * 如果当前传输模块正在传输，导致本次发送失败，不能简单的停止传输模块，
        * 有可能导致底层radio的txok中断异常无法发生，导致传输模块死锁
        */
        if (m_tran_can_send())
            m_tran_recv();
    }
    else
    {
        g_trans_info.mac_send_num++;
        g_trans_info.mac_success_num++;
        g_trans_info.mac_resent_num += m_tran_sent_times();
        g_trans_info.mac_cca_num += m_tran_cca_times();
        if (m_tran_can_send())
            m_tran_recv();
    }

    if (mac_cb.send_cb != NULL)
        mac_cb.send_cb(sbuf, res);
}


static void mac_event_handle(void)
{
    if (!m_tran_can_send())
    {
        return;
    }

    if (mac_pend_flag) //*< now is pending frame.
        return;

    sbuf_t *sbuf = mac_queue_get(IMMEDI_LINK);
    if (sbuf == NULL)
        return;

    switch (sbuf->primtype)
    {
    case N2M_DATA_REQUEST:
    {
        mac_prim_arg_t *mac_prim_arg = &(sbuf->primargs.prim_arg.mac_prim_arg);
        mac_data_fill_package(sbuf, mac_prim_arg->dst_addr);
        m_tran_send(sbuf, data_tx_done, MAC_SEND_TIMES);
        break;
    }
    case N2M_JOIN_RESPONE:
    {
        mac_prim_arg_t *mac_prim_arg = &(sbuf->primargs.prim_arg.mac_prim_arg);
        mac_data_fill_package(sbuf, mac_prim_arg->dst_addr);
        m_tran_send(sbuf, data_tx_done, MAC_SEND_TIMES);
        break;
    }
    default:
        break;
    }

}


PROCESS(mac_handle_process, "mac_handle_process");
PROCESS_THREAD(mac_handle_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;

    while (1)
    {
        /**
         * 所有的数据发送都延时1个os_tick;
         */
        OSEL_ETIMER_DELAY(&delay_timer, 200 / OSEL_TICK_PER_MS);
        mac_event_handle();
    }

    PROCESS_END();
}

static void wait_data_tx_done(sbuf_t *sbuf, bool_t res)
{
    mac_pend_flag = FALSE;
    m_tran_recv();

    /**
     * @brief sbuf is still in wait_queue. now we should delete it from queue.
     */
    osel_int8_t s = 0;
    s = osel_mutex_lock(OSEL_MAX_PRIO);
    list_del(&(sbuf->list));
    osel_mutex_unlock(s);

    pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
    sbuf_free(&sbuf __SLINE2);
}


PROCESS(mac_pending_process, "mac_pending_process");
PROCESS_THREAD(mac_pending_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    static sbuf_t *wait_sbuf = NULL;
    static uint8_t index = 0;
    while (1)
    {
        if (ev == M_PEND_SEND_EVENT)
        {
            wait_sbuf = (sbuf_t *)data; //*< store data.
            /**
             * wait for ack send ok and forward nwk data.
             */
            for (index = 0; index < 3; index++)
            {
                if (m_tran_can_send())
                {
                    mac_pend_flag = TRUE;
                    mac_prim_arg_t *mac_prim_arg =
                        &(wait_sbuf->primargs.prim_arg.mac_prim_arg);
                    mac_data_fill_package(wait_sbuf, mac_prim_arg->dst_addr);
                    wait_sbuf->primargs.pbuf->attri.send_mode = TDMA_SEND_MODE; //*<
                    m_tran_send(wait_sbuf, wait_data_tx_done, MAC_SEND_TIMES);
                    break;
                }
                OSEL_ETIMER_DELAY(&delay_timer, 100 / OSEL_TICK_PER_MS);
            }

            //*< don't free wait_sbuf, wait for next downlink slot.
        }
        else if (ev == M_PEND_RECV_EVENT)
        {
            mac_pend_flag = TRUE;
            m_tran_recv();

            OSEL_ETIMER_DELAY(&delay_timer, 1500 / OSEL_TICK_PER_MS);

            mac_pend_flag = FALSE;
            m_tran_recv();
        }

        PROCESS_YIELD();
    }

    PROCESS_END();
}


bool_t mac_handle_init(osel_task_t *task)
{
    mac_ctrl_switch_init(mac_data, mac_ack, mac_command);

    osel_pthread_create(task, &mac_handle_process, NULL);
    osel_pthread_create(task, &mac_pending_process, NULL);
    return TRUE;
}

bool_t mac_handle_start(osel_task_t *task)
{
    return TRUE;
}

bool_t mac_handle_stop(osel_task_t *task)
{
    return TRUE;
}

sbuf_t *mac_pend_get(uint32_t addr)
{
    return mac_queue_find(WAIT_LINK, addr);
}

bool_t mac_pend_recv(void)
{
    osel_event_t event;
    event.sig = M_PEND_RECV_EVENT;
    event.param = NULL;
    osel_post(NULL, &mac_pending_process, &event);
    return TRUE;
}

/**
 * @brief send pending frame to addr.
 * @param  addr the pending frame send to.
 * @return
 */
bool_t mac_pend_send(sbuf_t *sbuf)
{
    osel_event_t event;
    event.sig = M_PEND_SEND_EVENT;
    event.param = sbuf;
    osel_post(NULL, &mac_pending_process, &event);
    return TRUE;
}

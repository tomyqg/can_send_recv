/**
 * @brief       : this
 * @file        : mac_assoc.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2015-10-21  v0.0.1  gang.cheng    first version
 */
#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "../../common/sbuf.h"
#include "../../module/mac_module/mac_module.h"
#include "../../custom/phy/phy_state.h"

#include "mac_interface.h"
#include "mac_neighbor.h"
#include "mac_ctrl.h"
#include "mac_assoc.h"
#include "mac_assoc_table.h"
#include "mac_handle.h"
#include "mac_queue.h"

DBG_THIS_MODULE("mac_assoc");

static hal_timer_t *assoc_resp_delay_timer = NULL;


#define ASSOC_PERIOD_TIME       (20000ul)   //*< 15s
#define ASSOC_SLEEP_TIME        (400u)      //*< 300ms

PROCESS_NAME(mac_assoc_process);
PROCESS_NAME(mac_assoc_delay_process);

static bool_t mac_assoc_respone_send_flag = FALSE;

static void assoc_tx_done(sbuf_t *sbuf, bool_t res)
{
    /**
    * 检测器发送完关联请求以后，需要进入接收态，接收关联应答
    */
    m_tran_recv();
    pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
    sbuf_free(&sbuf __SLINE2);
}


static bool_t mac_assoc_handle(void)
{
    /*
     * 选择邻居表里面最新、最好节点作为父节点
     */
    neighbor_node_t node = mac_neighbor_get_best(DEBUG_COORD_HOP);
    if (node.addr == NEIGHBOR_NONE_ADDR)
    {
        return FALSE; //*< no neigbhor send respone to me.
    }

    mac_pib_info.mac_coord_addr = node.addr;
    mac_pib_info.mac_coord_rssi = node.rssi;
    mac_pib_info.mac_hops       = node.hops + 1;
    if (mac_cb.mac_assoc_cb != NULL)
        mac_cb.mac_assoc_cb(TRUE);
    else
        DBG_LOG(DBG_LEVEL_TRACE, "mac assoc cb not register\r\n");

    return TRUE;
}

PROCESS(mac_assoc_process, "mac_assoc_process");
PROCESS_THREAD(mac_assoc_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t assoc_cycle_etimer;    //*< 用于间隔发送关联帧的周期定时器
    static osel_etimer_t assoc_sleep_etimer;    //*< 用于关联发送以后等待进入睡眠
    static sbuf_t *sbuf = NULL;

    /**** 线程启动的时候全局中断并没有使能，只有在osel_run之后才启动全局中断，
    而radio发送需要等待中断，所以等待100ms，切出线程，让osel_run得到执行，启动GIE
    ************/
    OSEL_ETIMER_DELAY(&assoc_cycle_etimer, 100 / OSEL_TICK_PER_MS);
    DBG_LOG(DBG_LEVEL_INFO, "ASSOC start\r\n");
    while (1)
    {
        osel_etimer_ctor(&assoc_sleep_etimer, PROCESS_CURRENT(),
                         PROCESS_EVENT_TIMER, NULL);
        osel_etimer_arm(&assoc_sleep_etimer, ASSOC_SLEEP_TIME / OSEL_TICK_PER_MS, 0);

        sbuf = mac_assoc_request_package(mac_pib_info.mac_src_addr);
        if (sbuf != NULL)
        {
            if (m_tran_can_send())
            {
                m_tran_send(sbuf, assoc_tx_done, 1);
            }
            else
            {
                DBG_ASSERT(FALSE __DBG_LINE);
            }
        }

        PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_EXIT) ||
                                 (ev == PROCESS_EVENT_TIMER));
        if (mac_assoc_handle())
        {
            osel_etimer_disarm(&assoc_cycle_etimer);
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_EXIT)
        {
            osel_etimer_disarm(&assoc_cycle_etimer);
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_TIMER)
        {
#if (NODE_TYPE == NODE_TYPE_ROUTER)

            m_tran_recv();  //*< 此处为中继在任意时候可以接收无线复位帧
#else
            m_tran_sleep();
#endif

            osel_etimer_ctor(&assoc_cycle_etimer, PROCESS_CURRENT(),
                             PROCESS_EVENT_TIMER, NULL);
            osel_etimer_arm(&assoc_cycle_etimer,
                            ASSOC_PERIOD_TIME / OSEL_TICK_PER_MS, 0);

            PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_EXIT) ||
                                     (ev == PROCESS_EVENT_TIMER));
            if (ev == PROCESS_EVENT_EXIT)
            {
                osel_etimer_disarm(&assoc_cycle_etimer);
                PROCESS_EXIT();
            }
        }
    }

    PROCESS_END();
}

PROCESS(mac_assoc_delay_process, "mac_assoc_delay_process");
PROCESS_THREAD(mac_assoc_delay_process, ev, data)
{
    PROCESS_BEGIN();

    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);
        if (m_tran_can_send())
            m_tran_send((sbuf_t *)data, assoc_tx_done, 1);
        else
            assoc_tx_done((sbuf_t *)data, FALSE);
    }

    PROCESS_END();
}


void mac_assoc_init(osel_task_t *task)
{
    osel_pthread_create(task, &mac_assoc_delay_process, PROCESS_CURRENT());
}


static void assoc_resp_delay_timer_cb(void *p)
{
    assoc_resp_delay_timer = NULL;
    osel_event_t event;
    event.sig = PROCESS_EVENT_MSG;
    event.param = p;
    osel_post(NULL, &mac_assoc_delay_process, &event);
}

void mac_assoc_respone_can_send(bool_t res)
{
    mac_assoc_respone_send_flag = res;
}

void mac_assoc_request_parse(uint32_t assoc_addr, assoc_requ_pld_t pld)
{
    assoc_result_e res_state = ASSOC_SUCCESS;
    assoc_node_t node;
    node.node_id = assoc_addr;

    uint8_t res = mac_assoc_table_check(&node);
    if (res == ASSOC_TABLE_NONE)
    {
        if (mac_assoc_table_insert(&node) == FALSE)
        {
            res_state = ASSOC_FULL;
        }
    }
    else if (res == ASSOC_TABLE_FAILED)
    {
        res_state = ASSOC_ID_FAILED;
    }

    if (res_state == ASSOC_SUCCESS)
    {
        if (pld.pan_id != PAN_ID)
        {
            res_state = ASSOC_PAN_FAILED;
        }
    }

    if(!mac_assoc_respone_send_flag)
        return;

    sbuf_t *sbuf = mac_assoc_respone_package(assoc_addr, res_state);
    sbuf->up_down_link = IMMEDI_LINK;
    if (mac_pib_hops() != 0)
    {
        if (assoc_resp_delay_timer != NULL)
            hal_timer_cancel(&assoc_resp_delay_timer);

        if (assoc_resp_delay_timer == NULL)
        {
            HAL_TIMER_SET_REL(MS_TO_TICK(mac_pib_hops() * 100),
                              assoc_resp_delay_timer_cb,
                              sbuf,
                              assoc_resp_delay_timer);
            DBG_ASSERT(NULL != assoc_resp_delay_timer __DBG_LINE);
        }
    }
    else
    {
        m_tran_send(sbuf, assoc_tx_done, 1);
    }

}


void mac_assoc_respone_parse(uint32_t assoc_addr, int8_t rssi,
                             assoc_resp_pld_t pld)
{
    neighbor_node_t node;
    if (pld.assoc_state == ASSOC_SUCCESS)
    {
        node.addr = assoc_addr;
        node.rssi = rssi;
        node.hops = pld.hops;
        if(node.hops >= MAX_HOPS)
            return;

        mac_neighbor_insert(node);
    }
}

void mac_assoc_start(osel_task_t *task)
{
#if (NODE_TYPE != NODE_TYPE_GATEWAY)

    device_info_t dev_info = hal_board_info_look();
    phy_set_channel(dev_info.rf_channel);
    osel_pthread_create(task, &mac_assoc_process, PROCESS_CURRENT());

#endif

}

void mac_assoc_stop(osel_task_t *task)
{
#if NODE_TYPE != NODE_TYPE_GATEWAY

    osel_pthread_exit(task, &mac_assoc_process, PROCESS_CURRENT());
#endif

}
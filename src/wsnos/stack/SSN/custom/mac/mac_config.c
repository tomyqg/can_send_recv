/**
 * @brief       : this
 * @file        : mac_config.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */
#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "common/hal/hal.h"

#include "../../custom/phy/phy_state.h"
#include "../../common/sbuf.h"
#include "../../module/mac_module/mac_module.h"

#include "mac_config.h"
#include "mac_packet.h"
#include "mac_ctrl.h"
#include "mac_assoc.h"
#include "common/dev/dev.h"
uint8_t mac_conf_first_startup = TRUE;
static osel_task_t *mac_conf_task_tcb = NULL;
static uint8_t mac_conf_times = 0;

static mac_config_assoc_info_insert_t config_assoc_info_insert_cb = NULL;

PROCESS_NAME(mac_config_start_process);
PROCESS_NAME(mac_config_handle_process);


void mac_config_dependent_cfg(mac_config_assoc_info_insert_t insert_cb)
{
    if (insert_cb != NULL)
    {
        config_assoc_info_insert_cb = insert_cb;
    }
}

static void config_request_tx_done(sbuf_t *sbuf, bool_t res)
{
    m_tran_recv();
    pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
    sbuf_free(&sbuf __SLINE2);
}


static void mac_config_send_request(void)
{
    sbuf_t *sbuf = NULL;
    mac_config_requ_pld_t pld;
    device_info_t d_info = hal_board_info_look();
    pld.dev_id  = d_info.unique_id;
    pld.channel = d_info.rf_channel;
    pld.power   = d_info.rf_power;
    pld.ver[0]  = SOFTWARE_VER_X;
    pld.ver[1]  = SOFTWARE_VER_Y;
    pld.ver[2]  = SOFTWARE_VER_Z;

    uint8_t crc8_c = crc_compute((uint8_t *)&pld, sizeof(pld) - 1);
    pld.crc8 = crc8_c;
    sbuf = mac_config_request_package(mac_pib_info.mac_src_addr,
                                      &pld);
    if (sbuf != NULL)
    {
        if (m_tran_can_send())
        {
            m_tran_send(sbuf, config_request_tx_done, 1);
        }
        else
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }
    }
    else
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
}


static void config_confirm_tx_done(sbuf_t *sbuf, bool_t res)
{
    osel_event_t event;
    event.sig = PROCESS_EVENT_MSG;
    event.param = NULL;
    osel_post(NULL, &mac_config_handle_process, &event);

    m_tran_recv();
    pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
    sbuf_free(&sbuf __SLINE2);
}

static void mac_config_send_confirm(uint8_t crc)
{
    sbuf_t *sbuf = NULL;

    mac_config_confirm_t confirm;
    confirm.crc8 = crc;

    sbuf = mac_config_confirm_package(mac_pib_info.mac_src_addr,
                                      &confirm);
    if (sbuf != NULL)
    {
        if (m_tran_can_send())
        {
            m_tran_send(sbuf, config_confirm_tx_done, 1);
        }
        else
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }
    }
    else
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }
}

static void mac_config_operate_handle(mac_config_ctrl_t ctrl, pbuf_t *pbuf)
{
    uint8_t crc8_c = 0X00;
    device_info_t d_info = hal_board_info_look();

    if (ctrl.content == MAC_CONF_CONTENT_INFO)
    {
        mac_config_operate_info_t operate_info;
        osel_memcpy((uint8_t *)&operate_info, pbuf->data_p, sizeof(operate_info));
        pbuf->data_p += sizeof(operate_info);

        if (!osel_memcmp((uint8_t *)&d_info.unique_id,
                         (uint8_t *)&operate_info.dev_id, 8))
        {
            return;
        }
        crc8_c = crc_compute((uint8_t *)&operate_info,
                             sizeof(operate_info) - 1);
        if (crc8_c != operate_info.crc8)
        {
            return;
        }

        osel_memcpy((uint8_t *)&d_info.unique_id, (uint8_t *)&operate_info.dev_id, 8);
        d_info.rf_channel = operate_info.channel;
        d_info.rf_power   = operate_info.power;
        hal_board_info_save(&d_info, FALSE);
    }
    else if (ctrl.content == MAC_CONF_CONTENT_TABLE)
    {
        mac_config_operate_assoc_t operate_assoc;
        osel_memcpy((uint8_t *)&operate_assoc, pbuf->data_p, sizeof(operate_assoc));
        pbuf->data_p += sizeof(operate_assoc);

        if (!osel_memcmp((uint8_t *)&d_info.unique_id,
                         (uint8_t *)&operate_assoc.dev_id, 8))
        {
            return;
        }
        crc8_c = crc_compute((uint8_t *)&operate_assoc,
                             sizeof(operate_assoc) - 1);
        if (crc8_c != operate_assoc.crc8)
        {
            return;
        }

        if (config_assoc_info_insert_cb)
            config_assoc_info_insert_cb(&operate_assoc.new_id);
    }

    osel_pthread_exit(mac_conf_task_tcb, &mac_config_start_process,
                      PROCESS_CURRENT());
    osel_pthread_create(mac_conf_task_tcb,
                        &mac_config_handle_process,
                        PROCESS_CURRENT());

    mac_config_send_confirm(crc8_c);
}


void mac_config_update_handle(pbuf_t *pbuf)
{
//    _DINT();
//    WDTCTL = WDTPW + WDTHOLD;
#ifndef PF_CORE_POSIX
    asm("mov &0x00BEFE, PC;");
#endif
}

PROCESS(mac_config_start_process, "mac_config_start_process");
PROCESS_THREAD(mac_config_start_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t config_cycle_etimer;    //*< 用于间隔发送关联帧的周期定时器
    mac_conf_times = 0;

    /**** 线程启动的时候全局中断并没有使能，只有在osel_run之后才启动全局中断，
    而radio发送需要等待中断，所以等待100ms，切出线程，让osel_run得到执行，启动GIE
    ************/
    OSEL_ETIMER_DELAY(&config_cycle_etimer, 100 / OSEL_TICK_PER_MS);
    while (1)
    {

        for (; mac_conf_times < CONFIG_SEND_MAX; mac_conf_times++)
        {
            osel_etimer_ctor(&config_cycle_etimer, PROCESS_CURRENT(),
                             PROCESS_EVENT_TIMER, NULL);
            osel_etimer_arm(&config_cycle_etimer, CONFIG_SEND_TIMES / OSEL_TICK_PER_MS, 0);

            mac_config_send_request();

            PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_EXIT) ||
                                     (ev == PROCESS_EVENT_TIMER));
            if (ev == PROCESS_EVENT_EXIT)
            {
                osel_etimer_disarm(&config_cycle_etimer);
                mac_conf_first_startup = FALSE;
                PROCESS_EXIT();
            }
            else if (ev == PROCESS_EVENT_TIMER)
            {
                ;
            }
        }

        mac_conf_first_startup = FALSE;
        SSN_RADIO.set_value(RF_DATARATE, 10);
        mac_assoc_start(mac_conf_task_tcb);
        PROCESS_EXIT();
    }

    PROCESS_END();
}


PROCESS(mac_config_handle_process, "mac_config_handle_process");
PROCESS_THREAD(mac_config_handle_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t config_delay_timer;

    while (1)
    {
        osel_etimer_ctor(&config_delay_timer, PROCESS_CURRENT(),
                         PROCESS_EVENT_TIMER, NULL);
        osel_etimer_arm(&config_delay_timer, OSEL_TICK_RATE_HZ, 0);
        m_tran_recv();

        PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_EXIT) ||
                                 (ev == PROCESS_EVENT_TIMER) ||
                                 (ev == PROCESS_EVENT_MSG));
        if (ev == PROCESS_EVENT_MSG)
        {
            osel_etimer_disarm(&config_delay_timer);
        }
        else if (ev == PROCESS_EVENT_EXIT)
        {
            osel_etimer_disarm(&config_delay_timer);
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_TIMER)
        {
            //*< timeout and save config
            hal_board_info_delay_save();
            //*< assoc table.
#if NODE_TYPE == NODE_TYPE_ROUTER
            extern bool_t config_bind_info_save(void);
            config_bind_info_save();
#endif
            hal_board_reset();
        }
    }

    PROCESS_END();
}

void mac_config_init(osel_task_t *task)
{
    mac_conf_task_tcb = task;
}

void mac_config_parse(pbuf_t *pbuf)
{
    mac_config_ctrl_t ctrl;
    osel_memcpy((uint8_t *)&ctrl, pbuf->data_p, sizeof(mac_config_ctrl_t));
    pbuf->data_p++;
    switch (ctrl.conf_type)
    {
    case MAC_CTRL_OPER:
        mac_config_operate_handle(ctrl, pbuf);
        break;

    case MAC_CTRL_UPDATE:
        mac_config_update_handle(pbuf);
        break;

    case MAC_CTRL_REQ:
    case MAC_CTRL_CFM:
        break;

    default:
        break;
    }
}

void mac_config_start(osel_task_t *task)
{
#if NODE_TYPE != NODE_TYPE_GATEWAY
    if (mac_conf_first_startup)
    {
        SSN_RADIO.set_value(RF_DATARATE, 7);
        phy_set_channel(CONFIG_RF_CHANNEL);
        osel_pthread_create(task, &mac_config_start_process, NULL);
    }
    else
    {
        SSN_RADIO.set_value(RF_DATARATE, 10);
        mac_assoc_start(task);
    }
#else
    m_tran_recv();
#endif
}

void mac_config_stop(osel_task_t *task)
{
#if NODE_TYPE != NODE_TYPE_GATEWAY
    osel_pthread_exit(task, &mac_config_start_process, NULL);
    osel_pthread_exit(task, &mac_config_handle_process, NULL);
#endif
}
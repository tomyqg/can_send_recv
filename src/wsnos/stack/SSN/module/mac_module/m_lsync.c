/**
 * @brief       : this is limit sync module for dc3.0
 * @file        : m_lsync.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-04-18
 * change logs  :
 * Date       Version     Author        Note
 * 2016-04-18  v0.0.1  gang.cheng    first version
 */
#include "platform/platform.h"
#include "common/dev/dev.h"
#include "common/lib/lib.h"

#include "mac_module.h"

static volatile hal_time_t  rxsfdtime;
static sync_cfg_t   sync_cfg;
static hal_time_t   sync_txt;
static hal_time_t   sync_rxt;

static void m_sync_sfd_isr(uint16_t time);
static void (*m_sfd_isr)(uint16_t);
static void rx_sfd(uint16_t time);
static void tx_sfd(uint16_t time);

#define SFD_TIMESTAMP_SIZE          (4u)

static void rx_sfd(uint16_t time)
{
    rxsfdtime = hal_timer_now();
    if (rxsfdtime.s.hw < time)
    {
        /*捕获发生到中断处理中获取时间过程中，定时器翻转，取消在获取当前时间
        函数中所做的软件时间修正*/
        rxsfdtime.s.sw--;
    }
    rxsfdtime.s.hw = time;
}

static void tx_sfd(uint16_t time)
{
    if (sync_cfg.tx_sfd_cap == TRUE)
    {
        // 1 capture current local time
        hal_time_t now = hal_timer_now();

        now.s.hw = time;

        // 2 convert to global time if needed
        //...

        // 3 send the stamp to current FIFO
        SSN_RADIO.prepare((uint8_t *)&now.w, sizeof(now.w));
        m_sfd_isr = rx_sfd;
    }
}

uint8_t m_sync_txfilter(pbuf_t *tpac)
{
    hal_time_t now;
    if (tpac == NULL)
    {
        return 0;
    }

    if ((*(tpac->head + sync_cfg.flag_byte_pos) & sync_cfg.flag_byte_msk) != sync_cfg.flag_byte_val)
    {
        return 0;
    }
    //*< all frames add 2bytes hardware time.
    if (sync_cfg.tx_sfd_cap)
    {
        //enable tx sfd cap
        m_sfd_isr = tx_sfd;
    }
    else
    {
        now = hal_timer_now();
        if (!sync_cfg.sync_source)
        {
            m_sync_l2g(&now);
        }

        now.w += sync_cfg.tx_offset;
        if (sync_cfg.stamp_byte_pos != 0)
        {
            // 时间戳在指定的字节起始处；
            osel_memcpy(tpac->head + sync_cfg.stamp_byte_pos,
                        &now.w,
                        sync_cfg.stamp_len);
        }
        else
        {
            // 时间戳在帧尾，这时，在后面需要修改帧头长度和data_len的长度;
            // 请检查sync_cfg.len_modfiy = TRUE;
#include "../../custom/phy/phy_packet.h"
            osel_memcpy(tpac->head + PHY_HEAD_SIZE + tpac->data_len,
                        &now.w, sync_cfg.stamp_len);
        }
    }

    return sync_cfg.stamp_len;
}


void m_sync_rxfilter(pbuf_t *rpac)
{
    hal_time_t now;

    if (rpac == NULL)
    {
        return;
    }

    if (!sync_cfg.sync_source)
    {
        return;
    }

    if ((*(rpac->head + sync_cfg.flag_byte_pos) & sync_cfg.flag_byte_msk) != sync_cfg.flag_byte_val)
    {
        return;
    }

    //*< all frames add 2bytes hardware time.
    //txt
    if (sync_cfg.stamp_byte_pos != 0)
    {
        osel_memcpy(&sync_txt, rpac->head + sync_cfg.stamp_byte_pos, sync_cfg.stamp_len);
        //  TODO: 去掉载荷里面的时间戳
    }
    else
    {
        osel_memcpy(&sync_txt, rpac->head + rpac->data_len - sync_cfg.stamp_len,
                    sync_cfg.stamp_len);
        rpac->data_len -= sync_cfg.stamp_len;
    }

    //rxt
    if (sync_cfg.rx_sfd_cap)
    {
        sync_rxt.w = rxsfdtime.w;
    }
    else
    {
        now = hal_timer_now();
        now.w += sync_cfg.rx_offset;
        sync_rxt.w = now.w;
    }

    //calculate
    rpac->attri.tx_sfd = sync_txt.w;    //*< 2bytes
    rpac->attri.rx_sfd = sync_rxt.w;    //*< 4bytes
}

void m_sync_g2l(hal_time_t *gtime)
{
    ;
}

void m_sync_l2g(hal_time_t *ltime)
{
    ;
}


static void m_sync_sfd_isr(uint16_t time)
{
    /* ewan: need to timestamp the rx sfd time, or read it from time cap*/
    if (m_sfd_isr)
    {
        m_sfd_isr(time);
    }
}

static void sync_reset(void)
{
    SSN_RADIO.int_cfg(RX_SFD_INT, m_sync_sfd_isr, INT_ENABLE);
    SSN_RADIO.int_cfg(RX_SFD_INT, m_sync_sfd_isr, INT_ENABLE);

    m_sfd_isr = rx_sfd;
}

static void sync_cfg_check(const sync_cfg_t *const cfg)
{
    DBG_ASSERT(cfg != NULL __DBG_LINE);

    if (cfg->tx_sfd_cap == TRUE)
    {
        /* 使用硬件TxSFD中断 */
        DBG_ASSERT(cfg->len_modfiy == TRUE __DBG_LINE);
    }
    else
    {
        /* 模拟TxSFD中断 */
        DBG_ASSERT(cfg->tx_offset != 0 __DBG_LINE);
    }
}

void m_sync_cfg(const sync_cfg_t *const cfg)
{
    DBG_ASSERT(cfg != NULL __DBG_LINE);

    sync_cfg_check(cfg);

    osel_memcpy(&sync_cfg, cfg, sizeof(sync_cfg_t));
}


void m_sync_enable(bool_t enable)
{
    ;
}


void m_sync_init(osel_task_t *task)
{
    sync_reset();
}

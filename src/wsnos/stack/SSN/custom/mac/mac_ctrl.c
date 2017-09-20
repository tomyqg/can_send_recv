/**
 * @brief       : this
 * @file        : mac_ctrl.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */
#include "common/hal/hal.h"
#include "../../custom/phy/phy_packet.h"
#include "../../common/pbuf.h"
#include "../../common/sbuf.h"
#include "../../module/mac_module/mac_module.h"

#include "mac.h"

mac_pib_t mac_pib_info;
mac_trans_info_t g_trans_info;

mac_head_t mac_head_info;
static frame_switch_t frame_switch[MAC_FRAME_TYPE_RESERVED];

DBG_THIS_MODULE("mac_ctrl");

#define DEBUG           1

mac_trans_info_t mac_trans_info_get(void)
{
    fp32_t success_rate = 0.0;
    fp32_t avg_retrans_num = 0.0;
    fp32_t avg_cca_num = 0.0;

    if (g_trans_info.mac_send_num != 0)
    {
        success_rate = (fp32_t)g_trans_info.mac_success_num / g_trans_info.mac_send_num;
        success_rate = success_rate * 100;
        avg_retrans_num =  (fp32_t)g_trans_info.mac_resent_num / g_trans_info.mac_success_num;
        avg_retrans_num  = avg_retrans_num * 10;
        avg_cca_num =  (fp32_t)g_trans_info.mac_cca_num / g_trans_info.mac_success_num;
        avg_cca_num  = avg_cca_num * 10;
    }

    g_trans_info.success_rate = success_rate > 100.0 ? 100 : (uint8_t)success_rate;
    g_trans_info.avg_retrans_num = avg_retrans_num > 90.0 ? 90 : (uint8_t)avg_retrans_num;
    g_trans_info.avg_cca_num = avg_cca_num > 90.0 ? 90 : (uint8_t)avg_cca_num;

    return g_trans_info;
}


void mac_trans_info_clr(void)
{
    osel_memset(&g_trans_info, 0x00, sizeof(g_trans_info));
}


static pbuf_t *mac_frame_get(void)
{
    pbuf_t *frame = NULL;
    frame = phy_get_packet();
    return frame;
}

static uint8_t get_addr(pbuf_t *pbuf, uint32_t *addr)
{
    memcpy(addr, pbuf->data_p, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;
    return MAC_ADDR_SIZE;
}

static bool_t mac_frame_head_parse(pbuf_t *pbuf)
{
    if (pbuf == NULL)
    {
        DBG_LOG(DBG_LEVEL_WARNING, "mac frame head parse fail! \r\n");
        return FALSE;
    }

    uint8_t mac_offset = 0;
    osel_memset(&mac_head_info, 0 , sizeof(mac_head_t));
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;

    osel_memcpy(&(mac_head_info.ctrl), pbuf->data_p, MAC_HEAD_CTRL_SIZE);
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;
    mac_offset += MAC_HEAD_CTRL_SIZE;

    mac_head_info.seq = (*pbuf->data_p);
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;
    mac_offset += MAC_HEAD_SEQ_SIZE;

    uint32_t dst_addr = 0;
    mac_offset += get_addr(pbuf, &dst_addr);
    mac_head_info.dst_addr = dst_addr;

    uint32_t src_addr = 0;
    mac_offset += get_addr(pbuf, &src_addr);
    mac_head_info.src_addr = src_addr;
    pbuf->attri.src_id = src_addr;

    if (mac_pib_info.mac_src_addr == src_addr)
    {
        // DBG_LOG(DBG_LEVEL_TRACE, "recv our send data\n");
        return FALSE;
    }

#if DEBUG   //*< debug for send frame

    DBG_LOG(DBG_LEVEL_INFO, "pack recv: ");
    for (uint8_t i = 0; i < pbuf->data_len; i++)
        DBG_LOG(DBG_LEVEL_ORIGIN, "0x%02x ", pbuf->head[i]);
    DBG_LOG(DBG_LEVEL_ORIGIN, "0x%02x", pbuf->attri.crc8);
    DBG_LOG(DBG_LEVEL_ORIGIN, "\n");
#endif

    assoc_node_t node;
    node.node_id = src_addr;
    if (mac_head_info.ctrl.frm_type != MAC_FRAME_TYPE_ACK)
    {
        if (mac_assoc_table_check(&node) == ASSOC_TABLE_FAILED)
        {
            return FALSE;
        }
    }
    
    pbuf->attri.dst_id      = mac_head_info.dst_addr;
    pbuf->attri.src_id      = mac_head_info.src_addr;
    pbuf->attri.need_ack    = mac_head_info.ctrl.ack_req;
    pbuf->attri.seq         = mac_head_info.seq;
    pbuf->attri.is_ack      =
        (mac_head_info.ctrl.frm_type == MAC_FRAME_TYPE_ACK) ? (TRUE) : (FALSE);

    return TRUE;
}

static bool_t mac_frame_parse(pbuf_t *pbuf)
{
    if (pbuf == NULL)
    {
        DBG_LOG(DBG_LEVEL_WARNING, "mac frame parse fail! \r\n");
        return FALSE;
    }
    DBG_ASSERT(pbuf != NULL __DBG_LINE);
    if (mac_head_info.ctrl.frm_type < MAC_FRAME_TYPE_RESERVED)
    {
        frame_switch[mac_head_info.ctrl.frm_type](pbuf);
    }
    else
    {
        m_tran_recv();
        pbuf_free(&pbuf __PLINE2);
    }
    return TRUE;
}

static void ack_tx_ok_callback(sbuf_t *sbuf, bool_t res)
{
    if (sbuf == NULL)
    {
        DBG_LOG(DBG_LEVEL_WARNING, "mack tx ok callback fail! \r\n");
        return ;
    }

    DBG_ASSERT(sbuf != NULL __DBG_LINE);
    pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
    sbuf_free(&sbuf __SLINE2);

    /**
     * 检测器在发送ack以后，转到sleep态，降低功耗，由下次应用层数据启动发送，
     * 中继器、接收主机需要在每次发送完ack以后，进入接收态接收其他设备的无线帧
     */
#if NODE_TYPE == NODE_TYPE_TAG

    m_tran_sleep();
#else

    m_tran_recv();
#endif

}

static void mac_send_ack(uint8_t seqno)
{
    sbuf_t *pending_sbuf = mac_pend_get(mac_head_info.src_addr);
    bool_t pending = (NULL == pending_sbuf) ? FALSE : TRUE;
    DBG_LOG(DBG_LEVEL_INFO, "PENDING: %d\r\n", pending);
    sbuf_t *sbuf = mac_ack_fill_package(pending, seqno, mac_head_info.src_addr);

    DBG_ASSERT(sbuf != NULL __DBG_LINE);
    if (m_tran_can_send())
    {
        m_tran_send(sbuf, ack_tx_ok_callback, 1);
    }
    else
    {
        pbuf_free(&(sbuf->primargs.pbuf) __PLINE2);
        sbuf_free(&sbuf __SLINE2);
    }

    if (pending)
        mac_pend_send(pending_sbuf);
}

static void mac_tx_finish(sbuf_t *const sbuf, bool_t result)
{
    DBG_ASSERT(FALSE __DBG_LINE);
}

void mac_ctrl_switch_init(frame_switch_t mac_data,
                          frame_switch_t mac_ack,
                          frame_switch_t mac_command)
{
    frame_switch[MAC_FRAME_TYPE_DATA] = mac_data;
    frame_switch[MAC_FRAME_TYPE_ACK] = mac_ack;
    frame_switch[MAC_FRAME_TYPE_COMMAND] = mac_command;
}

void mac_ctrl_init(void)
{
    tran_cfg_t mac_tran_cb;
    mac_tran_cb.frm_get             = mac_frame_get;
    mac_tran_cb.frm_head_parse      = mac_frame_head_parse;
    mac_tran_cb.frm_payload_parse   = mac_frame_parse;
    mac_tran_cb.tx_finish           = mac_tx_finish;
    mac_tran_cb.send_ack            = mac_send_ack;
    m_tran_cfg(&mac_tran_cb);
}

void mac_pib_init(mac_pib_t *pib)
{
    osel_memcpy((uint8_t *)&mac_pib_info, pib, sizeof(mac_pib_info));
}


#undef DEBUG
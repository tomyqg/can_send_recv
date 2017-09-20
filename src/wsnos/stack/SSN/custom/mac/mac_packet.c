/**
 * @brief       : this
 * @file        : mac_packet.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-24
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-24  v0.0.1  gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "../../common/pbuf.h"
#include "../../common/sbuf.h"
#include "../phy/phy_packet.h"
#include "mac_packet.h"
#include "mac_assoc.h"
#include "mac_ctrl.h"

DBG_THIS_MODULE("mac_packet");

sbuf_t *mac_assoc_respone_package(uint32_t dst_addr, assoc_result_e res)
{
    pbuf_t *pbuf = pbuf_alloc(LARGE_PBUF_BUFFER_SIZE __PLINE1);
    DBG_ASSERT(NULL != pbuf __DBG_LINE);
    sbuf_t *sbuf = sbuf_alloc(__SLINE1);
    DBG_ASSERT(NULL != sbuf __DBG_LINE);


    sbuf->primargs.pbuf = pbuf;
    sbuf->orig_layer = MAC_LAYER;
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;
    mac_frm_ctrl_t mac_frm_ctrl;
    mac_frm_ctrl.frm_type    = MAC_FRAME_TYPE_COMMAND;
    mac_frm_ctrl.frm_pending = FALSE;
    mac_frm_ctrl.ack_req     = FALSE;
    mac_frm_ctrl.send_cnt    = 0;
    osel_memcpy(pbuf->data_p, &mac_frm_ctrl, sizeof(mac_frm_ctrl_t));
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;

    osel_memcpy(pbuf->data_p, &mac_pib_info.mac_seq_num, sizeof(uint8_t));
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&dst_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&mac_pib_info.mac_src_addr,
                MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    *(uint8_t *)pbuf->data_p = MAC_CMD_ASSOC_RESP;
    pbuf->data_p += sizeof(uint8_t);

    assoc_resp_pld_t resp_pld;
    resp_pld.assoc_state = res;
    resp_pld.hops = mac_pib_hops();
    osel_memcpy(pbuf->data_p, &resp_pld, sizeof(assoc_resp_pld_t));
    pbuf->data_p += sizeof(assoc_resp_pld_t);

    pbuf->attri.seq = mac_pib_info.mac_seq_num++;
    pbuf->attri.need_ack = mac_frm_ctrl.ack_req;
    pbuf->attri.already_send_times.mac_send_times = 0;
    pbuf->attri.send_mode = TDMA_SEND_MODE; //*< TDMA_SEND_MODE
    pbuf->data_len = pbuf->data_p - pbuf->head - PHY_HEAD_SIZE;
    pbuf->attri.dst_id = dst_addr;

    return sbuf;
}


sbuf_t *mac_assoc_request_package(uint32_t src_addr)
{
    pbuf_t *pbuf = pbuf_alloc(LARGE_PBUF_BUFFER_SIZE __PLINE1);
    DBG_ASSERT(NULL != pbuf __DBG_LINE);
    sbuf_t *sbuf = sbuf_alloc(__SLINE1);
    DBG_ASSERT(NULL != sbuf __DBG_LINE);

    sbuf->primargs.pbuf = pbuf;
    sbuf->orig_layer = MAC_LAYER;
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;
    mac_frm_ctrl_t mac_frm_ctrl;
    mac_frm_ctrl.frm_type    = MAC_FRAME_TYPE_COMMAND;
    mac_frm_ctrl.frm_pending = FALSE;
    mac_frm_ctrl.ack_req     = FALSE;
    mac_frm_ctrl.send_cnt    = 0;
    osel_memcpy(pbuf->data_p, &mac_frm_ctrl, sizeof(mac_frm_ctrl_t));
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;

    osel_memcpy(pbuf->data_p, &mac_pib_info.mac_seq_num, sizeof(uint8_t));
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;

    uint32_t dst_addr = BROADCAST_ADDR;
    osel_memcpy(pbuf->data_p, (uint8_t *)&dst_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&src_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    *(uint8_t *)pbuf->data_p = MAC_CMD_ASSOC_REQ;
    pbuf->data_p += sizeof(uint8_t);

    assoc_requ_pld_t requ_pld;
    requ_pld.dev_type = NODE_TYPE;
    requ_pld.pan_id = PAN_ID;
    osel_memcpy(pbuf->data_p, &requ_pld, sizeof(assoc_requ_pld_t));
    pbuf->data_p += sizeof(assoc_requ_pld_t);

    pbuf->attri.seq = mac_pib_info.mac_seq_num++;
    pbuf->attri.need_ack = mac_frm_ctrl.ack_req;
    pbuf->attri.already_send_times.mac_send_times = 0;
    pbuf->attri.send_mode = TDMA_SEND_MODE; //*< TDMA_SEND_MODE
    pbuf->data_len = pbuf->data_p - pbuf->head - PHY_HEAD_SIZE;
    pbuf->attri.dst_id = dst_addr;

    return sbuf;
}


/**
 * @brief 组配置请求帧
 * @param  src_addr 发起配置请求的源地址
 * @param  requ_pld 发起配置请求的载荷
 * @return          sbuf
 */
sbuf_t *mac_config_request_package(uint32_t src_addr,
                                   mac_config_requ_pld_t *requ_pld)
{
    pbuf_t *pbuf = pbuf_alloc(LARGE_PBUF_BUFFER_SIZE __PLINE1);
    DBG_ASSERT(NULL != pbuf __DBG_LINE);
    sbuf_t *sbuf = sbuf_alloc(__SLINE1);
    DBG_ASSERT(NULL != sbuf __DBG_LINE);

    sbuf->primargs.pbuf = pbuf;
    sbuf->orig_layer = MAC_LAYER;
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;
    mac_frm_ctrl_t mac_frm_ctrl;
    mac_frm_ctrl.frm_type    = MAC_FRAME_TYPE_COMMAND;
    mac_frm_ctrl.frm_pending = FALSE;
    mac_frm_ctrl.ack_req     = FALSE;
    mac_frm_ctrl.send_cnt    = 0;
    osel_memcpy(pbuf->data_p, &mac_frm_ctrl, sizeof(mac_frm_ctrl_t));
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;

    osel_memcpy(pbuf->data_p, &mac_pib_info.mac_seq_num, sizeof(uint8_t));
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;

    uint32_t dst_addr = BROADCAST_ADDR;
    osel_memcpy(pbuf->data_p, (uint8_t *)&dst_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&src_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    *(uint8_t *)pbuf->data_p = MAC_CMD_CONTROL;
    pbuf->data_p += sizeof(uint8_t);

    mac_config_ctrl_t m_conf_ctrl;
    m_conf_ctrl.conf_type = MAC_CTRL_REQ;
    m_conf_ctrl.content   = 0x00;
    m_conf_ctrl.sub_type  = 0x00;
    osel_memcpy(pbuf->data_p, (uint8_t *)&m_conf_ctrl, sizeof(mac_config_ctrl_t));
    pbuf->data_p += sizeof(mac_config_ctrl_t);

    osel_memcpy(pbuf->data_p, (uint8_t *)requ_pld, sizeof(mac_config_requ_pld_t));
    pbuf->data_p += sizeof(mac_config_requ_pld_t);

    pbuf->attri.seq = mac_pib_info.mac_seq_num++;
    pbuf->attri.need_ack = mac_frm_ctrl.ack_req;
    pbuf->attri.already_send_times.mac_send_times = 0;
    pbuf->attri.send_mode = TDMA_SEND_MODE; //*< TDMA_SEND_MODE
    pbuf->data_len = pbuf->data_p - pbuf->head - PHY_HEAD_SIZE;
    pbuf->attri.dst_id = dst_addr;

    return sbuf;
}


/**
 * @brief 组配置确认帧
 * @param  dst_addr 配置确认对应的目的设备地址
 * @param  confirm  确认信息，是与配置操作对应的CRC8
 * @return          sbuf
 */
sbuf_t *mac_config_confirm_package(uint32_t src_addr,
                                   mac_config_confirm_t *confirm)
{
    pbuf_t *pbuf = pbuf_alloc(LARGE_PBUF_BUFFER_SIZE __PLINE1);
    DBG_ASSERT(NULL != pbuf __DBG_LINE);
    sbuf_t *sbuf = sbuf_alloc(__SLINE1);
    DBG_ASSERT(NULL != sbuf __DBG_LINE);

    sbuf->primargs.pbuf = pbuf;
    sbuf->orig_layer = MAC_LAYER;
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;
    mac_frm_ctrl_t mac_frm_ctrl;
    mac_frm_ctrl.frm_type    = MAC_FRAME_TYPE_COMMAND;
    mac_frm_ctrl.frm_pending = FALSE;
    mac_frm_ctrl.ack_req     = FALSE;
    mac_frm_ctrl.send_cnt    = 0;
    osel_memcpy(pbuf->data_p, &mac_frm_ctrl, sizeof(mac_frm_ctrl_t));
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;

    osel_memcpy(pbuf->data_p, &mac_pib_info.mac_seq_num, sizeof(uint8_t));
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;

    uint32_t dst_addr = BROADCAST_ADDR;
    osel_memcpy(pbuf->data_p, (uint8_t *)&dst_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&src_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    *(uint8_t *)pbuf->data_p = MAC_CMD_CONTROL;
    pbuf->data_p += sizeof(uint8_t);

    mac_config_ctrl_t m_conf_ctrl;
    m_conf_ctrl.conf_type = MAC_CTRL_CFM;
    m_conf_ctrl.content   = 0x00;
    m_conf_ctrl.sub_type  = 0x00;
    osel_memcpy(pbuf->data_p, (uint8_t *)&m_conf_ctrl, sizeof(mac_config_ctrl_t));
    pbuf->data_p += sizeof(mac_config_ctrl_t);

    osel_memcpy(pbuf->data_p, (uint8_t *)confirm, sizeof(mac_config_confirm_t));
    pbuf->data_p += sizeof(mac_config_confirm_t);

    pbuf->attri.seq = mac_pib_info.mac_seq_num++;
    pbuf->attri.need_ack = mac_frm_ctrl.ack_req;
    pbuf->attri.already_send_times.mac_send_times = 0;
    pbuf->attri.send_mode = TDMA_SEND_MODE; //*< TDMA_SEND_MODE
    pbuf->data_len = pbuf->data_p - pbuf->head - PHY_HEAD_SIZE;
    pbuf->attri.dst_id = dst_addr;

    return sbuf;
}

/**
 * @brief 组ACK帧
 * @param  pending  接收ACK以后是否有下行数据
 * @param  seq      序列号
 * @param  dst_addr 目的地址
 * @return          sbuf
 */
sbuf_t *mac_ack_fill_package(bool_t pending, uint8_t seq, uint32_t dst_addr)
{
    pbuf_t *pbuf = pbuf_alloc(LARGE_PBUF_BUFFER_SIZE __PLINE1);
    DBG_ASSERT(NULL != pbuf __DBG_LINE);
    sbuf_t *sbuf = sbuf_alloc(__SLINE1);
    DBG_ASSERT(NULL != sbuf __DBG_LINE);

    sbuf->primargs.pbuf = pbuf;
    sbuf->orig_layer = MAC_LAYER;
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;
    mac_frm_ctrl_t mac_frm_ctrl;
    mac_frm_ctrl.frm_type    = MAC_FRAME_TYPE_ACK;
    mac_frm_ctrl.frm_pending = pending;
    mac_frm_ctrl.ack_req     = FALSE;
    mac_frm_ctrl.send_cnt    = 0;
    osel_memcpy(pbuf->data_p, &mac_frm_ctrl, sizeof(mac_frm_ctrl_t));
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;

    osel_memcpy(pbuf->data_p, &seq, sizeof(uint8_t));
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&dst_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    uint32_t src_addr = mac_pib_src_saddr();
    osel_memcpy(pbuf->data_p, (uint8_t *)&src_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    pbuf->attri.seq = seq;
    pbuf->attri.need_ack = mac_frm_ctrl.ack_req;
    pbuf->attri.already_send_times.mac_send_times = 0;
    pbuf->attri.send_mode = TDMA_SEND_MODE;
    pbuf->data_len = pbuf->data_p - pbuf->head - PHY_HEAD_SIZE;
    pbuf->attri.dst_id = dst_addr;
    return sbuf;
}

/**
 * @brief 填充mac数据帧未填充的部分
 * @param sbuf 要填充的数据缓存区域
 * @param dst_addr 数据要发送到的目的地址
 */
void mac_data_fill_package(sbuf_t *sbuf, uint32_t dst_addr)
{
    if (sbuf == NULL)
    {
        DBG_LOG(DBG_LEVEL_ERROR, "mac data fill package false!\r\n");
        return;
    }

    DBG_ASSERT(sbuf != NULL __DBG_LINE);
    DBG_ASSERT(sbuf->primargs.pbuf != NULL __DBG_LINE);

    pbuf_t *pbuf = sbuf->primargs.pbuf;
    pbuf->data_p = pbuf->head + PHY_HEAD_SIZE;
    mac_frm_ctrl_t mac_frm_ctrl;
    mac_frm_ctrl.frm_type    = MAC_FRAME_TYPE_DATA;
    mac_frm_ctrl.frm_pending = FALSE;
    mac_frm_ctrl.ack_req     = TRUE;
    mac_frm_ctrl.send_cnt    = 0;
    osel_memcpy(pbuf->data_p, &mac_frm_ctrl, sizeof(mac_frm_ctrl_t));
    pbuf->data_p += MAC_HEAD_CTRL_SIZE;

    osel_memcpy(pbuf->data_p, &mac_pib_info.mac_seq_num, sizeof(uint8_t));
    pbuf->data_p += MAC_HEAD_SEQ_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&dst_addr, MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    osel_memcpy(pbuf->data_p, (uint8_t *)&mac_pib_info.mac_src_addr,
                MAC_ADDR_SIZE);
    pbuf->data_p += MAC_ADDR_SIZE;

    pbuf->attri.seq = mac_pib_info.mac_seq_num++;
    pbuf->attri.need_ack = mac_frm_ctrl.ack_req;
    pbuf->attri.already_send_times.mac_send_times = 0;
    pbuf->attri.send_mode = CSMA_SEND_MODE; //*< TDMA_SEND_MODE
    pbuf->attri.dst_id = dst_addr;

    pbuf->data_len += pbuf->data_p - pbuf->head - PHY_HEAD_SIZE;
}
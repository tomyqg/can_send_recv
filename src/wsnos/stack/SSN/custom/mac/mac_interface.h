#ifndef __MAC_INTERFACE_H__
#define __MAC_INTERFACE_H__

#include "../../common/sbuf.h"
#include "../../custom/phy/phy_packet.h"

#define MAC_OFFSET_SIZE         (PHY_HEAD_SIZE + MAC_HEAD_CTRL_SIZE + \
                                 MAC_HEAD_SEQ_SIZE + MAC_ADDR_SIZE*2)

typedef struct
{
    uint8_t  rf_channel;               //*< rf信道
    uint8_t  rf_power;
    uint8_t  mac_seq_num;
    uint32_t mac_src_addr;
    uint64_t mac_self_addr;

    uint8_t mac_hops;

    uint32_t mac_coord_addr;
    int8_t   mac_coord_rssi;
} mac_pib_t;

typedef struct
{
    void (*recv_cb)(sbuf_t *const sbuf);                    //*< 接收回调
    void (*send_cb)(sbuf_t *const sbuf, bool_t state);      //*< 发送回调
    void (*mac_assoc_cb)(bool_t state);                     //*< 保留，考虑检测器
    int8_t (*mac_assoc_vailed)(uint32_t src_id);            //*< 用于判断ID是否在关联表里面，且直连
} mac_dependent_t;

extern mac_dependent_t mac_cb;

#endif
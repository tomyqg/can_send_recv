/***************************************************************************
* File          :   app_module_send.h
* Author        :   wangjifang
* Create Time       :   2017-05-09 14:10:30
* Version       :   v0.0.1
* Summary       :
* Last Modified by  :   wangjifang
* Last Modified time    :   2017-05-09 14:10:30
***************************************************************************/
#pragma once

#define FRM_TYPE_CONFIRM                (1u)
#define FRM_TYPE_UNCONFIRM              (0u)
#define FRM_MAC_HEAD_LEN                (2u)
typedef enum
{
    TYPE_MAC_FRM_NWK_JOIN= 0,
    TYPE_MAC_FRM_NWK_REQ,
    TYPE_MAC_FRM_ACK,
    TYPE_MAC_FRM_DATA,
    TYPE_MAC_FRN_RESERVE,
} type_mac_frm_e;

#pragma pack(1)

typedef struct
{
    list_head_t list;
    uint8_t *date_p;
    uint8_t data_len;
    uint8_t send_cnt;
    uint16_t data_type;
}app_data_buf_t;

typedef struct _mac_head_t_
{
    uint16_t    dst_nwk_id  :   7,
                frm_type    :   3,
                ack_req     :   1,
                rtr         :   1,
                ide         :   1,
                res         :   3;
} mac_head_t; 

typedef struct _app_package_t_
{
    uint16_t package_numb;
    uint16_t payload_len;
    uint16_t reserve;
} app_package_t; 

#pragma pack(1)

typedef void (*recv_frm_cb)(uint8_t *buf, uint8_t len);

void app_module_send_init(recv_frm_cb handler, osel_task_t *task);

void app_send_data(frame_app_t frame_app_data);

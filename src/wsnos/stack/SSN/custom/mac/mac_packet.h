/**
 * @brief       : this
 * @file        : mac_packet.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */
#ifndef __MAC_PACKET_H__
#define __MAC_PACKET_H__

#include "mac_interface.h"

#define MAC_FCS_SIZE                            0u
#define MAC_HEAD_CTRL_SIZE                      1u
#define MAC_HEAD_SEQ_SIZE                       1u
#define MAC_ADDR_SIZE                           4u

#define PAN_ID              (0x1234u)
#define BROADCAST_ADDR      (0xFFFFFFFFu)

typedef enum
{
    MAC_FRAME_TYPE_DATA = 0,
    MAC_FRAME_TYPE_ACK,
    MAC_FRAME_TYPE_COMMAND,
    MAC_FRAME_TYPE_RESERVED,
} mac_frame_switch_e;

typedef enum
{
    MAC_CMD_ASSOC_REQ = 1,
    MAC_CMD_ASSOC_RESP,
    MAC_CMD_CONTROL,
} mac_frame_type_e;

typedef enum
{
    MAC_CTRL_REQ = 0,
    MAC_CTRL_OPER,
    MAC_CTRL_CFM,
    MAC_CTRL_UPDATE,
} mac_conf_type_e;

typedef enum
{
    MAC_CONF_CONTENT_INFO,
    MAC_CONF_CONTENT_TABLE,
} mac_conf_content_e;


typedef struct
{
    uint8_t  frm_type           :    2,             //*<  MAC帧类型
             frm_pending        :    1,             //*<  下行等待
             ack_req            :    1,             //*<  是否需要ACK应答
             send_cnt           :    2,             //*<  重传次数
             reserved           :    2;
} mac_frm_ctrl_t;

typedef struct
{
    mac_frm_ctrl_t ctrl;
    uint8_t     seq;
    uint32_t    dst_addr;
    uint32_t    src_addr;
} mac_head_t;


#pragma pack(1)
typedef struct
{
    uint8_t dev_type;
    uint16_t pan_id;
} assoc_requ_pld_t;

typedef enum
{
    ASSOC_SUCCESS,
    ASSOC_PAN_FAILED,
    ASSOC_FULL,
    ASSOC_ID_FAILED,
} assoc_result_e;

typedef struct
{
    uint8_t assoc_state;
    uint8_t hops;
} assoc_resp_pld_t;


typedef struct 
{
    uint8_t conf_type : 2,
            content   : 2,
            sub_type  : 4;
} mac_config_ctrl_t;

#define MAC_CONF_UPDATE_ENTER       0x00

typedef struct
{
    uint64_t dev_id;
    uint8_t channel;
    uint8_t power;
    uint8_t ver[3];
    uint8_t crc8;
} mac_config_requ_pld_t;

typedef struct
{
    uint64_t dev_id;
    uint64_t new_id;
    uint8_t channel;
    uint8_t power;
    uint8_t crc8;
} mac_config_operate_info_t;


typedef struct
{
    uint64_t dev_id;
    uint64_t new_id;
    uint8_t crc8;
} mac_config_operate_assoc_t;



typedef struct 
{
    uint8_t crc8; 
} mac_config_confirm_t;

#pragma pack()

/**
 * @brief 组关联请求帧，并返回存储数据的buf
 * @param  src_addr 源设备ID
 * @return  sbuf
 */
sbuf_t *mac_assoc_request_package(uint32_t src_addr);

/**
* @brief  组关联应答帧
* @param[in] des_addr 目的地址
* @return   sbuf
*/
sbuf_t *mac_assoc_respone_package(uint32_t des_addr, assoc_result_e res);

/**
 * @brief 组配置请求帧
 * @param  src_addr 发起配置请求的源地址
 * @param  requ_pld 发起配置请求的载荷
 * @return          sbuf
 */
sbuf_t *mac_config_request_package(uint32_t src_addr,
                                   mac_config_requ_pld_t *requ_pld);


/**
 * @brief 组配置确认帧
 * @param  dst_addr 配置确认对应的目的设备地址
 * @param  confirm  确认信息，是与配置操作对应的CRC8
 * @return          sbuf
 */
sbuf_t *mac_config_confirm_package(uint32_t src_addr,
                                   mac_config_confirm_t *confirm);

/**
 * @brief 组ACK帧
 * @param  pending  接收ACK以后是否有下行数据
 * @param  seq      序列号
 * @param  dst_addr 目的地址
 * @return          sbuf
 */
sbuf_t *mac_ack_fill_package(bool_t pending, uint8_t seq, uint32_t dst_addr);

/**
 * @brief 填充mac数据帧未填充的部分
 * @param sbuf 要填充的数据缓存区域
 * @param dst_addr 数据要发送到的目的地址
 */
void mac_data_fill_package(sbuf_t *sbuf, uint32_t dst_addr);

#endif
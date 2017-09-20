/***************************************************************************
* File          :   app_module_frame.h
* Author        :   wangjifang
* Create Time       :   2017-05-09 09:00:00
* Version       :   v0.0.1
* Summary       :
* Last Modified by  :   wangjifang
* Last Modified time    :   2017-05-09 09:00:00
***************************************************************************/
#pragma once

#pragma pack(1)
typedef enum
{
    TYPE_APP_FRM_ALARM = 0,
    TYPE_APP_FRM_FEATURE,
    TYPE_APP_FRM_ORIGINAL_DATA,
    TYPE_APP_FRM_HB,
    TYPE_APP_FRM_CONFIG_REQUEST,
    TYPE_APP_FRM_MAX,
} type_app_frm_e;

typedef enum
{
    TYPE_FRM_DEV_CRAWL = 0,                     //围栏
    TYPE_FRM_DEV_BRICK_WALL,                    //砖墙
    TYPE_FRM_MAX,
} type_app_dev_e;

typedef enum
{
    TYPE_APP_FRM_TRANS_DISABLE= 0,
    TYPE_APP_FRM_TRANS_START,
    TYPE_APP_FRM_TRANS_RUNNING,
    TYPE_APP_FRM_TRANS_END,
} type_app_frm_trans_e;

typedef enum
{
    TYPE_FRM_HB_NORMARL = 0,
    TYPE_FRM_HB_RESET,
    TYPE_FRM_HB_REJOIN,
} type_app_hb_e;

typedef enum
{
    TYPE_FRM_CFG_REQ = 0,
    TYPE_FRM_CFG_OPEREA,
} type_app_cfg_e;

typedef enum
{
    TYPE_FRM_CFG_TYPE_HB = 0,
    TYPE_FRM_CFG_TYPE_ORIGIN,
    TYPE_FRM_CFG_TYPE_RESET,
    TYPE_FRM_CFG_TYPE_DEBUG,
    TYPE_FRM_CFG_TYPE_ALARM,
} type_app_cfg_type_e;

typedef struct _frm_alarm_t_
{
    uint8_t rtc[6];
} frm_alarm_t;

typedef struct _frm_feature_t_
{
    frm_alarm_t rtc;
    uint16_t energy;
    uint16_t ez_max0;
    uint16_t ez_max8;
    uint16_t ez_max16;
    uint16_t ez_max32;
    uint16_t ez_max64;
    uint16_t ez_max128;
    uint16_t ez_max256;
    uint16_t ez_max512;
    uint16_t res1;
    uint16_t res2;
} frm_feature_t;

typedef struct _frm_origin_data_t_
{
    frm_alarm_t rtc;
    uint16_t origin_data[512];
} frm_origin_data_t;

typedef struct _normal_hb_t_
{
    uint16_t hb_cycle;
    uint8_t ver[3];
} normal_hb_t;

typedef struct _frm_hb_t_
{
    uint8_t hb_type;
    union
    {
        normal_hb_t normal_hb_data;
        uint16_t reset_line;
    };
} frm_hb_t;

typedef struct _frm_config_t_
{
    uint8_t config_ctrl;
    uint8_t config_type;
} frm_config_t;

typedef struct _frm_config_ack_t_
{
    uint16_t hb_cycle;
} frm_config_ack_t;

typedef struct _app_ctrl_t_
{
    uint8_t frm_type    :   3,
            dev_type    :   3,
            transmit    :   2;
} app_ctrl_t;

typedef struct _frame_app_head_t_
{
    app_ctrl_t app_ctrl;
    uint8_t seq_num;  
} frame_app_head_t;

typedef struct _frame_app_t_
{
    app_ctrl_t app_ctrl;
    uint8_t seq_num;        
    union
    {
        frm_alarm_t rtc_val;
        frm_feature_t feature_val;
        frm_origin_data_t origin_data_val;
        frm_hb_t hb_info;
        frm_config_t config_info;
        frm_config_ack_t config_info_ack;
    };
} frame_app_t;

#pragma pack()

/**
 * @brief lsn设备管理数据填充
 * @param[in] manage_type  管理数据类型
 * @param[in] payload      管理数据载荷
 * @param[in] frame_type_data  填充的管理数据结构体地址
 * @return 返回操作正确或失败
 */

uint16_t app_alarm_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, uint8_t seq,
                            frm_alarm_t *frame_type_alarm);

/**
 * @brief lsn调试信息数据填充
 * @param[in] debug_type  调试帧类型
 * @param[in] payload  调试帧载荷
 * @param[in] frame_type_data  填充的据结构体地址
 * @param[in] data_len  填充的数据长度
 * @return 返回操作正确或失败
 */
uint16_t app_feature_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, 
                               uint8_t seq, frm_feature_t *frame_type_feature);

/**
 * @brief lsn电量信息数据填充
 * @param[in] data_pld  电量帧载荷
 * @param[in] frame_type_data  填充的据结构体地址
 * @return 返回操作正确或失败
 */
uint16_t app_hb_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, 
                          uint8_t seq, frm_hb_t *frame_type_hb);

/**
 * @brief lsn车辆信息数据填充
 * @param[in] app_sub_type  车辆状态类型
 * @param[in] data_numb    车辆帧序列号
 * @param[in] frame_type_data  填充的车辆数据结构体地址
 * @return 返回操作正确或失败
 */
uint16_t app_config_data_fill(const uint8_t *payload, 
                              app_ctrl_t app_ctrl, uint8_t seq, 
                              frm_config_ack_t *frame_type_config_ack);
 
 uint16_t app_origin_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, 
                               uint8_t seq, frm_origin_data_t *frame_type_origin);

uint16_t app_config_data_analy(const uint8_t *payload, uint8_t seq,
                                  frm_config_t *frame_type_config);


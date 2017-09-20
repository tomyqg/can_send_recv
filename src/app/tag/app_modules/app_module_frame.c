/***************************************************************************
* File          :   app_module_frame.c
* Author        :   wangjifang
* Create Time       :   2017-05-09 08:58:53
* Version       :   v0.0.1
* Summary       :
* Last Modified by  :   wangjifang
* Last Modified time    :   2017-05-09 08:58:53
***************************************************************************/
#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "sys_arch/osel_arch.h"
#include "app_module_frame.h"

uint16_t app_alarm_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, uint8_t seq,
                            frm_alarm_t *frame_type_alarm)
{
    uint8_t *data_p = (uint8_t *)payload;

    *data_p = *(uint8_t*)&app_ctrl;
    data_p ++;
    *data_p = seq;
    data_p ++;
    osel_memcpy(data_p, (uint8_t *)frame_type_alarm, sizeof(frm_alarm_t));
    data_p += sizeof(frm_alarm_t);
    return (data_p - payload);
}


uint16_t app_origin_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, 
                               uint8_t seq, frm_origin_data_t *frame_type_origin)
{
    uint8_t *data_p = (uint8_t *)payload;
    
    *data_p = *(uint8_t*)&app_ctrl;
    data_p ++;
    *data_p = seq;
    data_p ++;
    
    osel_memcpy(data_p, (uint8_t *)frame_type_origin, sizeof(frm_feature_t));
    data_p += sizeof(frm_feature_t);
    return (data_p - payload);
}

uint16_t app_hb_data_fill(const uint8_t *payload, app_ctrl_t app_ctrl, 
                          uint8_t seq, frm_hb_t *frame_type_hb)
{
    uint8_t *data_p = (uint8_t *)payload;
        
    *data_p = *(uint8_t*)&app_ctrl;
    data_p ++;
    *data_p = seq;
    data_p ++;
    *data_p = frame_type_hb->hb_type;
    data_p ++;
    if(frame_type_hb->hb_type == TYPE_FRM_HB_NORMARL)
    {
        osel_memcpy(data_p, (uint8_t *)&(frame_type_hb->normal_hb_data), 
                    sizeof(normal_hb_t));
        data_p += sizeof(normal_hb_t);
    }
    else if(frame_type_hb->hb_type == TYPE_FRM_HB_RESET)
    {
        osel_memcpy(data_p, (uint8_t *)&(frame_type_hb->reset_line), sizeof(uint16_t));
        data_p += sizeof(uint16_t);
    }
    return (data_p - payload);
}

uint16_t app_config_data_fill(const uint8_t *payload,  
                              app_ctrl_t app_ctrl, uint8_t seq,
                              frm_config_ack_t *frame_type_config_ack)
{
    uint8_t *data_p = (uint8_t *)payload;
    
    *data_p = *(uint8_t*)&app_ctrl;
    data_p ++;
    *data_p = seq;
    data_p ++;
    
    osel_memcpy(data_p, (uint8_t *)frame_type_config_ack, 
                sizeof(frm_config_ack_t));
    data_p += sizeof(frm_config_ack_t);

    return (data_p - payload);
}

uint16_t app_config_data_analy(const uint8_t *payload, uint8_t seq,
                                  frm_config_t *frame_type_config)
{
    uint8_t *data_p = (uint8_t *)payload;
    
    frame_app_t frame_app_tmp;
    osel_memcpy((uint8_t *)&(frame_app_tmp.app_ctrl), data_p, sizeof(uint8_t));
    
    if(frame_app_tmp.app_ctrl.frm_type != TYPE_APP_FRM_CONFIG_REQUEST)
        return 0;
    
    data_p ++;
    frame_app_tmp.seq_num = *data_p;
    data_p ++;
    
    osel_memcpy((uint8_t *)frame_type_config, data_p, sizeof(frm_config_t));
    data_p += sizeof(frm_config_t);

    return (data_p - payload);
}
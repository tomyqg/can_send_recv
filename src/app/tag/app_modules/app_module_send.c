/***************************************************************************
* File          :   app_module_send.c
* Author        :   wangjifang
* Create Time       :   2017-05-09 14:16:33
* Version       :   v0.0.1
* Summary       :
* Last Modified by  :   wangjifang
* Last Modified time    :   2017-05-09 14:16:32
***************************************************************************/
#include "gz_sdk.h"
#include "../tag/app.h"
#include "app_module_frame.h"
#include "app_module_send.h"
#include "app_module_cfg.h"

#define ASSOC_PERIOD_TIME           (10000ul)   //60s
#define ASSOC_SLEEP_TIME            (400ul)     //400ms
#define RESEND_MAX_CNT              (1u)        //最大重传次数3
#define LOST_MAX_CNT                (6u)        //最大连续丢帧个数
#define ONE_FRM_LEN_MAX             (8u)       //最大帧长
#define ONE_FRM_PLD_SIZE            (6u)        //一帧载荷大小
#define APP_MBUF_NUM                (500u)
#define APP_MBUF_SIZE               (40u)
#define FRM_DATA_MAX                (1024u)     //最大帧长
#define DEVICE_NWK_ID_MASK          (0x0000007f)

static osel_task_t *app_task = NULL;
static mpool_manage_t mpool_app_manage;

static bool_t   app_can_send = FALSE;
static bool_t   app_join_flag = FALSE;
static list_head_t databuf_list_head;
static recv_frm_cb recv_frm_handle_cb = NULL;
static uint8_t  app_continuous_lost_frm = 0;
static uint8_t  app_seq = 0;
static osel_etimer_t app_wait_ack_etimer;
static app_cfg_info_t app_cfg_device_info_local;

static void can_send(msg_flag_t flag, app_data_buf_t *app_can_data);

PROCESS_NAME(app_ack_timeout_process);
DBG_THIS_MODULE("app_module_send");

static bool_t lsn_databuf_init(void)
{
    list_init(&databuf_list_head);
    return TRUE;
}

static void lsn_databuf_insert(app_data_buf_t *sbuf)
{
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    list_add_to_tail(&(sbuf->list), &databuf_list_head);
    OSEL_EXIT_CRITICAL(s);
}

static app_data_buf_t *lsn_databuf_remove(void)
{
    app_data_buf_t *sbuf = NULL;

    if (list_empty(&databuf_list_head))
    {
        return NULL;
    }

    if (!list_empty(&databuf_list_head))
    {
        app_data_buf_t *sbuf = list_entry_decap(&databuf_list_head, app_data_buf_t, list);
        return sbuf;
    }

    return sbuf;
}

static app_data_buf_t *lsn_databuf_look(void)
{
    app_data_buf_t *sbuf = NULL;

    if (list_empty(&databuf_list_head))
    {
        return NULL;
    }

    if (!list_empty(&databuf_list_head))
    {
        sbuf =
            list_entry_addr_find(list_first_elem_look(&databuf_list_head),
                                 app_data_buf_t, list);
        return sbuf;
    }

    return sbuf;
}

static uint8_t lsn_databuf_cnt(void)
{
    uint8_t cnt = 0;
    list_count(&databuf_list_head, cnt);
    return cnt;
}

/******************************************************************************/
static void fill_app_head(frame_app_head_t *dst_p, uint8_t app_frm_type,
                          uint8_t app_sub_type, uint8_t trans_flag)
{
     if(dst_p == 0 || app_frm_type > TYPE_APP_FRM_MAX)
        return;
     
     dst_p->app_ctrl.frm_type = app_frm_type;
     dst_p->app_ctrl.dev_type = TYPE_FRM_DEV_CRAWL;
     dst_p->app_ctrl.transmit = trans_flag;
     dst_p->seq_num = app_seq ++;
}

static void fill_mac_head(mac_head_t *dst_p, uint8_t mac_frm_type,
                              uint8_t *data_pld_p, uint16_t date_len)
{
    if(dst_p == 0 || mac_frm_type > TYPE_MAC_FRN_RESERVE)
        return;
    
    config_module_read(&app_cfg_device_info_local);
    if(mac_frm_type == TYPE_MAC_FRM_DATA)
        dst_p->ack_req = TRUE;
    else
        dst_p->ack_req = FALSE;
    
    dst_p->dst_nwk_id = app_cfg_device_info_local.device_id;
    dst_p->frm_type = mac_frm_type;
    dst_p->res = 0;
    dst_p->rtr = 0;
    dst_p->ide = 0;
}

static uint16_t fill_data_buf(uint8_t const *dst_p, frame_app_head_t frame_app_head,
                              mac_head_t mac_head, uint8_t *src_p, 
                              uint8_t data_len)
{
    uint8_t *data_tmp_p = (uint8_t *)dst_p;

    osel_memcpy(data_tmp_p, (uint8_t *)&mac_head, sizeof(mac_head_t));
    data_tmp_p += sizeof(mac_head_t);
    
    osel_memcpy(data_tmp_p, (uint8_t *)&frame_app_head, sizeof(frame_app_head_t));
    data_tmp_p += sizeof(frame_app_head_t);
    
    osel_memcpy(data_tmp_p, (uint8_t *)&src_p, data_len);
    data_tmp_p += data_len;
    
    return (data_tmp_p - dst_p);
}

static void app_send_join_frm(void)
{
    app_data_buf_t *data_buf = NULL;
    uint8_t *data_tmp_p = NULL;
    mac_head_t frm_mac_head;
    app_cfg_info_t app_cfg_info_tmp;
    config_module_read(&app_cfg_info_tmp);
        
    data_buf = (app_data_buf_t *)mpool_alloc(&mpool_app_manage, 
                                             APP_MBUF_SIZE __MLINE);
    if(data_buf == NULL)
    {
        DBG_LOG(DBG_LEVEL_INFO, "mpool alloc fail!");
        return;
    }
    data_buf->date_p = (uint8_t *)data_buf + sizeof(app_data_buf_t);
    data_tmp_p =    data_buf->date_p; 
    fill_mac_head(&frm_mac_head, TYPE_MAC_FRM_NWK_JOIN, 0, 0);
    
    osel_memcpy(data_tmp_p, (uint8_t *)&frm_mac_head, sizeof(frm_mac_head));
    data_tmp_p += sizeof(frm_mac_head);
    osel_memcpy(data_tmp_p, (uint8_t *)&(app_cfg_info_tmp.unique_id), sizeof(uint64_t));
    data_tmp_p += sizeof(uint64_t);
    
    data_buf->data_len = data_tmp_p - data_buf->date_p;
    data_buf->data_type = FRM_TYPE_UNCONFIRM;
    if(data_buf->data_len + data_tmp_p > (uint8_t *)data_buf + APP_MBUF_SIZE)
    {
        DBG_LOG(DBG_LEVEL_INFO, "copy data over len!");
        mpool_free((void **)&data_buf __MLINE);
        return;
    }
    
    can_send(MSG_UNCONFIRMED, data_buf);
}
static void app_one_frm_send(uint8_t *data_p, uint16_t data_len, 
                         uint8_t app_type, uint8_t app_sub_type)
{
    app_data_buf_t *data_buf = NULL;
    frame_app_head_t frame_app_head;
    mac_head_t frm_mac_head;
    
    data_buf = (app_data_buf_t *)mpool_alloc(&mpool_app_manage, 
                                             APP_MBUF_SIZE __MLINE);
    if(data_buf == NULL)
    {
        DBG_LOG(DBG_LEVEL_INFO, "mpool alloc fail!");
        return;
    }
    data_buf->date_p = (uint8_t *)data_buf + sizeof(app_data_buf_t);
    
    fill_app_head(&frame_app_head, app_type, app_sub_type, 
                  TYPE_APP_FRM_TRANS_DISABLE);
    
    fill_mac_head(&frm_mac_head, TYPE_MAC_FRM_DATA, data_p, data_len);
    data_buf->data_type = FRM_TYPE_CONFIRM;
    data_buf->data_len = fill_data_buf(data_buf->date_p, frame_app_head, 
                                       frm_mac_head, data_p, data_len);
    
    if(data_buf->data_len + data_buf->date_p > (uint8_t *)data_buf + APP_MBUF_SIZE)
    {
        DBG_LOG(DBG_LEVEL_INFO, "copy data over len!");
        mpool_free((void **)&data_buf __MLINE);
        return;
    }
    
    lsn_databuf_insert(data_buf);
}

static void unpack_package(uint8_t *dst_p, uint8_t package_seq, 
                           uint8_t *src_p, uint8_t data_len)
{
    if(dst_p == 0 || src_p == 0)
        return;
    
    *dst_p = package_seq;
    dst_p ++;
    osel_memcpy(dst_p, src_p, data_len);
}

static void app_more_frm_send(uint8_t *data_p, uint16_t data_len, 
                         uint8_t app_type, uint8_t app_sub_type)
{
    frame_app_head_t frame_app_head;
    mac_head_t frm_mac_head;
    uint8_t unpack_buf[ONE_FRM_PLD_SIZE];
    uint8_t frm_res = data_len % 5;
    app_package_t app_package;
    app_package.payload_len = data_len;
    app_package.reserve = 0;
    
    if(frm_res == 0)
        app_package.package_numb = data_len / 5;
    else
        app_package.package_numb = (data_len / 5) + 1;
    
    osel_memcpy(unpack_buf, (uint8_t *)&app_package, sizeof(app_package_t));
    
    for(uint16_t i = 0; i < app_package.package_numb; i++)
    {
        app_data_buf_t *data_buf = NULL;
        data_buf = (app_data_buf_t *)mpool_alloc(&mpool_app_manage, 
                                                 APP_MBUF_SIZE __MLINE);
        if(data_buf == NULL)
        {
            DBG_LOG(DBG_LEVEL_INFO, "mpool alloc fail!");
            return;
        }
        data_buf->data_type = FRM_TYPE_CONFIRM;
        data_buf->date_p = (uint8_t *)data_buf + sizeof(app_data_buf_t);
        if(i == 0)
        {
            fill_app_head(&frame_app_head, app_type, app_sub_type, 
                          TYPE_APP_FRM_TRANS_START);
            
            fill_mac_head(&frm_mac_head, TYPE_MAC_FRM_DATA, data_p, ONE_FRM_PLD_SIZE);
            data_buf->data_len = fill_data_buf(data_buf->date_p, frame_app_head, 
                                               frm_mac_head, unpack_buf, ONE_FRM_PLD_SIZE);
            
            if(data_buf->data_len + data_buf->date_p > (uint8_t *)data_buf + APP_MBUF_SIZE)
            {
                DBG_LOG(DBG_LEVEL_INFO, "copy data over len!");
                mpool_free((void **)&data_buf __MLINE);
                return;
            }
            lsn_databuf_insert(data_buf);
            osel_memset(unpack_buf, 0x00, ONE_FRM_PLD_SIZE);
        }
        
        if(frm_res == 0)
            data_len = ONE_FRM_PLD_SIZE;
        else
            data_len = frm_res;
        
        if(i < app_package.package_numb - 1)
        {
            unpack_package(unpack_buf, i, (i*5 + data_p), ONE_FRM_PLD_SIZE - 1);
            
            fill_app_head(&frame_app_head, app_type, app_sub_type, 
                          TYPE_APP_FRM_TRANS_RUNNING);
            
            fill_mac_head(&frm_mac_head, TYPE_MAC_FRM_DATA, data_p, ONE_FRM_PLD_SIZE);
            
            if(data_buf->data_len + data_buf->date_p > (uint8_t *)data_buf + APP_MBUF_SIZE)
            {
                DBG_LOG(DBG_LEVEL_INFO, "copy data over len!");
                mpool_free((void **)&data_buf __MLINE);
                return;
            }
            
            lsn_databuf_insert(data_buf);
            osel_memset(unpack_buf, 0x00, ONE_FRM_PLD_SIZE);
        }
        else
        {
            unpack_package(unpack_buf, i, (i*5 + data_p), ONE_FRM_PLD_SIZE - 1);
            
            fill_app_head(&frame_app_head, app_type, app_sub_type, 
                          TYPE_APP_FRM_TRANS_END);
            
            fill_mac_head(&frm_mac_head, TYPE_MAC_FRM_DATA, data_p, data_len);
            
            if(data_buf->data_len + data_buf->date_p > (uint8_t *)data_buf + APP_MBUF_SIZE)
            {
                DBG_LOG(DBG_LEVEL_INFO, "copy data over len!");
                mpool_free((void **)&data_buf __MLINE);
                return;
            }
            
            lsn_databuf_insert(data_buf);
            osel_memset(unpack_buf, 0x00, ONE_FRM_PLD_SIZE);
        }
    }
}
 
void app_send_data(frame_app_t frame_app_data)
{
    uint8_t data_buf[FRM_DATA_MAX];
    uint8_t *data_p = data_buf + sizeof(frame_app_head_t);
    uint8_t data_len = 0;
    uint8_t app_sub_type = 0;
    
    switch(frame_app_data.app_ctrl.dev_type)
    {
    case TYPE_APP_FRM_ALARM:
        data_len = app_alarm_data_fill(data_buf, frame_app_data.app_ctrl, 
                                       frame_app_data.seq_num, 
                                       &frame_app_data.rtc_val);
        break;
        
    case TYPE_APP_FRM_FEATURE:
        data_len = app_feature_data_fill(data_buf, frame_app_data.app_ctrl, 
                                       frame_app_data.seq_num, 
                                       &frame_app_data.feature_val);
        break;
        
    case TYPE_APP_FRM_ORIGINAL_DATA:
        data_len = app_origin_data_fill(data_buf, frame_app_data.app_ctrl, 
                                       frame_app_data.seq_num, 
                                       &frame_app_data.origin_data_val);
        break;
        
    case TYPE_APP_FRM_HB:
        data_len = app_hb_data_fill(data_buf, frame_app_data.app_ctrl, 
                                       frame_app_data.seq_num, 
                                       &frame_app_data.hb_info);
        app_sub_type = frame_app_data.hb_info.hb_type;
        break;
        
    case TYPE_APP_FRM_CONFIG_REQUEST:
        data_len = app_config_data_fill(data_buf, frame_app_data.app_ctrl, 
                                       frame_app_data.seq_num, 
                                       &frame_app_data.config_info_ack);
        break;
        
    default:
        break;
    }
    if(data_len <= ONE_FRM_LEN_MAX)
    {
        app_one_frm_send(data_p, data_len - 2 , 
                         frame_app_data.app_ctrl.dev_type, 
                         app_sub_type);
    }
    else
    {
        app_more_frm_send(data_p, data_len - 2 ,
                          frame_app_data.app_ctrl.dev_type, 
                          app_sub_type);
    }
}

/******************************************************************************/
static void app_can_send_tx_confirm(bool_t res)
{  
    app_can_send = TRUE;
    if(app_join_flag)
        return;
    
    app_data_buf_t *app_data_buf = lsn_databuf_look();
    if(res == TRUE)
    {
        app_continuous_lost_frm = 0;
        app_data_buf = lsn_databuf_remove();
        mpool_free((void **)&app_data_buf __MLINE);
        
    }
    else
    {
        if(app_data_buf->send_cnt >= RESEND_MAX_CNT)
        {
            app_data_buf_t *app_data_buf = lsn_databuf_remove();
            mpool_free((void **)&app_data_buf __MLINE);
        }
        else
        {
            app_continuous_lost_frm ++;
        }
    }
    if(app_continuous_lost_frm == LOST_MAX_CNT)
    {
        hal_board_reset();
    }
}

static void app_rx_indicate(const void *buffer, uint8_t len)
{
    if(recv_frm_handle_cb != NULL)
        recv_frm_handle_cb((uint8_t *)buffer, len);
}

static void app_join_confirm(bool_t res)
{
    app_can_send = TRUE;
    app_join_flag = TRUE;
    osel_pthread_create(app_task, &app_send_hb_process, NULL);
}

static void wait_ack_timeout(void)
{
    if(app_can_send == FALSE)
    {
        app_can_send_tx_confirm(FALSE);
    }
    else
        DBG_ASSERT(FALSE __DBG_LINE);
}

static void can_data_recv_handler(uint8_t  *data_p, uint8_t data_len)
{
    mac_head_t mac_head = *(mac_head_t *)data_p;
    uint8_t tmp_buf[10] = {0};
    if(data_len >= sizeof(mac_head))
        osel_memcpy(tmp_buf, data_p + sizeof(mac_head), 
                    data_len - sizeof(mac_head));
    if(mac_head.frm_type == TYPE_MAC_FRM_NWK_REQ)
        app_join_confirm(TRUE);
    
    else if(mac_head.frm_type == TYPE_MAC_FRM_DATA)
    {
        bool_t cancel_res = FALSE;
        cancel_res = osel_etimer_disarm(&app_wait_ack_etimer);
        app_can_send_tx_confirm(TRUE);
        if(cancel_res == FALSE)
            DBG_LOG(DBG_LEVEL_ORIGIN, "cancel false!");
    }
    else if(mac_head.frm_type == TYPE_MAC_FRM_DATA)
        app_rx_indicate(tmp_buf, data_len - sizeof(mac_head));
}


static void can_send(msg_flag_t flag, app_data_buf_t *app_can_data)
{
    mac_head_t data_head = *(mac_head_t *)app_can_data->date_p;
    
    can_send_data(app_can_data->date_p + FRM_MAC_HEAD_LEN, 
                      app_can_data->data_len - FRM_MAC_HEAD_LEN,
                      data_head.frm_type);
    if(flag == MSG_CONFIRMED)
    {
        osel_etimer_arm(&app_wait_ack_etimer, 200 / OSEL_TICK_PER_MS, 0);
    }
    else if(flag == MSG_UNCONFIRMED)
    {
        app_can_send_tx_confirm(TRUE);
    }
    
}
PROCESS(app_ack_timeout_process, "app_ack_timeout_process");
PROCESS_THREAD(app_ack_timeout_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == A_ACK_TIMEOUT);
        wait_ack_timeout();
    }
    PROCESS_END();
}

PROCESS(app_join_req_process, "app_join_req_process");
PROCESS_THREAD(app_join_req_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    OSEL_ETIMER_DELAY(&delay_timer, 10);
    while (1)
    {
        app_send_join_frm();
        OSEL_ETIMER_DELAY(&delay_timer, 20000 / OSEL_TICK_PER_MS);
    }
    PROCESS_END();
}

PROCESS(app_send_process, "app_send_process");
PROCESS_THREAD(app_send_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    static app_data_buf_t *lsn_data_sbuf;
    while (1)
    {
        OSEL_ETIMER_DELAY(&delay_timer, 200 / OSEL_TICK_PER_MS);
        if(lsn_databuf_cnt() == 0 || app_can_send == FALSE 
           || app_join_flag == FALSE)
        {
            continue;
        }
        OSEL_ETIMER_DELAY(&delay_timer, 3000 / OSEL_TICK_PER_MS);
        lsn_data_sbuf = lsn_databuf_look();
        app_can_send = FALSE;
        lsn_data_sbuf->send_cnt ++;
        if(lsn_data_sbuf->data_type == FRM_TYPE_CONFIRM)
            can_send(MSG_CONFIRMED, lsn_data_sbuf);
        else
            can_send(MSG_UNCONFIRMED, lsn_data_sbuf);
    }

    PROCESS_END();
}

void app_module_send_init(recv_frm_cb handler, osel_task_t *task)
{
    config_module_read(&app_cfg_device_info_local);
    mpool_init(&mpool_app_manage, APP_MBUF_NUM, APP_MBUF_SIZE);
    lsn_databuf_init();
    DBG_ASSERT(handler != NULL __DBG_LINE);
    DBG_ASSERT(task != NULL __DBG_LINE);
    
    app_task = task;
    recv_frm_handle_cb = handler;
    can_init((uint8_t)app_cfg_device_info_local.device_id, 
             HOST_DEVICE_ID);
    can_int_cb_reg(can_data_recv_handler);
    osel_pthread_create(task, &app_join_req_process, NULL);
    osel_pthread_create(task, &app_send_process, NULL);
    osel_etimer_ctor(&app_wait_ack_etimer, &app_ack_timeout_process, 
                     A_ACK_TIMEOUT, NULL);
}
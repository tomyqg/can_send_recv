/**
 * @brief       :
 *
 * @file        : prim.h
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 * 2015/09/07  v0.0.2      gang.cheng    change union to struct, delete same
 *                                       elements
 */

#ifndef __PRIM_H__
#define __PRIM_H__

#include "pbuf.h"

#define NWK_TASK_PRIO               (5u)
#define MAC_TASK_PRIO               (6u)

typedef enum
{
    N2M_DATA_REQUEST = 1,
    N2M_JOIN_RESPONE,
    N2M_DATA_RESEND,
    M2M_DATA_RESEND,
    M2M_ASSOC_REQUETS,

    M2N_DATA_CONFIRM,
    M2N_DATA_INDICATION,
    M2N_ASSOC_CONFIRM,
    M2N_UNASSOC_INDICATION,

    N2N_HB_REQUEST,

    N2A_DATA_CONFIRM,
    N2A_DATA_INDICATION,
    N2N_DATA_RESEND,
    N2A_DATA_RESEND,
    N2A_JOIN_CONFIRM,
    N2A_UNJOIN_INDICATION,

    A2N_DATA_REQUEST,
    A2A_HB_REQUEST,

} prim_type_t;


#include "sys_arch/osel_arch.h"

typedef enum
{
    /* M_TRAN */
    M_TRAN_RESEND_TIMEOUT_EVENT = ((MAC_TASK_PRIO << 8) | 0x01),
    M_TRAN_CSMA_TIMEOUT_EVENT,
    M_TRAN_RXOK_EVENT,
    M_TRAN_TXOK_EVENT,
    M_TRAN_RXOVR_EVENT,
    M_TRAN_TX_ERROR_EVENT,
    M_TRAN_TXUND_EVENT,
    M_TRAN_ACK_TIMEOUT_EVENT,
    M_TRAN_BACKOFF_SUCCESS_EVENT,
    M_TRAN_BACKOFF_FAIL_EVENT,
    M_TRAN_RESUME_EVENT,

    /* M_SYNC */
    M_SYNC_BACKGROUND,
    //    M_SYNC_CALCULATE_EVENT,

    /* M_SLOT */
    M_SLOT_TIMEOUT_EVENT,

    /** USER */
    M_PEND_SEND_EVENT,
    M_PEND_RECV_EVENT,
    M_DATA_REQ_EVENT,

} mac_task_sig_enum_t;

typedef struct
{
    uint32_t src_addr;          //*< 帧的mac源地址
    uint32_t dst_addr;          //*< 帧的mac目的地址

    void *msdu;
    uint8_t msdu_length;

    uint8_t src_mode : 4,
            dst_mode : 4;

    bool_t status;
} mac_prim_arg_t;


typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;

    uint64_t key;

    void *nsdu;
    uint8_t nsdu_length;

    int8_t rssi;
    bool_t status;
} nwk_prim_arg_t;

typedef struct
{
    mac_prim_arg_t mac_prim_arg;        //mac层原语参数
    nwk_prim_arg_t nwk_prim_arg;        //nwk层原语参数
} prim_arg_t;

typedef struct
{
    prim_arg_t prim_arg;
    pbuf_t *pbuf;
} prim_args_t;


#endif


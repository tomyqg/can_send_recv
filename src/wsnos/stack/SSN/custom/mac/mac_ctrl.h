#ifndef __MAC_CTRL_H__
#define __MAC_CTRL_H__

#include "mac_packet.h"

typedef struct
{
    uint16_t mac_send_num;
    uint16_t mac_success_num;
    uint16_t mac_resent_num;
    uint16_t mac_cca_num;

    uint8_t success_rate;
    uint8_t avg_retrans_num;
    uint8_t avg_cca_num;
    int8_t  father_rssi;
} mac_trans_info_t;

//*< mac帧处理函数
typedef void (*frame_switch_t)(pbuf_t *const pbuf);

extern mac_pib_t mac_pib_info;
extern mac_trans_info_t g_trans_info;
extern mac_head_t mac_head_info;

static inline uint32_t mac_pib_coord_saddr(void)/**< 获取上级设备短地址 */
{
    return mac_pib_info.mac_coord_addr;
}

static inline int8_t mac_pib_coord_rssi(void)
{
    return mac_pib_info.mac_coord_rssi;
}

static inline uint64_t mac_pib_self_saddr(void)
{
    return mac_pib_info.mac_self_addr;
}

static inline uint32_t mac_pib_src_saddr(void)
{
    return mac_pib_info.mac_src_addr;
}

static inline uint8_t mac_pib_hops(void)
{
    return mac_pib_info.mac_hops;
}

/**
 * @brief 获取传输信息：传输成功率以及平均传输次数
 */
mac_trans_info_t mac_trans_info_get(void);

/**
 * @brief 清空传输信息，为下一次信息获取做准备
 */
void mac_trans_info_clr(void);


/**
 *  @brief      注册传输模块回调
 *  @para       mac_beacon
 *  @para       mac_data
 *  @para       mac_ack
 *  @para       mac_command
 *  @return     void
 */
void mac_ctrl_switch_init(frame_switch_t mac_data,
                          frame_switch_t mac_ack,
                          frame_switch_t mac_command);

/**
 *  @brief      注册传输模块回调
 *  @return     void
 */
void mac_ctrl_init(void);

/**
 *  @brief      获取帧里的源地址
 *  @para       pbuf 帧存放的pbuf
 *  @para       addr 获取到的地址
 *  @return     void
 */
// uint8_t get_addr(pbuf_t *pbuf, uint32_t *addr);

/**
 *  @brief      pib属性初始化
 *  @return     void
 */
void mac_pib_init(mac_pib_t *pib);

/*
 *@brief 设置pib属性
 */
void mac_pib_set(mac_pib_t *mac_pib);

#endif

/**
 * @brief       : this is general ID rules.
 * @file        : id_rules.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-08-25
 * change logs  :
 * Date       Version     Author        Note
 * 2016-08-25  v0.0.1  gang.cheng    first version
 */

#ifndef __ID_RULES_H__
#define __ID_RULES_H__

#include "common/lib/data_type_def.h"


#pragma pack(1)
enum
{
    NAME_ITCS = 1,  //*< 地磁系列产品
    NAME_ISPS = 2,  //*< 安防系列产品
    NAME_SCCP = 3,  //*< 感知网络系列产品
    NAME_ILSS = 4,  //*< 感知物流系列产品
    NAME_RESERVED,  //*< 预留
};

typedef enum
{
    CATE_ITCS_C = 0,     //*< 通用应用
    CATE_ITCS_M = 1,     //*< 车流量检测应用
    CATE_ITCS_S = 2,     //*< 测速应用
    CATE_ITCS_SM = 3,    //*< 流量/测速
    CATE_ITCS_P = 4,     //*< 电子警察应用
    CATE_ITCS_R1 = 5,    //*< 预留
    CATE_ITCS_R2 = 6,    //*< 预留
    CATE_ITCS_T = 7,     //*< 停车位应用
} cate_itcs_e;

typedef enum
{
    CATE_ISPS_C = 0,    //*< 通用应用
    CATE_ISPS_B = 1,    //*< 砖墙应用
    CATE_ISPS_F = 2,    //*< 栅栏应用
    CATE_ISPS_R,        //*< 预留
} cate_isps_e;

typedef enum
{
    CATE_ILSS_C = 0,    //*< 通用应用
    CATE_ILSS_L = 1,    //*< 货物质押
    CATE_ILSS_R,        //*< 预留
} cate_ilss_e;

typedef struct
{
    uint8_t category : 4,
            name     : 4;
} product_info_t;

typedef struct
{
    uint8_t minor : 4,  //*< 小的分类，1~15
            major : 4;  //*< 大的分类，1~9
} device_type_t;

typedef struct
{
    uint8_t ver;
} version_t;

/**
 * @brief 同一批生产的所有设备的批次号相同
 *        对应生产年月日的BCD码，如16年8月25号，为0x160825
 */
typedef struct 
{
    uint8_t year;   
    uint8_t month;
    uint8_t day;
} product_batch_t;

/**
 * 检测器：0x0000~0x7999
 * 中继器: 0x8000~0x8999
 * 主机  : 0x9000~0x9999 
 */
typedef struct
{
    uint16_t num;   //*< 生产序号的BCD码，如第358个设备，对应为0x0358 
} serial_num_t;

typedef struct
{
    product_info_t  pd_info;
    device_type_t   dev_type;
    version_t       ver;
    product_batch_t pd_batch;
    serial_num_t    ser_num;
} identifiers_t;

#pragma pack()

/**
 * @brief 把8字节的通用设备ID转换成地磁3.0系统网内4字节ID
 * @param[in]  array 存放8字节通用设备ID的数组
 * @param[in]  num   id的长度，默认是8字节
 * @param[out] id    转换后的ID，还是以大端保存
 * @return       如果8字节ID符合规则，则转换成功，否则失败
 */
bool_t id_general(uint8_t array[], uint8_t num, uint32_t *id);


#endif
